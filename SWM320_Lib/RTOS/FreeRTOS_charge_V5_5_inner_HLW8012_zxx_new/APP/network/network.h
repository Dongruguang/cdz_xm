#ifndef __net_work_h__
#define __net_work_h__

#include "framework.h"


#define NET_LOG(X...)	sysprt->alog(X)
#define NET_LINE( ) {NET_LOG("%s,%d\r\n",__FUNCTION__,__LINE__);}

#define EnErrLog	1
#define UnErrLog	0

#define Ndev_RX_BUF_LEN 4096
#define Ndev_TX_BUF_LEN 4096

#define _e_dev_done_msta	0xD0			
#define _e_dev_err_msta		0xF0

#define __dev_msta_done(  ) 		{ dev->msta = _e_dev_done_msta; }
#define __dev_msta_err( val) 	{ dev_dev->msta = _e_dev_err_msta + val; }


typedef struct{
	uint8_t msta;				
	uint8_t ssta;
	
	uint8_t err_cntr;		//错误计数器
	uint8_t big_err;		//大错误
	
	char* rxbuf;
	uint32_t rx_len;
	
	char* txbuf;
	uint32_t tx_len;
	
	uint32_t otick;
	uint32_t ntick;
	
	
	void* cnt_dev;
}net_dev_t;

typedef int (*msg_cycle_t)( void* ndev,int argc,char** argv );
typedef int (*net_cycle_t)( void* ndev );


typedef struct{

	/*构建一个设备*/
	int (*create_dev)( net_dev_t* netdev,void* dev );

	void (*clr_sta)( net_dev_t* netdev );	
	
	/*清除接收数据*/
	void (*clr_rxbuf)(net_dev_t* netdev);
	
	/*清除发送数据*/
	void (*clr_txbuf)(net_dev_t* netdev);
	
	
	/*打印发送缓冲区内容*/
	void (*ptf_rxbuf)(net_dev_t* netdev,int hex );
	
	/*打印接收缓冲区内容*/
	void (*ptf_txbuf)(net_dev_t* netdev,int hex );
	
	/*发送字符串*/
	void (*send_str)( net_dev_t* netdev,char *string);
	
	/*处理命令超时*/
	int (*deal_cmd_timeout)( net_dev_t* netdev,uint32_t timeout,uint32_t max_err,int log,uint32_t re_msta,const char* call,uint32_t line);

	
	/*工作: 复位设备*/
	int (*work_rst_dev)( net_dev_t* netdev);
	
	/*工作: 初始化设备*/
	int (*work_init_dev)( net_dev_t* netdev );
	
	/*工作: 构建网络环境*/
	int (*work_connect_serv)( net_dev_t* netdev,char* usr, char* pwd,char* vision );
	
	int (*install_net_cycle)( net_cycle_t callback);
	int (*install_msg_cycle)( msg_cycle_t callback);
	
}netdev_opt_t;

#include "./air7xx/air7xx.h"


//网络工作流程
typedef struct{
	uint8_t msta;
	uint8_t ssta;
	uint8_t connect;		
	uint8_t rec;
}wk_net_t;

extern net_dev_t* wk_netdev;

extern void network_entry( void );

#endif


