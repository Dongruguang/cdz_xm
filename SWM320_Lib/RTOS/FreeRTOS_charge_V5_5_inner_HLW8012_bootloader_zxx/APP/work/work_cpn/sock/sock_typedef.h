
typedef struct{
	char ddh[20];	//订单号
	uint16_t num;	//描述控制哪一个设备
	uint16_t cmd;	//描述控制命令
	uint32_t dat;	//描述参数
	
}order_t,*order_pt;


#define _event_curr 		0
#define _event_sock   	1
#define _event_net			2
#define _event_recore		3

typedef struct{
	char ddh[20];		//订单号 16字节订单号，一字节'\0'
	uint32_t type;	//描述事件类型	=0 电流；=1 插座事件
	
	uint16_t num;		//描述控制哪一个设备
	uint16_t val;		//描述事件值
	
	uint32_t dat;		//描述整数参数
	double curr;		//描述电流值参数
}event_t,*event_pt;


typedef struct{
	void (*init)( void );
	void (*send_order)( char* ddh, int num, int cmd, uint32_t dat  );
	void (*put_event)( char* ddh,int type,int num, 
					int val, uint32_t dat,double curr  );
	int (*recv_order)( order_pt order );
	int (*recv_event)( event_pt event );

}sock_mq_t,*sock_mq_pt;

enum sock_order{//order cmd to 
	oct_start_charge = 0,	//充电
	oct_cancel_charge,		//取消充电
	oct_clr_err,					//清除错误
};

enum sock_event{//order event by
	oeb_time = 0, 	 						//订单正常结束
	oeb_curr_too_small,					//电流过小
	oeb_curr_too_big,						//电流过大
	oeb_no_ins,									//没有插入充电器
	oeb_fuse_broken,						//保险丝断掉了
	
};

/*---------------------------------------------------------------*/
//hlw8032





//typedef struct{
//	uint32_t SOCK_CONNECT;	//插座是否接设备
//	double SOCK_P;				//有效功率
//	double SOCK_Vol;			//插座电压
//	
//}sock_hlw_dat_t,*sock_hlw_dat_pt;



#define cmd_start0 	0x5A
#define cmd_len			24

typedef struct{
	//初始化hlw
	void (*init)( void );
	int (*cat_connect)( int road );
	double (*cat_epwr)( int road );
	double (*cat_ecurr)( int road );
	
	double (*cat_evol)( int road );
	
	//轮询作业
	void (*work)( void );

	
}sock_hlw_opt_t,*sock_hlw_opt_pt;



