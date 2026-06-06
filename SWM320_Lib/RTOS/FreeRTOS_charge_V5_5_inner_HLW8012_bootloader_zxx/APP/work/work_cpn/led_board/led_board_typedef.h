

/*----------------------------------------------------------------------------------*/

//常规缓冲区，无礼品分值的缓冲区
/*
bit[9 : 0]	灯板1
bit[23:14]	灯板2
*/
typedef struct{
	uint8_t led[3];					//灯板，显示LED
}ledb_buf_t,*ledb_buf_pt;



enum{
	_e_mode_standing=0,			//待机状态
	_e_mode_charging,				//充电中
	_e_mode_charge_full,		//充满状态
	
};//

/*----------------------------------------------------------------------------------*/
//操作缓冲区的函数

typedef struct{
	void (*init)( void );
	
	//设置网络状态
	void (*set_net_led)( uint8_t net );
	
	//设置保险丝状态
	void (*set_fuse_led)( uint8_t fuse );
	
	
	//请求切换显示模式，road,mode,dat0,dat1
	void (*mode_req)( uint8_t road,uint8_t mode,uint32_t dat0,uint32_t dat1 );
	
	void (*work)( void );
	
	
}led_board_t,*led_board_pt;



