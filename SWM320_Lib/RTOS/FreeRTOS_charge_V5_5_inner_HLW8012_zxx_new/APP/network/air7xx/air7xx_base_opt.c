#include "network.h"


/*-------------------------------------------------------------------------------------*/
//构建一个设备
int air_create_dev( net_dev_t* netdev,void* cnt_dev )
{

	netdev->cnt_dev = cnt_dev;
	
	netdev->rxbuf = mem_alloc( Ndev_RX_BUF_LEN );
	if( netdev->rxbuf == NULL )
	{
		NET_LOG("err:%s,%d\r\n",__FUNCTION__,__LINE__ );
		return -1;
	}
	
	netdev->txbuf = mem_alloc( Ndev_TX_BUF_LEN );
	if( netdev->txbuf == NULL )
	{
		NET_LOG("err:%s,%d\r\n",__FUNCTION__,__LINE__ );
		return -2;
	}
	
	return 1;
}
/*-------------------------------------------------------------------------------------*/
void air_clr_sta( net_dev_t* netdev )
{
	netdev->msta=0;
	netdev->ssta=0;
	netdev->err_cntr = 0;
	netdev->big_err = 0;
	
}
/*-------------------------------------------------------------------------------------*/
/*清除接收数据*/
void air_clr_rxbuf(net_dev_t* netdev)
{
	memset( netdev->rxbuf,0,Ndev_RX_BUF_LEN );
	netdev->rx_len = 0;
}

/*清除发送数据*/
void air_clr_txbuf(net_dev_t* netdev)
{
	memset( netdev->txbuf,0,Ndev_TX_BUF_LEN );
	netdev->tx_len = 0;
}
/*-------------------------------------------------------------------------------------*/
/*打印缓冲区内容*/
#define MaxPtr 128
static char ptf_buf[ MaxPtr ];
static void air_ptf_buf(net_dev_t* netdev,int dir,char* buf,uint32_t buf_size,uint32_t len,int hex,const char* call)
{
	
	uint32_t pos = 0;
	uint32_t i=0;
	if( len >= buf_size )
	{
		NET_LOG("err: %s,len =%08d \r\n",call,netdev->rx_len );
		return;
	}
	if(dir == 0)
	{
		NET_LOG("--------------->%s\r\n",call);
	}
	else
	{
		NET_LOG("---------------<%s,err %d...\r\n",call,netdev->err_cntr);
	}
	if( hex )
	{
		
		for( int i=0;i<len;i++ )
		{
			NET_LOG("0x%02x ",buf[i]);
		}
	}
	else
	{
		i=0;
		pos = 0;
		while( pos < len )
		{
			ptf_buf[i] = buf[pos];
			i++;
			pos++;
			if( i == (MaxPtr-1) )
			{
				ptf_buf[i] = '\0';
				
				NET_LOG("%s",ptf_buf);
				i=0;
		
			}
		}
		ptf_buf[i] = '\0';
		NET_LOG("%s",ptf_buf);
		
	}
}

void air_ptf_rxbuf(net_dev_t* netdev,int hex)
{
	air_ptf_buf(netdev,1,netdev->rxbuf,Ndev_RX_BUF_LEN,netdev->rx_len,hex,__FUNCTION__);
}

void air_ptf_txbuf(net_dev_t* netdev,int hex)
{
	air_ptf_buf(netdev,0,netdev->txbuf,Ndev_TX_BUF_LEN,netdev->tx_len,hex,__FUNCTION__);
}

/*-------------------------------------------------------------------------------------*/
//发送字符串
void air_send_str( net_dev_t* netdev,char *string)
{
	uint32_t ptr=0;
	uart_pt cnt_dev = (uart_pt) netdev->cnt_dev;
	
	while ( string[ptr] != '\0' )
	{
		cnt_dev->send( (uint8_t*)(&string[ptr]),1 );
		ptr++;
		//限制发送长度
		if( ptr >= Ndev_RX_BUF_LEN ) break;
	}
}
/*-------------------------------------------------------------------------------------*/
/*处理命令超时*/
int air_deal_cmd_timeout( net_dev_t* netdev,uint32_t timeout,uint32_t max_err,int log,uint32_t re_msta,const char* call,uint32_t line )
{
	int ret = 0;
	netdev->ntick = get_sys_ticks();
	if( (netdev->ntick - netdev->otick) > timeout )
	{
		netdev->err_cntr++;
		
		netdev->otick = netdev->ntick;
		if( 0 != log )	air_ptf_txbuf( netdev,0 );
		air_clr_rxbuf( netdev );
		
		
		if( netdev->err_cntr >= max_err)
		{//说明服务异常,重大故障，需要重启网络模块
			netdev->msta = _e_dev_err_msta;
			netdev->ssta = 0;
			ret = -2;
			NET_LOG("BigErr:%s,%d \r\n",call,line );
		}
		else
		{//重新尝试指令
			netdev->msta = re_msta;
			netdev->ssta = 0;
			ret = -1;
		}
	}
	return ret;
}
