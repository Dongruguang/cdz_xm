
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

extern void work_internet_create_and_rst( wk_inet_pt inet );
extern void work_internet_init_hardware( wk_inet_pt inet );
extern void work_internet_work_env( wk_inet_pt inet );
extern void work_internet_http( wk_inet_pt inet );
extern void work_internet_aes_base64( wk_inet_pt inet );
extern void work_internet_connect_server( wk_inet_pt inet );
extern void work_internet_recv_analysis( wk_inet_pt inet );

wk_inet_t wk_inet;


static int net_reconnect_cntr = 0;
static void clr_net_reconnect_cntr(void)
{
	net_reconnect_cntr = 0;
}

static void add_net_reconnect_cntr(void)
{
	extern void req_rst_mcu( void );

	net_reconnect_cntr++;
	PRT_LOG( "net_reconnect_cntr ==> %d\r\n",net_reconnect_cntr );
	if( net_reconnect_cntr>5 ) 
		req_rst_mcu();
}




void work_internet(void)
{
	switch( wk_inet.msta )
	{
		case 0://创建并复位Air724
			
			work_internet_create_and_rst( &wk_inet );
			
			break;
		case 1://初始化 air724 硬件
			add_net_reconnect_cntr();
		
			work_internet_init_hardware( &wk_inet );
			
			break;
		case 2://初始化 air724联网工作环境
			work_internet_work_env( &wk_inet );
			break;
		case 3://连接http服务器，获取程序
			work_internet_http( &wk_inet );
			break;
		case 4:
			break;
		default:
			break;
	}
	
}





