#include "framework.h"

#define PRT_LOG(X...)	printf(X)//sysprt->alog	

extern loop_pt loop;//使用环形缓冲区

static void init( uint32_t baudrate );
static void start( void );
static void stop( void );

static void send(uint8_t *buf,uint8_t len);
static void req_data_send(uint8_t *buf,uint8_t len);
static void deal_data_send(void);
static uint32_t read_recv_buf(uint8_t *buf,uint8_t len);
static void clr_rbuf( void );

static void en_irq( void );	
static void dis_irq( void );


static void recv_callback(uint8_t dat);

static drv_uart_port_pt drvp_uart;

static uart_t do_uart={
	.init = init,
	.start = start,
	.stop = stop,

	.send = send,
	.req_data_send = req_data_send,
	.deal_data_send = deal_data_send,
	.read_recv_buf = read_recv_buf,
	
	.clr_rbuf = clr_rbuf,
	
	.en_irq = en_irq,
	.dis_irq = dis_irq,
};

uart_pt uart3 = &do_uart;
loopbuf_pt loopbuf_tx3;
loopbuf_pt loopbuf_rx3;

extern void hlw_recv_callback_u3(uint8_t dat);

static thread_mq_t rx_mq;
static int mq_init_flag = 0;
/*--------------------------------------------------------------------------------*/
static void init( uint32_t baudrate )
{
	//创建环形缓冲区
	loopbuf_tx3 = loop->create();
	//loopbuf_rx3 = loop->create();
	
	rx_mq = thread_mq_init("prt_mq", sizeof( uint8_t ), 128 );
	mq_init_flag = 1;
	
	
	//初始化串口
	drvp_uart = drvp_uart3;	//引用串口硬件
	drvp_uart->Init(baudrate);
	drvp_uart->Open();
	drvp_uart->SetIntRxFunc( recv_callback );
	
}

static void start( void )
{
	drvp_uart->Open();
}
static void stop( void )
{
	drvp_uart->Close();
}


static void send(uint8_t *buf,uint8_t len)
{
	//发送出去
	for( int i=0;i<len;i++ )
		drvp_uart->Write( buf[i] );
}
static void req_data_send(uint8_t *buf,uint8_t len)
{
	for( int i=0;i<len;i++ )
	{
		loop->write( loopbuf_tx3,buf[i] );
	}
}

static void deal_data_send(void)
{
	//一次请求读10个数据
	uint8_t buf[10];
	int ret = loop->read( loopbuf_tx3,buf,10 );
	
	if( ret<=0 ) return;
	
	//发送出去
	for( int i=0;i<ret;i++ )
		drvp_uart->Write( buf[i] );
	
}

static uint32_t read_recv_buf(uint8_t *buf,uint8_t len)
{
//	return loop->read( loopbuf_rx3,buf,len ); 
	int ret = 0;
	int i = 0;
	if( 0 == mq_init_flag ) return i;
	for( i=0;i<len;i++ )
	{
		ret = thread_mq_recv( rx_mq, &buf[i], sizeof( uint8_t ), 0 );
		if( ret == bFALSE ) break;
	}
	return i;
}
static void clr_rbuf( void )
{
	int ret = 0;
	uint8_t dat = 0;
	
//	drvp_uart->DisableIRQ();
	drvp_uart->Close();
	while( 1 )
	{
		ret = thread_mq_recv( rx_mq, &dat, sizeof( uint8_t ), 0 );
		if( ret == bFALSE ) break;
	}
	
//	drvp_uart->EnableIRQ();
	drvp_uart->Open();
}
static void en_irq( void )	{ drvp_uart->EnableIRQ(); }
static void dis_irq( void ) { drvp_uart->DisableIRQ(); }	


/*--------------------------------------------------------------------------------*/
//接收中断回调函数
static void recv_callback(uint8_t dat)
{
	//把数据往缓冲区里面扔
	//loop->write( loopbuf_rx3,dat );
////	PRT_LOG("%c ",dat);
	if( 0 == mq_init_flag ) return;
	thread_mq_irq_send( rx_mq, &dat, sizeof( uint8_t ), 0 );

}




