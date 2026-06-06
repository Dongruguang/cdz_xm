#include "network.h"


static int AT_CGMI_cat_work( net_dev_t* esp_dev );



enum{

	//------------------------------------------------------------------------
	//读取模块厂商信息
	_e_AT_CGMI_cat_start = 0,
	_e_AT_CGMI_cat_wait,
	_e_AT_CGMI_cat_work,
	
	//------------------------------------------------------------------------
	//读取固件版本信息
	_e_AT_CGMR_cat_start,		
	_e_AT_CGMR_cat_wait,
	_e_AT_CGMR_cat_work,
	
	
	//------------------------------------------------------------------------
	//查询卡是否插好
	_e_AT_CPIN_cat_start,		
	_e_AT_CPIN_cat_wait,
	_e_AT_CPIN_cat_work,
	
	//------------------------------------------------------------------------
	//查询设置信号质量
	_e_AT_CSQ_cat_start,		
	_e_AT_CSQ_cat_wait,
	_e_AT_CSQ_cat_work,

	//------------------------------------------------------------------------
	//查询网络注册状态
	_e_AT_CREG_cat_start,		
	_e_AT_CREG_cat_wait,
	_e_AT_CREG_cat_work,

	//------------------------------------------------------------------------
	//查询附着GPRS网络
	_e_AT_CGATT_cat_start,		
	_e_AT_CGATT_cat_wait,
	_e_AT_CGATT_cat_work,


	//------------------------------------------------------------------------
	//设置透传模式
	_e_AT_CIPMODE_set_start,		
	_e_AT_CIPMODE_set_wait,
	_e_AT_CIPMODE_set_work,
	//------------------------------------------------------------------------
	//设置IP为单链接
	_e_AT_CIPMUX_set_start,		
	_e_AT_CIPMUX_set_wait,
	_e_AT_CIPMUX_set_work,
	//------------------------------------------------------------------------
	//设置自动获取APN
	_e_AT_CSTT_set_start,		
	_e_AT_CSTT_set_wait,
	_e_AT_CSTT_set_work,
	
	//------------------------------------------------------------------------
	//激活移动场景，激活后能获取到IP
	_e_AT_CIICR_set_start,		
	_e_AT_CIICR_set_wait,
	_e_AT_CIICR_set_work,
	
	//------------------------------------------------------------------------
	//查询IP，只有获取到IP后才能上网
	_e_AT_CIFSR_cat_start,		
	_e_AT_CIFSR_cat_wait,
	_e_AT_CIFSR_cat_work,

	//------------------------------------------------------------------------
	_e_work_init_dev_end,
	
};

static uint32_t tlen = 0;

