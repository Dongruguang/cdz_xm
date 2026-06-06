#include "work.h"
#include "network.h"

/*-------------------------------------------------------------------------------------*/
//默认轮询工作函数(提示作用)
static int default_net_cycle( void* ndev )
{
	NET_LOG("no install usr func\r\n");
	NET_LINE();
	thread_delay_ms(1000);
	return 0;
}
static net_cycle_t net_cycle = default_net_cycle;

int air_install_net_cycle( net_cycle_t callback)
{
	net_cycle = callback;
	return 0;
}

/*-------------------------------------------------------------------------------------*/
//默认解析回调函数(提示作用)
static int default_msg_cycle( void* ndev,int argc,char** argv )
{
	NET_LOG("no install usr func\r\n");
	NET_LINE();
	thread_delay_ms(1000);
	return 0;
}
static msg_cycle_t msg_cycle = default_msg_cycle;

int air_install_msg_cycle( msg_cycle_t callback)
{
	msg_cycle = callback;
	return 0;
}
/*-------------------------------------------------------------------------------------*/



enum{
	//-----------------------------------------------------------
	//连接服务器
	_e_AT_CIPSTART_start = 0,
	_e_AT_CIPSTART_wait,
	_e_AT_CIPSTART_work,
	//-----------------------------------------------------------
	//登录服务器
	_e_Login_start,
	_e_Login_wait,
	_e_Login_wrok,
	//-----------------------------------------------------------
	//数据交互
	_e_msg_cycle,
	_e_clr_swap,
	
	
	//-----------------------------------------------------------
	//断开服务器
	_e_AT_CIPCLOSE_start,
	_e_AT_CIPCLOSE_wait,
	_e_AT_CIPCLOSE_work,
	
	
	_e_deal_timeout,
	
	
};

static uint32_t tlen = 0;

//初始化环境
//netdev 联网的设备
//argv[0]: 用户名
//argv[1]: 密码
//argv[2]: 版本号

#define MaxArgc_Normal 10


int air_work_connect_serv( net_dev_t* netdev,char* usr, char* pwd,char* vision )
{
	int fun_ret = 0;
	int ret = 0;
//	int val = 0;
//	int len = 0;
	int cmd_ret=0;
	uint8_t* rxbuf = (uint8_t*)(netdev->rxbuf);
	
//	int offset[4];
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	uart_pt cnt_dev = (uart_pt) netdev->cnt_dev;
	
	switch( netdev->msta )
	{
		//------------------------------------------------------------------------
		//连接服务器
		case _e_AT_CIPSTART_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CIPSTART=");
			air_opt->send_str(netdev,"\"TCP\"");
			air_opt->send_str(netdev,",");
			air_opt->send_str(netdev,"\"www.armsoc.cn\"");
			air_opt->send_str(netdev,",");
			air_opt->send_str(netdev,"\"9002\"");
			air_opt->send_str(netdev,"\r\n");
			
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		
		case _e_AT_CIPSTART_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 100  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			NET_LINE( );
			break;
		
		case _e_AT_CIPSTART_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 回车
			ret = strlib->findstr( (char*)rxbuf,"CONNECT");
			if( ret >= 0 )
			{
				//说明收到OK
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			if( 0 < strlib->findstr( (char*)rxbuf,"ERROR" ) )
			{//说明收到ERROR
				netdev->msta = _e_dev_err_msta;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}	
			
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,4000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );		
			break;
		//------------------------------------------------------------------------
			//登录服务器
		case _e_Login_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
#if 0
			air_opt->send_str(netdev,"LOGIN ");
		
			air_opt->send_str(netdev,usr);
			air_opt->send_str(netdev," ");
		
			air_opt->send_str(netdev,pwd);
			air_opt->send_str(netdev," ");

			air_opt->send_str(netdev,vision );
			air_opt->send_str(netdev,"\r\n");
#endif
			air_opt->clr_txbuf( netdev );
			snprintf( netdev->txbuf,Ndev_TX_BUF_LEN,"LOGIN %s %s %s\r\n",usr,pwd,vision );
			air_opt->send_str(netdev,netdev->txbuf);
			air_opt->ptf_txbuf( netdev,0 );
		
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case _e_Login_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 100  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			NET_LINE( );
			break;
		case _e_Login_wrok:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 想要的数据
			ret = strlib->findstr( (char*)rxbuf,"Logon\r\n" );
			if( ret > 0 )
			{//说明连接服务器成功
				NET_LOG( "LOGON->>>>work cycle\r\n" );
				led_board->set_net_led ( __ON );
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,4000,2,EnErrLog,_e_AT_CIPCLOSE_start,__FUNCTION__,__LINE__ );	
			break;
		//------------------------------------------------------------------------
		//数据交互
		case _e_msg_cycle:
			
			//此处负责循环作业，不管是否收到数据，都会执行
			net_cycle( netdev );
			
			//下方负责解析数据，收到数据才会执行
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//超时检测
			air_opt->deal_cmd_timeout( netdev,30000,2,EnErrLog,_e_AT_CIPCLOSE_start,__FUNCTION__,__LINE__ );	
			
			
			//检查是否收到 回车
			ret = strlib->findstr( (char*)rxbuf,"\r\n");
			if( ret < 0 ) break;
			
			air_opt->ptf_rxbuf( netdev,0 );
			
//			NET_LINE( );
			//拆分消息
			argc = strlib->split_string_n( MaxArgc_Normal,(char*)rxbuf, argv," \t\n\r");
			//参数不对,跳转到 _e_clr_swap 清空数据
			if (argc == 0) { netdev->msta = _e_clr_swap;break; }
			//解析消息(约定返回值,0不做任何处理,)
			cmd_ret = msg_cycle( netdev,argc,argv );
			netdev->msta = _e_clr_swap;
			
			break;
		//------------------------------------------------------------------------		
		case _e_clr_swap:
			
			
			air_opt->clr_rxbuf( netdev );
			netdev->otick = get_sys_ticks();
			netdev->msta --;
			tlen = 0;
			break;
		//------------------------------------------------------------------------
		//断开服务器
		case _e_AT_CIPCLOSE_start:
			//退出透传模式
			thread_delay_ms( 1500 );
			air_opt->send_str( netdev,"+++" );
			thread_delay_ms( 1000 );
			air_opt->send_str( netdev,"AT+CIPCLOSE\r\n" );

			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case _e_AT_CIPCLOSE_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 100  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			NET_LINE( );
			break;
		case _e_AT_CIPCLOSE_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 回车
			ret = strlib->findstr( (char*)rxbuf,"OK");
			if( ret >= 0 )
			{
				//说明收到OK
				netdev->msta = 0;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			if( 0 < strlib->findstr( (char*)rxbuf,"ERROR" ) )
			{//说明收到ERROR
				netdev->msta = 0;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}	
			
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,4000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );		

			break;
		
		case _e_deal_timeout:
			//退出透传
			NET_LINE();
			netdev->msta = _e_dev_err_msta;
			break;
		
		//------------------------------------------------------------------------
		//操作完成
		case _e_dev_done_msta:
			fun_ret = 1;
			break;
		
		//处理错误
		case _e_dev_err_msta:
			NET_LINE( );
			fun_ret = -1;
			break;
		
		default:
			break;
		//------------------------------------------------------------------------
			
	}
	return fun_ret;
}





