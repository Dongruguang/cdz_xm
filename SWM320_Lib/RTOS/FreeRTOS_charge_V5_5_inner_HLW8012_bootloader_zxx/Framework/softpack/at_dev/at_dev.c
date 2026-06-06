#include "framework.h"
#define PRT_LOG(X...)	dprintf(X)	



/*---------------------------------------------------------------------------------*/

//创建一个AT设备
static at_dev_pt create_dev( uart_pt uart,uint32_t max_cmd )
{
	//分配内存作为设备结构
	at_dev_pt at_dev = mem_alloc( sizeof( at_dev_t ) );
	
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
static void delete_dev( at_dev_pt dev	)
{
	
	mem_free( dev );
	
}
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
//发送字符串
static void send_str(at_dev_pt dev,char *string)
{
	uint32_t ptr=0;
	while ( string[ptr] != '\0' )
	{
		dev->uart->send( (uint8_t*)(&string[ptr]),1 );
		ptr++;
	}
}

//hex 
//=0/NULL 说明是默认格式字符格式
//
static void printf_rxbuf( at_dev_pt dev , int hex )
{
	uint32_t max = dev->rxlen;
	if( max >= 4096 )
	{
		PRT_LOG("err: printf_rxbuf,len =%08d \r\n",dev->rxlen );
		return;
	}
	PRT_LOG("---------------\r\n")
	if( hex )
	{
		
		for( int i=0;i<max;i++ )
		{
			PRT_LOG("0x%02x ",dev->rxbuf[i])
		}
	}
	else
	{
		for( int i=0;i<max;i++ )
		{
			PRT_LOG("%c",dev->rxbuf[i])
		}
	}
	
}
static int auto_baudrate_one_cycle( at_dev_pt dev )
{
	uint8_t str[2] = {'A','T'};
	uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len
	uint8_t* rxbuf = dev->rxbuf;							//接收缓冲区指针，指针会被修改

	//先把接收缓冲区清空
	while( 1 )
	{
		if( 0 == dev->uart->read_recv_buf( rxbuf,1 ) )
		{
//			PRT_LOG("clr uart rxbuf\r\n");
			break;
		}
	}
	//发送命令
	dev->uart->send( str,2 );
	task_delay(50);
	
	//开始读数据
	dev->rxlen = 0;															
	while(1)
	{
		//读数据到dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );
		dev->rxlen += rd_len;
		
		//本轮没有收到任何数据
		if( rd_len == 0  ) 
		{
			
			if( ('A' == rxbuf[0]) && ( 'T' == rxbuf[1] )  )
			{
				
				return 0;
			}
			return -1;
		}
		
		
	}
}
static int aoto_baudrate( at_dev_pt dev,uint32_t cto )
{
	int ret = 0;
	uint8_t cntr = 0;
	while(1)
	{
		ret = auto_baudrate_one_cycle( dev );
		printf_rxbuf( dev,NULL );
		if( ret >= 0 )
		{//说明配对上了
			PRT_LOG("aoto_baudrate success \r\n");
			return 0;
		}
		cntr++;
		if( cntr>10 ) 
		{
			PRT_LOG("aoto_baudrate faild \r\n");
			return -1;
		}
	}
}

//AT
//返回 1  说明 命令返回的内容正确
//返回 0  说明命令返回的内容说明AT设备错误
//返回 -1 说明很久没有收到数据了
//返回 -2 说明命令超时了

//static char* str_enter = "\r\n";

static int send_cmd( at_dev_pt dev,char* at,uint32_t dto,uint32_t cto )
{
	int ret = 0;
	uint32_t err_tick = 0;										//描述错误滴答
	uint32_t old_tick = 0,now_tick = 0;				//tick 对										
	
	uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len
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
	//发送命令
	send_str( dev,at );
	//发送回车
	send_str( dev,"\r\n" );
	task_delay(50);
	
	//开始读数据
	
	dev->rxlen = 0;
	err_tick =0;
	old_tick = get_sys_ticks();
	while(1)
	{
		//读数据到dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );
		dev->rxlen += rd_len;
		
		//本轮没有收到任何数据
		if( rd_len == 0  ) 
		{
			ret = strlib->findstr( (char*)rxbuf,"\r\n");			
			if( ret > 0 )
			{//说明收到了回车符号
				if( 0 < strlib->findstr( (char*)rxbuf,"OK" ) )
				{//说明收到OK
					return 1;
				}
				if( 0 < strlib->findstr( (char*)rxbuf,"ERROR" ) )
				{//说明收到ERROR
					return 0;
				}	
			}
			
			if( '>' == rxbuf[dev->rxlen-1] ) return 3;//收到透传符号
			
			if( ( dto == 0 )||( dto == 0XFFFFFFFF ) )
			{//说明这条消息接收比较慢
				task_delay(10);
			}
			else
			{//说明这条消息接收比较快
				err_tick++;
				task_delay(1);
				if( err_tick > dto )
				{//说明数据接收超时了
					return -1;
				}
			}

			now_tick = get_sys_ticks();
			if( cto < ( now_tick - old_tick ) )
			{//说明命令接收超时了
				return -2;
			}
			continue;
		}
		err_tick = 0;
	}
}



/*--------------------------------------------------------------------------------*/

static at_man_t do_at_man={ create_dev,delete_dev,aoto_baudrate,send_cmd,printf_rxbuf };

at_man_pt at_man = &do_at_man;






