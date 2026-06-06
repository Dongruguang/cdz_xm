
#include "network.h"


/*-------------------------------------------------------------------------------------*/
//air7xx_base_opt.c

extern int air_create_dev( net_dev_t* netdev,void* cnt_dev );

extern void air_clr_sta( net_dev_t* netdev );

extern void air_clr_rxbuf(net_dev_t* netdev);

extern void air_clr_txbuf(net_dev_t* netdev);

extern void air_ptf_rxbuf(net_dev_t* netdev,int hex);

extern void air_ptf_txbuf(net_dev_t* netdev,int hex);

extern void air_send_str( net_dev_t* netdev,char *string);

extern int air_deal_cmd_timeout( net_dev_t* netdev,uint32_t timeout,uint32_t max_err,int log,uint32_t re_msta,const char* call,uint32_t line );

/*-------------------------------------------------------------------------------------*/
//air7xx_work_rst_dev.c
extern int air_work_rst_dev( net_dev_t* netdev );

/*-------------------------------------------------------------------------------------*/
//air7xx_work_init_dev.c
extern int air_work_init_dev( net_dev_t* netdev );
/*-------------------------------------------------------------------------------------*/
//air7xx_work_connect_serv.c
extern int air_work_connect_serv( net_dev_t* netdev,char* usr, char* pwd,char* vision );

extern int air_install_net_cycle( net_cycle_t callback);
extern int air_install_msg_cycle( msg_cycle_t callback);
/*-------------------------------------------------------------------------------------*/


static netdev_opt_t do_air_opt={
	.create_dev = air_create_dev,
	.clr_sta = air_clr_sta,
	.clr_rxbuf = air_clr_rxbuf,
	.clr_txbuf = air_clr_txbuf,
	.ptf_rxbuf = air_ptf_rxbuf,
	.ptf_txbuf = air_ptf_txbuf,
	.send_str = air_send_str,
	.deal_cmd_timeout = air_deal_cmd_timeout,
	
	.work_rst_dev = air_work_rst_dev,
	.work_init_dev = air_work_init_dev,
	.work_connect_serv = air_work_connect_serv,
	.install_net_cycle = air_install_net_cycle,
	.install_msg_cycle = air_install_msg_cycle,
	
};

netdev_opt_t* air_opt = &do_air_opt;

/*-------------------------------------------------------------------------------------*/




