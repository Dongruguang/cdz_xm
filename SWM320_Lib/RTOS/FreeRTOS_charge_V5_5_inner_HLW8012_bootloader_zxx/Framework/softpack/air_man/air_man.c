#include "framework.h"
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	



/*---------------------------------------------------------------------------------*/

//创建一个AT设备
static air_dev_pt create_dev( uart_pt uart )
{
	//分配内存作为设备结构
	air_dev_pt at_dev = mem_alloc( sizeof( air_dev_t ) );
	
	//查看分配是否成功
	if( 0 == at_dev  )
	{	
		PRT_LOG("err:create_dev()-> mem_alloc( sizeof( at_dev_t ) )\r\n");
	}
	
	//设置串口
	at_dev->uart = uart;
	
	//分配接收缓冲区
	at_dev->rxbuf = mem_alloc( sizeof( uint8_t ) * 4096 );
	//查看分配是否成功
	if( 0 == at_dev->rxbuf  )
	{
		PRT_LOG("err:create_dev()-> mem_alloc(sizeof( uint8_t ) * 4096 ) \r\n");
	}
	
	return at_dev;
}

/*---------------------------------------------------------------------------------*/

//销毁一个AT设备
static void delete_dev( air_dev_pt dev	)
{
	
	mem_free( dev );
	
}

/*---------------------------------------------------------------------------------*/
//发送字符串
static void send_str(air_dev_pt dev,char *string)
{
	uint32_t ptr=0;
	while ( string[ptr] != '\0' )
	{
		dev->uart->send( (uint8_t*)(&string[ptr]),1 );
		ptr++;
	}
}

/*---------------------------------------------------------------------------------*/

static void clr_rxbuf( air_dev_pt dev )
{
	uint8_t* rxbuf = dev->rxbuf;							//接收缓冲区指针，指针会被修改
	//先把接收缓冲区清空
	while( 1 )
	{
		if( 0 == dev->uart->read_recv_buf( rxbuf,1 ) )
		{
			//PRT_LOG("clr uart rxbuf\r\n");
			break;
		}
	}
}
/*--------------------------------------------------------------------------------*/


//hex 
//=0/NULL 说明是默认格式字符格式
//
static void ptf_rxbuf( air_dev_pt dev , int hex )
{
	uint32_t max = dev->rxlen;
	if( 0 == wdat->storage->net_ptf ) return ;
	if( max >= 4096 )
	{
		PRT_LOG("err: printf_rxbuf,len =%08d \r\n",dev->rxlen );
		return;
	}
	PRT_LOG("---------------<\r\n");
	if( hex )
	{
		
		for( int i=0;i<max;i++ )
		{
			PRT_LOG("0x%02x ",dev->rxbuf[i]);
		}
	}
	else
	{
		for( int i=0;i<max;i++ )
		{
			PRT_LOG("%c",dev->rxbuf[i]);
		}
	}
	
}


/*--------------------------------------------------------------------------------*/

static air_man_t do_air_man={ create_dev,delete_dev,send_str,clr_rxbuf,ptf_rxbuf };

air_man_pt air_man = &do_air_man;






