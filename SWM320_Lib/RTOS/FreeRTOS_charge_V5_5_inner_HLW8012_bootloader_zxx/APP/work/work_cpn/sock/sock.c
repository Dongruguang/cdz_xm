

#include "work.h"

extern sock_hlw_opt_pt sock_hlw_opt;


extern void sock_ctrl_work( void );


void sock_init(void)
{
	//ÉèÖÃ
	sock_mq->init();
	sock_hlw_opt->init();
	led_board->set_fuse_led( __ON );
	
	led_board->mode_req( 0,_e_mode_standing,NULL,NULL );
	led_board->mode_req( 1,_e_mode_standing,NULL,NULL );
}

void sock_work_entry( void )
{
	sock_ctrl_work();
}
