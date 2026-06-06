#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)		

static uint32_t dto = 4000;
//static uint32_t cto = 4000;
extern char *http_serv;


char* str_http_tail = "application/octet-stream\r\n\r\n";


char http_file_path[128];

void dump_http_file_path( int MainVersion,int SubVersion )
{
	
	int ret = 0;
	char tbuf[128];
	memset( tbuf,0,128 );
	
	memset( http_file_path,0,128 );
	
	ret = 0;
	ret += strlib->dump( &http_file_path[ret],"/zxx/ota/app1_charge_inner_hlw8012_v" );
	
	sprintf( tbuf,"%d_%d.bin",MainVersion,SubVersion );

	ret += strlib->dump( &http_file_path[ret],tbuf );
	
}


//----------------------------------------------------------------------------------------
int http_download_get_size( void* airdev,uint32_t *size )
{
	air_dev_pt dev = airdev;
//	int offset[4];
	int ret = 0;
	uint32_t old_tick = 0,now_tick = 0;				//一对tick 		
	uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len
	uint8_t* rxbuf = dev->rxbuf;							//接收缓冲区指针，指针会被修改
	char tbuf[128];
	//先把接收缓冲区清空
	memset( rxbuf,0,1024 );
	air_man->clr_rxbuf( dev );								//清除air设备内部读写指针
	
	//发送http请求，获取文件尺寸
	PRT_LOG("http_req...\r\n");	
		
	//通过请求获取1个字节，得知整个文件的尺寸
	memset( tbuf,0,128 );
	ret = 0;
	ret += strlib->dump( &tbuf[ret],"GET " );
	ret += strlib->dump( &tbuf[ret],http_file_path );
	ret += strlib->dump( &tbuf[ret]," HTTP/1.1\r\n" );


	ret += strlib->dump( &tbuf[ret],"HOST:" );
	ret += strlib->dump( &tbuf[ret],http_serv );
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	ret += strlib->dump( &tbuf[ret],"Range:bytes=0-0 \r\n" );
//	ret += strlib->dump( &tbuf[ret],"Connection: close \r\n" );
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	
	PRT_LOG("txsend:%s\r\n",tbuf);	
	air_man->send_str( dev,tbuf );
	
	
	thread_delay_ms(200);
	//开始读数据
	dev->rxlen = 0;
//	err_tick =0;
	old_tick = get_sys_ticks();
//	err_cntr = 0;
	while( 1 )
		{
		//读数据到dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );

		dev->rxlen += rd_len;
		
		//本轮没有收到任何数据
		if( rd_len == 0  ) 
		{
			
			ret = strlib->findstr( (char*)rxbuf,"application/octet-stream"  );
			
			if( ret >= 0 )
			{//说明收到数据了
					//截取数据长度
				ret = strlib->intercept( (char*)rxbuf,(char*)tbuf ,"bytes 0-0/","Content-Type:" );
				if( ret < 0 )
				{//说明收到的内容是有问题的
					PRT_LOG("error:unknow size\r\n");
					return -2;
				}
				else
				{
					PRT_LOG("size = %s \r\n",tbuf);
					size[0] = atol( tbuf );
					return 1;
				}
				
			}
			
			//数据超时
			now_tick = get_sys_ticks();
			if( (now_tick - old_tick) >  dto )
			{
				return -2;
			}
			
		}
//		err_tick = 0;
	}
	
	
}


