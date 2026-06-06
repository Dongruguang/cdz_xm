

#include "work.h"
#define PRT_LOG(X...)	sysprt->dlog(X)	

/*


	//½âÎö
	hlw_val[ road ].state_reg = hu->buf.state_reg[0];	
	hlw_val[ road ].check_reg = hu->buf.check_reg[0];
	hlw_val[ road ].vol_para_reg = ( hu->buf.vol_para_reg[0]<<16 )	+ ( hu->buf.vol_para_reg[1]<<8 ) 	+ ( hu->buf.vol_para_reg[2] );
	hlw_val[ road ].vol_reg = ( hu->buf.vol_reg[0]<<16 ) + ( hu->buf.vol_reg[1]<<8 ) + ( hu->buf.vol_reg[2] );
	hlw_val[ road ].curr_para_reg = ( hu->buf.curr_para_reg[0]<<16 ) + ( hu->buf.curr_para_reg[1]<<8 ) + ( hu->buf.curr_para_reg[2] );
	hlw_val[ road ].curr_reg = ( hu->buf.curr_reg[0]<<16 ) + ( hu->buf.curr_reg[1]<<8 ) + ( hu->buf.curr_reg[2] );
	hlw_val[ road ].pwr_para_reg = ( hu->buf.pwr_para_reg[0]<<16 ) + ( hu->buf.pwr_para_reg[1]<<8 ) + ( hu->buf.pwr_para_reg[2] );
	hlw_val[ road ].pwr_reg = ( hu->buf.pwr_reg[0]<<16 ) + ( hu->buf.pwr_reg[1]<<8 ) + ( hu->buf.pwr_reg[2] );
	hlw_val[ road ].dat_updata_reg = ( hu->buf.dat_updata_reg[0] );
	hlw_val[ road ].pf_reg = ( hu->buf.pf_reg[0]<<8 ) + hu->buf.pf_reg[1] ;
	hlw_val[ road ].check_sum_reg =  hu->buf.check_sum_reg[0] ;
	
	dvol[road] 	=  hlw_val[road].vol_para_reg * vol_c[road] / hlw_val[road].vol_reg ;
	dcurr[road] =  hlw_val[road].curr_para_reg *curr_c[road] / hlw_val[road].curr_reg ;		
	dpwr[road] 	=  hlw_val[road].pwr_para_reg * vol_c[road] * curr_c[road] / hlw_val[road].pwr_reg ;

*/

