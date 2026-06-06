
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X);//dprintf(X)	

#define CMD_MaxErr	3
#define __ERR_AIR_RST( ) 	{ line = __LINE__;goto __AIR_RST; }


void work_internet_create_and_rst( wk_inet_pt inet )
{
	static uint32_t sec = 0;
	static time_t otick=0,ntick=0;
	static uint32_t wait_tick = 0;

	int err = 0,ret = 0,line = 0;
	air_dev_pt  this_dev;
	this_dev = air724;

	
	switch( inet->ssta )
	{
		/*--------------------------------------------------------*/

		case 0:
			//构建air724结构
			
			
			create_dev_air724();
		
			dump_http_file_path( Brd_Version,Brd_SubVersion);
			inet->ssta ++;
			break;
		/*--------------------------------------------------------*/
		
		case 1:
			PRT_LOG("\r\nreset net server\r\n");
		
			led_board->set_net_led ( __OFF );
			//复位这个模块
			air724_rst();
			otick = get_sys_ticks();
			sec = 0;
			
			//根据模块属性选择复位时长
			switch( wdat->storage->net_mode )
			{
				case NetMode_Air724:
					wait_tick = 45;
					break;
				case NetMode_Air780:
					wait_tick = 45;
					break;
				default:
					wait_tick = 40;
					break;
			}
		
			inet->ssta ++;
			thread_delay_ms(2000);
			break;
		/*--------------------------------------------------------*/
			
		case 2:
			//查询卡是否插好
			err = 0;
			while( 1 )
			{
				ret = air724->cmd_AT_RESET( this_dev,NULL,NULL );
				air_man->ptf_rxbuf( this_dev,NULL );
				if( ret == 1 ) break;
				thread_delay_ms(500);
				if( (err++)>5 ){__ERR_AIR_RST();}
			}
			inet->ssta ++;
			
			break;
		/*--------------------------------------------------------*/
		case 3:
			//给一定的时间，让这个模块完成复位
			ntick = get_sys_ticks();
			if( 500 > ( ntick - otick ) ) break;
			otick = ntick;
			PRT_LOG(".");
			
			//满10s，则下一步
			if( sec++ >= wait_tick )
			{
				PRT_LOG("\r\n");
				inet->msta ++;
				inet->ssta = 0;
			}
			break;
		/*--------------------------------------------------------*/
		
		default:
			break;
		/*--------------------------------------------------------*/

	}
	return ;
	
//	air_man->send_str( this_dev,"+++" );
__AIR_RST:
	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
	//请求复位
	inet->msta = 0;
	inet->ssta = 1;
	return;	
}









