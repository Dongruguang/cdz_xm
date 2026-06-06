
#include "work.h"

static thread_mq_t event_mq;
static thread_mq_t order_mq;
static int mq_init_flag = 0;

static void init( void )
{
	
	if( 1 == mq_init_flag ) return;
	//用于sock向外发送事件
	event_mq = thread_mq_init("event_mq", sizeof(event_t), 20);
	//用于向sock发送命令
	order_mq = thread_mq_init("order_mq", sizeof(order_t), 20);
	mq_init_flag = 1;
}


static void copy_ddh_to_order( char* ddh,order_pt order )
{
	int i = 0;
	while( ddh[i] != '\0' )
	{
		order->ddh[i] = ddh[i];
		i++;
		if( i>=16 ) break;
	}
	order->ddh[16] = '\0';
}
static void copy_ddh_to_event( char* ddh,event_pt event )
{
	int i = 0;
	while( ddh[i] != '\0' )
	{
		event->ddh[i] = ddh[i];
		i++;
		if( i>=16 ) break;
	}
	event->ddh[16] = '\0';
}


//给插座控制器发送控制命令
/*------------------------------------------------------------------------*/

static void send_order( char* ddh,int num, int cmd, uint32_t dat  )
{
	int ret = 0;
	order_t order;
	copy_ddh_to_order( ddh,&order );
	order.num = num;
	order.cmd = cmd;
	order.dat = dat;
	if( mq_init_flag == 0 ) return;
	ret = thread_mq_send( order_mq,&order, sizeof(order_t), 0x01 );
	if( ret == bFALSE )
	{
		sysprt->aerr("send_order \r\n");
	}
}

//插座控制器申报事件
static void put_event( char* ddh,int type,int num, int val, uint32_t dat,double curr  )
{
	int ret;
	event_t event;
	copy_ddh_to_event( ddh,&event );
	event.type = type;			//事件号		
	event.num = num;				//事件码
	event.val = val;				//事件具体值,没有则为0
	event.dat = dat;				//事件涉及到的整数
	event.curr = curr;			//事件涉及到的浮点数
	if( mq_init_flag == 0 ) return;
	thread_mq_send( event_mq,&event, sizeof(event_t), 0x01 );
	if( ret == bFALSE )
	{
		sysprt->aerr("put_event \r\n");
	}
}
/*------------------------------------------------------------------------*/

static int recv_order( order_pt order )
{
	if( mq_init_flag == 0 ) return -1;
	int ret = thread_mq_recv( order_mq, order, sizeof(order_t), 0);
	if(ret != bFALSE) 
		return 0;
	else
		return -1;
	
}

static int recv_event( event_pt event )
{
	if( mq_init_flag == 0 ) return -1;
	int ret = thread_mq_recv( event_mq, event, sizeof(event_t), 0);
	if(ret != bFALSE) 
		return 0;
	else
		return -1;
	
}

/*------------------------------------------------------------------------*/
static sock_mq_t do_sock_mq ={
	init,
	send_order,
	put_event,
	recv_order,
	recv_event
};

sock_mq_pt sock_mq = &do_sock_mq;