//初始化设备
int air_work_init_dev( net_dev_t* netdev )
{
	int fun_ret = 0;
	int ret = 0;
	int val = 0;
	int len = 0;
	uint8_t* rxbuf = (uint8_t*)(netdev->rxbuf);
	
	int offset[4];
	
	uart_pt cnt_dev = (uart_pt) netdev->cnt_dev;
	
	switch( netdev->msta )
	{
		//------------------------------------------------------------------------
		//读取模块厂商信息
		case _e_AT_CGMI_cat_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CGMI\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			
			break;
		
		case _e_AT_CGMI_cat_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			NET_LINE( );
			break;
		
		case _e_AT_CGMI_cat_work:
					
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );
			
			break;
		
		//------------------------------------------------------------------------
		//读取固件版本信息
		case _e_AT_CGMR_cat_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CGMR\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;
			break;
		case _e_AT_CGMR_cat_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			break;
		case _e_AT_CGMR_cat_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );
			
			
			break;
			
		//------------------------------------------------------------------------
			//查询卡是否插好
		case _e_AT_CPIN_cat_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CPIN?\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		
		case _e_AT_CPIN_cat_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;
			break;
		
		case _e_AT_CPIN_cat_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );
			break;
		//------------------------------------------------------------------------
		//查询设置信号质量
		case 	_e_AT_CSQ_cat_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CSQ\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case 	_e_AT_CSQ_cat_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			break;
		case 	_e_AT_CSQ_cat_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );
			break;
		//------------------------------------------------------------------------
		//查询网络注册状态
		case _e_AT_CREG_cat_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CREG?\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case _e_AT_CREG_cat_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			break;
		case _e_AT_CREG_cat_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );

			break;
		//------------------------------------------------------------------------
		//查询附着GPRS网络
		case _e_AT_CGATT_cat_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CGATT?\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case _e_AT_CGATT_cat_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			break;
		case _e_AT_CGATT_cat_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );
			
			break;
		//------------------------------------------------------------------------
		//设置透传模式
		case _e_AT_CIPMODE_set_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CIPMODE=1\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case _e_AT_CIPMODE_set_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			break;
		case _e_AT_CIPMODE_set_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,4000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );

			break;
		//------------------------------------------------------------------------
			//设置IP为单链接
		case _e_AT_CIPMUX_set_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CIPMUX=0\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case _e_AT_CIPMUX_set_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			break;
		case _e_AT_CIPMUX_set_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );
			
			break;
		//------------------------------------------------------------------------
		//设置自动获取APN
		case _e_AT_CSTT_set_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CSTT=\"\",\"\",\"\"\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case _e_AT_CSTT_set_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			break;
		case _e_AT_CSTT_set_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );

			break;		
		//------------------------------------------------------------------------
		//激活移动场景，激活后能获取到IP
		case _e_AT_CIICR_set_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CIICR\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;		
			break;
		case _e_AT_CIICR_set_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 50  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;	
			break;
		case _e_AT_CIICR_set_work:
			tlen = cnt_dev->read_recv_buf( &( rxbuf[ netdev->rx_len ]),10 );
			netdev->rx_len += tlen;
			
			if( netdev->rx_len >= Ndev_RX_BUF_LEN )
			{//检查接收缓冲区
				netdev->rx_len = 0;
				netdev->msta = netdev->msta - 2;	//回退2步
				NET_LINE( );
				break;
			}
			
			//检查是否收到 OK
			ret = strlib->findstr( (char*)rxbuf,"OK");			
			if( ret >= 0 ) 
			{
				netdev->msta ++;
				netdev->ssta = 0;
				air_opt->ptf_rxbuf( netdev,0 );
				netdev->err_cntr = 0;
				NET_LINE( );
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,2000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );

			break;
		
		//------------------------------------------------------------------------
		//查询IP，只有获取到IP后才能上网
		case _e_AT_CIFSR_cat_start:
			NET_LINE( );
			air_opt->clr_rxbuf( netdev );
			air_opt->send_str(netdev,"AT+CIFSR\r\n");
			netdev->otick = get_sys_ticks();
			tlen = 0;
			netdev->msta++;
			break;
		
		case _e_AT_CIFSR_cat_wait:
			netdev->ntick = get_sys_ticks();
			if( (netdev->ntick - netdev->otick) < 1000  ) break;
			netdev->otick = netdev->ntick;
			netdev->msta++;
			break;
		case _e_AT_CIFSR_cat_work:
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
			ret = strlib->findstr( (char*)rxbuf,"\r\n");			
			if( ret >= 0 )
			{
				if( 3 <= strlib->get_substr_index( (char*)rxbuf,".",offset,4 ) )
				{//说明收到IP
					netdev->msta ++;
					netdev->ssta = 0;
					air_opt->ptf_rxbuf( netdev,0 );
					NET_LINE( );
				}
				
				break;
			}
			
			//处理超时,超时则回退2步
			air_opt->deal_cmd_timeout( netdev,4000,2,EnErrLog,netdev->msta - 2,__FUNCTION__,__LINE__ );

			break;
		//------------------------------------------------------------------------
		case _e_work_init_dev_end:
			NET_LINE();
			netdev->msta = _e_dev_done_msta;
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

/*-------------------------------------------------------------------------------------*/






/*-------------------------------------------------------------------------------------*/

