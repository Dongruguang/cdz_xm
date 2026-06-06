
#include "work.h"
#include "network.h"

/*-------------------------------------------------------------------------------------*/

static int wait_reday( net_dev_t* netdev );
/*-------------------------------------------------------------------------------------*/


static void rst_pin_ctrl( int val )
{
	switch( val )
	{
		case 0:
			gpio->write( IO_NetRst,0 );
			break;
		
		case 1:
			gpio->write( IO_NetRst,1 );

			break;
		
		default:
			break;
	}
}



/*-------------------------------------------------------------------------------------*/

enum{
	//------------------------------------------------------------------------
	_e_server_init = 0,	//服务初始化
	//------------------------------------------------------------------------
	_e_rst_pin_ctrl_0,			//复位引脚低电平
	_e_rst_pin_ctrl_1,			//复位引脚高电平
	_e_rst_pin_ctrl_2,			//复位引脚低电平
	//------------------------------------------------------------------------
	_e_AT_RST,							//使用指令复位设备
	_e_wait_some_tick,			//等待一些时间
	_e_wait_reday,					//等待就绪
	//------------------------------------------------------------------------
	_e_wait_boot_time,			//等待启动时间
	
	
	
};

//复位设备
int air_work_rst_dev( net_dev_t* netdev )
{

	int ret = 0;
	int val = 0;
	
	
	switch( netdev->msta )
	{
		//------------------------------------------------------------------------

		case _e_server_init:
			NET_LINE( );
			netdev->msta++;
		
			break;
		//------------------------------------------------------------------------
		
		case _e_rst_pin_ctrl_0:
			NET_LINE( );
			led_board->set_net_led ( __OFF );
		
			air_opt->clr_rxbuf( netdev );		//清除缓冲区
			rst_pin_ctrl( 0 );
		
			netdev->otick = get_sys_ticks();
			netdev->msta++;
		
			break;
		
		case _e_rst_pin_ctrl_1:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 100 ) break;
			
			NET_LINE( );
			
			rst_pin_ctrl( 1 );
			
			netdev->otick = get_sys_ticks();
			netdev->msta++;
			break;
		
		case _e_rst_pin_ctrl_2:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 100 ) break;
			
			NET_LINE( );
			
			rst_pin_ctrl( 0 );
			
			netdev->otick = netdev->ntick;
			netdev->msta++;
			break;
		
		//------------------------------------------------------------------------
		case _e_AT_RST:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 100 ) break;
		
			NET_LINE( );
			
//			air_opt->send_str( netdev,"\r\nAT\r\n" );
			air_opt->send_str( netdev,"AT+RESET\r\n" );
			
			netdev->otick = netdev->ntick;
			netdev->msta++;
			break;
		
		case _e_wait_some_tick:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 100 ) break;
			
			netdev->otick = netdev->ntick;
			netdev->msta++;
			
			break;
		
		case _e_wait_reday:
			val = wait_reday( netdev );
			
			if( val > 0 )
			{//说明收到想要的数据了
				netdev->msta ++;
				netdev->ssta = 0;									//借助此变量来计数，此处清零
				NET_LINE( );
				air_opt->ptf_rxbuf( netdev,0 );
				air_opt->clr_rxbuf( netdev );
				break;
			}
			
			//等待超时
			air_opt->deal_cmd_timeout( netdev,2000,4,bFALSE,_e_AT_RST,__FUNCTION__,__LINE__ );
			
			break;
		
		case _e_wait_boot_time:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 500 ) break;
			netdev->otick = netdev->ntick;
			
			NET_LOG(".");
			netdev->ssta++;
			if( netdev->ssta >= 10 )
			{//
				netdev->msta = _e_dev_done_msta;
				netdev->ssta = 0;
			}
		
			break;
		
		//------------------------------------------------------------------------
			
		//操作完成
		case _e_dev_done_msta:
			
			ret = 1;
			break;
		
		//处理错误
		case _e_dev_err_msta:
			air_opt->clr_sta( netdev );
			
			break;
			
		default:
			break;
		
		//------------------------------------------------------------------------
		
	}
	return ret;
}

/*-------------------------------------------------------------------------------------*/
//等待reday
static uint32_t tlen = 0;
static int wait_reday( net_dev_t* netdev )
{
	int ret = 0;
	uart_pt cnt_dev = (uart_pt) netdev->cnt_dev;
	uint8_t* rxbuf = netdev->rxbuf;
	//读数据到dev->rxbuf

	tlen = cnt_dev->read_recv_buf( &(rxbuf[ netdev->rx_len ]),10 );
	netdev->rx_len += tlen;
	
//	if( tlen != 0  ) return 0;
	
	//本轮没有收到任何数据,说明数据可能收够了
	
	ret = strlib->findstr( (char*)rxbuf,"RDY");
	if( ret >= 0 ) return 1;
	
	ret = strlib->findstr( (char*)rxbuf,"OK");
	if( ret >= 0 ) return 1;

	ret = strlib->findstr( (char*)rxbuf,"NITZ");
	if( ret >= 0 ) return 1;
	
	return 0;
	
}

