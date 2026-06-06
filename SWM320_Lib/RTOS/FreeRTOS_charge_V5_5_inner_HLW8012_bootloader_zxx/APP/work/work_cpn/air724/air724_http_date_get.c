#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)		


static uint8_t http_date[20];

char* get_date_str(void)
{
	return (char*)http_date;
}


static uint32_t dto = 4000;
static uint32_t cto = 4000;
extern char *http_serv;
int air724_http_date_get( void* airdev )
{
	air_dev_pt dev = airdev;

//	int enter_cntr = 0;												//计算收到了多少个回车符号
	int offset[4];
	
	int ret = 0,i = 0;
	uint32_t err_tick = 0;										//描述错误滴答
	uint32_t old_tick = 0,now_tick = 0;				//tick 对										
	
	uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len
	uint8_t* rxbuf = dev->rxbuf;							//接收缓冲区指针，指针会被修改
	uint32_t err_cntr = 0;										//计数器
//	char* sub_str = NULL;
	
	//先把接收缓冲区清空
	memset( rxbuf,0,1024 );
	air_man->clr_rxbuf( dev );
	//发送http请求
	PRT_LOG("http_req...\r\n");	
	
	air_man->send_str( dev,"GET /skin2/save/fudai/riqi.php HTTP/1.1\n" );
	

	air_man->send_str( dev,"HOST: \n" );
	
	
	air_man->send_str( dev,"Connection:Close\n" );
	air_man->send_str( dev,"\n" );
	air_man->send_str( dev,"\n" );
	
		
	thread_delay_ms(300);
	
	//开始读数据
	dev->rxlen = 0;
	err_tick =0;
	old_tick = get_sys_ticks();
	err_cntr = 0;
	
	while(1)
	{
		//读数据到dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );

		dev->rxlen += rd_len;
		
		//本轮没有收到任何数据
		if( rd_len == 0  ) 
		{
			ret = strlib->get_substr_index( (char*)rxbuf,"\r\n\r\n", offset,4 );
			
			if( ret >= 2 )
			{//说明收到了双回车符号
				if( strlib->findstr( (char*)rxbuf,"HTTP/1.1 200 OK" ) < 0 )
				{//说明收到的内容是有问题的
					PRT_LOG("recv data is error \r\n");
					return -2;
				}
				PRT_LOG("enter x2 \r\n");
				PRT_LOG("offset[0] = %d \r\n",offset[0] );
				PRT_LOG("offset[1] = %d \r\n",offset[1] );
								
				//将日期字符串复制出来
				for( i=0; i<8; i++ )
				{
					http_date[ i ] = rxbuf[ offset[0] + 4 + 3 + i ];
				}
				http_date[i] = '\0';
				PRT_LOG("http_date:%s\r\n", http_date );
				
				return 1;
			}
			else
			{//说明还没收到想要的数据
				if( strlib->findstr( (char*)rxbuf,"CONNECT" ) < 0 )
				{//说明连接中，还没连上，要等一下
					err_cntr++;
					thread_delay_ms(10);
					if( err_cntr > 3 )
					{//说明命令接收超时了						
						return -1;
					}		
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



//static void get_date_to_rtc( void* rtc,uint8_t* dat  )
//{
//	
//	
//}

