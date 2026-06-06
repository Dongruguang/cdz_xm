#include "work.h"
#define PRT_LOG(X...)	//sysprt->alog(X)


#define Max_Pool	20		//交互池容量

#define no_using	0
#define is_using	1

pool_pt pool_head;
static uint32_t num_cntr = 0;

//初始化 交互池
static void init( void )
{
	int i=0;
	pool_head = NULL;
	
}

//注册 一个 交互项 到 交互池
static void register_one( pool_pt pool )
{
	pool_pt tpool;
	if( pool == NULL )
	{
		PRT_LOG( "error:register_one	\r\n " );
		return ;
	}			

	if( pool_head == NULL )
	{
		PRT_LOG( "register_one:	pool_head == NULL	\r\n " );
		PRT_LOG( "  pool->num =	%d \r\n ",pool->num		);
		
		pool_head = pool;
		pool_head->last = NULL;
		pool_head->next = NULL;
	}
	else
	{
		PRT_LOG( "register_one:	pool_head != NULL	\r\n " );
		PRT_LOG( "  pool->num =	%d \r\n ",pool->num		);
		//从头开始找尾巴
		tpool = pool_head;
		while( tpool->next != NULL )
		{
			tpool = tpool->next;
		}
		//添加pool 到尾巴
		tpool->next = pool;
		
		//tpool作为pool的上一项
		pool->last = tpool;
		
		//pool作为新的尾巴
		pool->next = NULL;
		
	}
}

//从 交互池 删除 一个 交互项
static int delate_one( pool_pt pool )
{
	pool_pt lpool,npool;		//分别描述获取到的上一项、下一项
	
	if( pool->last == NULL )
	{//说明 可能是链表头
		
		PRT_LOG( " pool->last == NULL \r\n " );
		if( pool->num != pool_head->num )
		{//说明不是链表头，为非法操作(表头这一项也是分配而来，num是唯一的)
			//不需要做任何操作
			PRT_LOG( "pool->num = %d \r\n ",pool->num );
			PRT_LOG( "pool_head->num = %d \r\n ",pool_head->num );
			
			
			PRT_LOG( "error: pool->last == NULL \r\n " );
			return -1;
		}
		
		if( pool->next == NULL )
		{//说明也是链表尾巴( 既是头，也是尾 )，整个表只有这1项	
			PRT_LOG( "pool->next == NULL \r\n " );
			pool_head = NULL;	//直接把pool_head设置为 NULL，说明没有 表项了 
			
		}
		else
		{//说明是表头，不是尾
			PRT_LOG( "it is head \r\n " );
			pool_head = pool_head->next;		//表头指针滑 到下一项
			pool_head->last = NULL;					//变成表头
		}
	}
	else if( pool->next == NULL )
	{//说明可能是链表尾巴
		PRT_LOG( "pool->next == NULL \r\n " );
		lpool = pool->last;	//获取链表尾巴的 上一项 
		lpool->next = NULL;	//将上一项 设置为链表尾巴
	}
	else
	{//说明是中间项 m
		PRT_LOG( "other \r\n " );
		lpool = pool->last;
		npool = pool->next;
		
		//传递pool->next 给 lpool->next
		lpool->next = pool->next;
		//传递lpool给 npool->last		
		npool->last = lpool;
		
	}
	return 1;
}

//创建一个交互项
static pool_pt create( uint32_t dest,uint32_t wait,event_pt event )
{
	
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	
	//分配内存
	pool_pt pool = mem_alloc( sizeof( pool_t ) );
	
	if( pool == NULL ) return NULL;
	
	/*------------------------------------*/
	//初始化交互项
	pool->last = NULL;
	
	pool->sta = 0;
	pool->num = num_cntr++;
	pool->dest_cntr = dest;
	pool->now_cntr = 0;
	pool->wait_tick = wait;
	pool->otick = get_sys_ticks();
	
	/*------------------------------------*/
	//拷贝event
	src = (uint8_t*)event;
	dst = (uint8_t*)( &( pool->event ) );
	
	for( int i=0;i<sizeof( event_t ) ;i++ )
	{
		dst[i] = src[i];
	}
	/*------------------------------------*/
	
	pool->next = NULL;
	PRT_LOG( "c pool->num = %d \r\n",pool->num );
	PRT_LOG( "c pool->event.ddh = %s \r\n",pool->event.ddh );
	
	
	
	return pool;
	
}

//销毁
static void destroy( pool_pt pool )
{
	/*------------------------------------*/
	//释放内存
	mem_free( pool );
	
}

//根据订单号找到 交互项
static pool_pt get_by_ddh( char* ddh )
{
	int i=0;
	pool_pt pool = pool_head;		//用于表示正在处理的对象，head -> ....
	while( pool != NULL )
	{
		//-----------------------------------
		//比较ddh是否一致
		for( i=0;i<MaxDDH_Len;i++ )
		{
			if( ddh[i] != pool->event.ddh[i] ) break;
		}
		
		if( i == MaxDDH_Len )
		{//说明ddh一致
			PRT_LOG( "find success:get_by_ddh->num = %d \r\n",pool->num );
			return pool;
		}
		
		//-----------------------------------
		if( pool->next != NULL ) 
			pool = pool->next;				//切换链表
		else 
			break;										//跳出循环
	}
	PRT_LOG( "find faild:get_by_ddh \r\n" );
	return NULL;

}

static void deal_delate( void )
{
	pool_pt pool = pool_head;		//用于表示正在处理的对象，head -> ....
	
	while( pool != NULL )
	{
		//处理需要删除的项
		if( pool->sta == 2 )
		{
			//从交互池中删除这一项
			ipool->delate_one( pool );
			ipool->destroy( pool );
			break;
		}
		//-----------------------------------
		if( pool->next != NULL ) 
			pool = pool->next;				//切换链表
		else 
			break;										//跳出循环
		
	}
	
}

static void work( void )
{
	pool_pt pool = pool_head;		//用于表示正在处理的对象，head -> ....
	time_t ntick = 0;
	//遍历链表，进行状态机交互
	while( pool != NULL )
	{
		//-----------------------------------
		switch( pool->sta )
		{
			case 0://如果存在请求，则往服务器发送数据
				//发送数据
				switch( pool->event.type )
				{
					//电流上报
					case _event_curr:	break;
						
					//插座事件
					case _event_sock:	event_report_sock( &(pool->event) ); break;
						
					//网络事件
					case _event_net:	event_report_net( &(pool->event) ); break;
					
					default:
						break;
				}
				
				//统计发送次数
				if( pool->dest_cntr > pool->now_cntr )
				{
					pool->now_cntr++;
					pool->sta = 1;			//等待一阵时间
				}
				else
				{
					pool->sta = 2;			//去删除本元素
				}
			
				break;
				
			case 1://等待 若干 滴答
				ntick = get_sys_ticks();
				if( pool->wait_tick > ( ntick - pool->otick ) ) break;
			
				pool->otick = ntick;
							
				pool->sta = 0;
				
				break;
			
			case 2://等待删除 自身对象
				
				break;
			
			default:
				break;
		}
		
		
		//-----------------------------------
		if( pool->next != NULL ) 
			pool = pool->next;				//切换链表
		else 
			break;										//跳出循环
		
	}

	deal_delate();
	
}




static ipool_t m_ipool = {
	init,
	register_one,
	delate_one,
	create,
	destroy,
	get_by_ddh,
	work
};

ipool_pt ipool = &m_ipool;

