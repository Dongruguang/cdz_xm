#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

//创建TCP/UDP连接
//返回 1  说明 命令返回的内容正确
//返回 0  说明命令返回的内容说明AT设备错误
//返回 -1 说明很久没有收到数据了
//返回 -2 说明命令超时了

//static char* str_enter = "\r\n";


static uint32_t dto = 4000;
static uint32_t cto = 4000;
//	argv[0] = 	TCP/UDP
//	argv[1] = 	IP addr
//	argv[2] = 	port

int air724_cmd_AT_CIPSTART( void* airdev,int argc,char** argv )
{
	air_dev_pt dev = airdev;

	int ret = 0;
	uint32_t err_tick = 0;										//描述错误滴答
	uint32_t old_tick = 0,now_tick = 0;				//tick 对										
	
	uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len
	uint8_t* rxbuf = dev->rxbuf;							//接收缓冲区指针，指针会被修改
	
	//先把接收缓冲区清空
	memset( rxbuf,0,1024 );
	air_man->clr_rxbuf( dev );
	//发送命令
	PRT_LOG("argv[0]:%s \r\n",argv[0]);
	PRT_LOG("argv[1]:%s \r\n",argv[1]);
	PRT_LOG("argv[2]:%s \r\n",argv[2]);
	
	//AT+CIPSTART="TCP","14.116.153.234",80\r
	
	//AT+CIPSTART=
	air_man->send_str( dev,"AT+CIPSTART=");
	//"TCP" of "UDP"
	air_man->send_str( dev,"\"");
	air_man->send_str( dev,argv[0] );	//TCP or UDP
	air_man->send_str( dev,"\"");
	//,
	air_man->send_str( dev,",");
	//"000.000.000.000"
	air_man->send_str( dev,"\"");
	air_man->send_str( dev,argv[1] );
	air_man->send_str( dev,"\"");
	//,
	air_man->send_str( dev,",");
	//port
	air_man->send_str( dev,argv[2] );
	
	//enter
	air_man->send_str( dev,"\r\n" );
	
		
	thread_delay_ms(150);
	
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
			if( 0 < strlib->findstr( (char*)rxbuf,"CONNECT" ) )
			{//说明连上网了
				return 1;
			}
			
			ret = strlib->findstr( (char*)rxbuf,"\r\n");			
			if( ret > 0 )
			{//说明收到了回车符号
				
				if( 0 < strlib->findstr( (char*)rxbuf,"ERROR" ) )
				{//说明收到ERROR
					return 0;
				}	
			}
			
			if( ( dto == 0 )||( dto == 0XFFFFFFFF ) )
			{//说明这条消息接收比较慢
				thread_delay_ms(10);
			}
			else
			{//说明这条消息接收比较快
				err_tick++;
				thread_delay_ms(1);
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


