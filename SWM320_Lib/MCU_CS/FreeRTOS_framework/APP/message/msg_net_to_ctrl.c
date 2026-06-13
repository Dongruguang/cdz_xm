
#include "message.h"

/*
实现一个消息队列，用于网络任务传输消息给 控制任务



*/

static thread_mq_t mq_local;

static int init_flag = 0;

static void init( void )
{
	if( init_flag != 0 ) return;
	
	mq_local = thread_mq_init( "mq_ntc",MsgLen,20 );
	
	
	init_flag = 1;
}

//发送一条消息
static void put_order( int type,int road,uint32_t time,char* ddh )
{
	if( init_flag == 0 ) return;
	
	//判断类型长度是否跟MsgLen一致
	if( sizeof( order_dat_t ) != MsgLen )
	{
		sysprintf( "err:%s type len %d != MsgLen %d\r\n",
			__func__,sizeof( order_dat_t),MsgLen );
		return ;
	}
	
	//组装数据
	order_dat_t dat;
	dat.type = type;
	dat.road = road;
	dat.time = time;

	memset( dat.ddh,0,sizeof( dat.ddh ) );
	snprintf( dat.ddh,sizeof( dat.ddh ),"%s",ddh );
	
	
	int ret = thread_mq_send( mq_local,&dat,MsgLen,0x01 );
	if( ret == pdTRUE )
	{//说明发送成功了，意思时消息已经入队。
		//相当于我们把数据写入环形缓冲区，但是此时，还没有消耗这一个数据
		
	}
	else
	{//说明发送超时了
		sysprintf("warning:%s(%d)\r\n",__func__,road );
	}
}


//接收一条消息
static int get( void* data,int len )
{
	//防止队列没有初始化
	if( init_flag == 0 ) return -1;
	
	//防止data的空间过小
	if( len <MsgLen ) return -2;
		
	int ret = thread_mq_recv(  mq_local,data,MsgLen,0 );
	if( ret == pdTRUE )
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

static msg_ntc_opt_t do_msg_ntc_opt={
	.init = init,
	.put_order = put_order,
	.get = get,
	
};

msg_ntc_opt_t* msg_ntc_opt = &do_msg_ntc_opt;


