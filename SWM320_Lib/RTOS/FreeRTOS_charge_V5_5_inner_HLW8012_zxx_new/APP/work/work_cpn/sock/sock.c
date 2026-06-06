

#include "work.h"

extern sock_hlw_opt_pt sock_hlw_opt;


extern void sock_ctrl_work( void );


sock_sta_t socks[MaxSock] ={
	{0,0},
	{0,0},
};

void sock_init(void)
{
	//设置
	sock_mq->init();
	sock_hlw_opt->init();
	led_board->set_fuse_led( __ON );
	
	led_board->mode_req( 0,_e_mode_standing,NULL,NULL );
	led_board->mode_req( 1,_e_mode_standing,NULL,NULL );
}







void sock_work_entry( void )
{
	//定时重启一遍设备(如果没有插座正在工作)
	rst_dev_over_week();
	
	//监听保险丝状态
	listen_fuse();

	//监听电流状态
//	sock_hlw_opt->work();
	
	//插座控制工作
	sock_ctrl_work();
	
	//等待网络订单指令
	wait_net_order();
	
}