//----------------------------------------------------------------------------------------
static char str_range[20];
static char str_start[16];
static char end_start[16];
int http_download_part( void* airdev,uint8_t *dbuf,uint32_t vstart,uint32_t vend,uint32_t speed )
{
	air_dev_pt dev = airdev;

	int ret = 0,i = 0;
//	uint32_t err_tick = 0;										//描述错误滴答
//	uint32_t old_tick = 0,now_tick = 0;				//一对tick 		
	uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len
	uint8_t* rxbuf = dev->rxbuf;							//接收缓冲区指针，指针会被修改
//	uint32_t err_cntr = 0;										//计数器
	char* tbuf = (char*)dbuf;;

	

	
	//先把接收缓冲区清空
	memset( rxbuf,0,1024 );
	memset( tbuf,0,( vend - vstart + 1 ) );
	memset( str_start,0,sizeof( str_start ) );
	memset( end_start,0,sizeof( end_start ) );
	
	memset( str_range,'\0',sizeof( str_range ) );

	thread_delay_ms(50);
	air_man->clr_rxbuf( dev );								//清除air设备内部读写指针
	
	//发送http请求，获取文件尺寸
	PRT_LOG("http_download_part:%08d-%08d\r\n",vstart,vend);	
	
	
	sprintf( str_range,"%d-%d",vstart,vend );
	

	sprintf( str_start,"%08d",vstart );
	sprintf( end_start,"%08d",vend );
	
	
	ret = 0;
//	ret += strlib->dump( &tbuf[ret],"GET /ota/app1_charge_v3.bin HTTP/1.1\r\n" );
	ret += strlib->dump( &tbuf[ret],"GET " );
	ret += strlib->dump( &tbuf[ret],http_file_path );
	ret += strlib->dump( &tbuf[ret]," HTTP/1.1\r\n" );
	
	
	ret += strlib->dump( &tbuf[ret],"HOST:" );
	ret += strlib->dump( &tbuf[ret],http_serv );
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	ret += strlib->dump( &tbuf[ret],"Range:bytes=" );
	
	ret += strlib->dump( &tbuf[ret],str_start );
	ret += strlib->dump( &tbuf[ret],"-" );
	ret += strlib->dump( &tbuf[ret],end_start );
	
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	
	air_man->send_str( dev,tbuf );
	
	uint32_t t_delay = 300+speed*50;
	if( t_delay >2000 ) t_delay =2000; 
	thread_delay_ms( t_delay );
	//开始读数据
	dev->rxlen = 0;
//	err_tick =0;
//	old_tick = get_sys_ticks();
//	err_cntr = 0;
	while( 1 )
	{
		//读数据到dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );

		dev->rxlen += rd_len;
		
		//本轮没有收到任何数据
		if( rd_len == 0  ) 
		{
			//查找数据范围 str_range
			ret = strlib->findstr( (char*)rxbuf,str_range  );
			if( ret < 0 )
			{
				return -2;
			}
			
			//查找协议协议的末端
			ret = strlib->findstr( (char*)rxbuf,str_http_tail  );
			
			if( ret >= 0 )
			{//说明收到数据了
				//通过协议末端获取文件片段内容
				int dpos = ret + strlen( str_http_tail );
				int rlen = dev->rxlen - dpos;
				PRT_LOG("len = %d\r\n", rlen );
				
				if( rlen != 130 )
				{
					return -2;
				}
				
				//复制数据到目标缓冲区
				for( i=0;i<(dev->rxlen - dpos);i++ )
				{
					dbuf[i] = rxbuf[ i + dpos];
				}
				return 1;

				
					
			}
			else
			{
//				PRT_LOG("o");
				return -2;
			}
//			continue;
		}
//		err_tick = 0;
	}
}

//----------------------------------------------------------------------------------------
int http_close( void* airdev )
{
	air_dev_pt dev = airdev;
	int ret = 0;
	char tbuf[256];
	//先把接收缓冲区清空
	air_man->clr_rxbuf( dev );								//清除air设备内部读写指针
	

	
	memset( tbuf,0,256 );
	ret = 0;
//	ret += strlib->dump( &tbuf[ret],"GET /ota/app1_charge_v3.bin HTTP/1.1\r\n" );
	ret += strlib->dump( &tbuf[ret],"GET " );
	ret += strlib->dump( &tbuf[ret],http_file_path );
	ret += strlib->dump( &tbuf[ret]," HTTP/1.1\r\n" );
	
	ret += strlib->dump( &tbuf[ret],"HOST:" );
	ret += strlib->dump( &tbuf[ret],http_serv );
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	ret += strlib->dump( &tbuf[ret],"Range:bytes=0-0 \r\n" );
	ret += strlib->dump( &tbuf[ret],"Connection: close \r\n" );
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	ret += strlib->dump( &tbuf[ret],"\r\n" );
	
	air_man->send_str( dev,tbuf );

	task_delay(300);
		
	return 1;
	
}




