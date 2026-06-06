#include "framework.h"

#define PRT_LOG(X...)	sysprt->alog//dprintf(X)	

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

uart_pt uart2 = &do_uart;
loopbuf_pt loopbuf_tx2;
loopbuf_pt loopbuf_rx2;

extern void hlw_recv_callback_u2(uint8_t dat);

/*--------------------------------------------------------------------------------*/
static void init( uint32_t baudrate )
{
	//创建环形缓冲区
	loopbuf_tx2 = loop->create();

	loopbuf_rx2 = loop->create();
	//初始化串口
	drvp_uart = drvp_uart2;	//引用串口硬件
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
		loop->write( loopbuf_tx2,buf[i] );
	}
}

static void deal_data_send(void)
{
	//一次请求读10个数据
	uint8_t buf[10];
	int ret = loop->read( loopbuf_tx2,buf,10 );
	
	if( ret<=0 ) return;
	
	//发送出去
	for( int i=0;i<ret;i++ )
		drvp_uart->Write( buf[i] );	
}

static uint32_t read_recv_buf(uint8_t *buf,uint8_t len)
{
		return loop->read( loopbuf_rx2,buf,len );
}
static void clr_rbuf( void )
{
	
	drvp_uart->Close();
	loop->reset( loopbuf_rx2 );
	drvp_uart->Open();
}
static void en_irq( void )	{ drvp_uart->EnableIRQ();  }
static void dis_irq( void ) { drvp_uart->DisableIRQ(); }	

/*--------------------------------------------------------------------------------*/

//接收中断回调函数
extern void irq_log(char* format,...);
static void recv_callback(uint8_t dat)
{
	
	//把数据往缓冲区里面扔
	loop->write( loopbuf_rx2,dat );
}




