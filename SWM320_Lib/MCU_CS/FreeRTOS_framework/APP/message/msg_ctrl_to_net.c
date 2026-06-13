
#include "message.h"
#include "work.h"

/*
实现一个消息队列，用于  控制任务 传输消息给 网络任务 



*/

enum{
	_e_type_curr = 0,			//电流报文
	_e_type_over,					//结束事件报文
	
};

typedef struct{
	uint32_t type;				//消息类型
	uint32_t num;					//报文序号
	uint32_t sock;				//= 1 左边插座;= 2 右边插座 从1开始数数
	float curr;						//电流值
	float	pwr;						//功率值
	float	vol;						//电压值
	uint32_t usetime;			//已充时间
	uint32_t rec;					//保留
	
	char ddh[32];					//订单号
}curr_rp_t;

static thread_mq_t mq_local;

static int init_flag = 0;

static void init( void )
{
	if( init_flag != 0 ) return;
	
	mq_local = thread_mq_init( "mq_ctn",MsgLen,20 );
	
	
	init_flag = 1;
}

//发送一条电流报告
static void put_curr_report( char* ddh,int num,int road,float curr,float pwr,float vol,uint32_t usetime )
{
	if( init_flag == 0 ) return;
	
	if( wdat->storage->net_mode == NetMode_Not ) return;
	
	//判断类型长度是否跟MsgLen一致
	if( sizeof( order_dat_t ) != MsgLen )
	{
		sysprintf( "err:%s type len %d != MsgLen %d\r\n",
			__func__,sizeof( order_dat_t),MsgLen );
		return ;
	}
	
	//组装数据
	curr_rp_t dat;
	dat.type = _e_type_curr;
	
	memset( dat.ddh,0,sizeof( dat.ddh ) );
	snprintf( dat.ddh,sizeof( dat.ddh ),"%s",ddh );
	
	dat.num = num;
	dat.sock = road+1;
	dat.curr = curr;
	dat.pwr = pwr;
	dat.vol = vol;
	dat.usetime = usetime;

	int ret = thread_mq_send( mq_local,&dat,MsgLen,0x01 );
	if( ret == pdTRUE )
	{//说明发送成功了，意思时消息已经入队。
		
	}
	else
	{//说明发送超时了
		sysprintf("warning:%s(%d)\r\n",__func__,road );
	}
}

typedef struct{
	uint32_t type;				//消息类型
	float ekwh;						//充电过程所消耗的电能
	uint32_t time;				//充电时长 分钟
	uint32_t event;				//订单结束的原因
	uint32_t ret[4];			//保留
	
	char ddh[32];					//订单号
}over_rp_t;


//发送一条结束事件报告
static void put_over_report( char* ddh,float ekwh,uint32_t time,uint32_t event )
{
	if( init_flag == 0 ) return;
	
	if( wdat->storage->net_mode == NetMode_Not ) return;
	
	//判断类型长度是否跟MsgLen一致
	if( sizeof( over_rp_t ) != MsgLen )
	{
		sysprintf( "err:%s type len %d != MsgLen %d\r\n",
			__func__,sizeof( over_rp_t),MsgLen );
		return ;
	}
	
	//组装数据
	over_rp_t dat;
	dat.type = _e_type_over;
	
	memset( dat.ddh,0,sizeof( dat.ddh ) );
	snprintf( dat.ddh,sizeof( dat.ddh ),"%s",ddh );
	
	dat.ekwh = ekwh;
	dat.time = time;
	dat.event = event;
	
	
	
	int ret = thread_mq_send( mq_local,&dat,MsgLen,0x01 );
	if( ret == pdTRUE )
	{//说明发送成功了，意思时消息已经入队。
		
	}
	else
	{//说明发送超时了
		sysprintf("warning:%s\r\n",__func__ );
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


static char obuf[ 256 ];
static void work( void *ndev )
{
	
	//防止队列没有初始化
	if( init_flag == 0 ) return;
	
	//接收消息
	uint8_t buf[ MsgLen ];
	
	int ret = get( buf,sizeof( buf ) );
	if( ret < 0 ) return;
	
	//根据消息的类型，把消息解析出来，发送给服务器
	
	//对消息模板化
	curr_rp_t* curr_rp = (curr_rp_t*)buf;
	over_rp_t* over_rp = (over_rp_t*)buf;
	
	net_dev_t* netdev = (net_dev_t*)ndev;
	
	switch( curr_rp->type )
	{
		case _e_type_curr:
			memset( obuf,0,sizeof( obuf ) );
			//log data ddh num sock curr pwr vol usetime
			snprintf( obuf,sizeof( obuf ),"log data %s %d %d %f %f %f %d\r\n",
				curr_rp->ddh,
				curr_rp->num,
				curr_rp->sock,
				curr_rp->curr,
				curr_rp->pwr,
				curr_rp->vol,
				curr_rp->usetime
			);
			netdev_opt->send_str( netdev,obuf );
			sysprintf("-->send:%s\r\n",obuf );
			break;
		
		case _e_type_over:
			memset( obuf,0,sizeof( obuf ) );
			//log over ddh ekwh time event  
			snprintf( obuf,sizeof( obuf ),"log over %s %f %d %d\r\n",
				over_rp->ddh,
				over_rp->ekwh,
				over_rp->time,
				over_rp->event
				
			);
			netdev_opt->send_str( netdev,obuf );
			sysprintf("-->send:%s\r\n",obuf );
			break;
		
		default:
			break;
		
	}
}

static msg_ctn_opt_t do_msg_ctn_opt={
	.init = init,
	.put_curr_report = put_curr_report,
	.put_over_report = put_over_report,
	.work = work,
	
};

msg_ctn_opt_t* msg_ctn_opt = &do_msg_ctn_opt;

