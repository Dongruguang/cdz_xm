
typedef struct{
	char ddh[MaxDDH_Len];	//订单号
	uint16_t num;	//描述控制哪一个设备
	uint16_t cmd;	//描述控制命令
	uint32_t dat;	//描述参数，订单剩余可用充电时长,单位分钟
//	uint32_t utime;	//描述参数，订单已用时长
	uint32_t param;	//描述参数，订单已用时长
}order_t,*order_pt;


#define _event_curr 			0			//电流日志
#define _event_sock   		1			//插座工作日志
#define _event_net				2			//联网日志
#define _event_recore			3			//警告日志
#define _event_m_tenance 	4			//维护日志

//区间电能
typedef struct{
	uint32_t min;		//多少分钟
	float kwh;			//千瓦时
}period_e_t;

typedef struct{
	char ddh[MaxDDH_Len];		//订单号
	uint32_t type;	//描述事件类型	=0 电流；=1 插座事件
	
	uint16_t num;		//描述控制哪一个设备
	uint16_t val;		//描述事件值
	
	uint32_t dat;		//描述整数参数
	double curr;		//描述电流值参数
	
	period_e_t period_e[MaxPeriodE];	//描述订单结束时的区间电能
	
	
}event_t,*event_pt;


typedef struct{
	void (*init)( void );
	//订单消息
	void (*send_order)( char* ddh, int num, int cmd, uint32_t dat, uint32_t param  );
	int (*recv_order)( order_pt order );

	//事件消息
	void (*put_event)( char* ddh,int type,int num,int val, uint32_t dat,double curr,void* period_e   );
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
	oeb_manual_stop,						//手动结束订单，请等待电量结算
	
};

/*---------------------------------------------------------------*/
//hlw8032

//typedef struct{
//	uint32_t SOCK_CONNECT;	//插座是否接设备
//	double SOCK_P;					//有效功率
//	double SOCK_Vol;				//插座电压
//	
//}sock_hlw_dat_t,*sock_hlw_dat_pt;

#define MaxHLW	2


#define cmd_start0 	0x5A
#define cmd_len			24


//插座工作状态控制
typedef struct{
	int msta;
	int ssta;
	
	int resver;
	
	uint32_t otick_order;			//描述订单开始时间		单位 ms
//	uint64_t dtick_order;			//描述订单目标时间
	
	uint32_t utime_order;			//订单已过时长(该值在充电过程中，定时更新) 单位 min
	uint32_t atime_order;			//订单总的时长 单位 min
	
	float base_curr;						//用于定时上传时描述变化程度的基础电流
	
	float average_curr;				//N个周期内的平均电流
	float average_pwr;					//N个周期内的平均功率
	
	float min_w_s;							//最近1分钟所消耗的电能 多少 w * s
	
	
	uint16_t sock_out;			//插座被拔出
	uint16_t cntr_out;			//插座被拔出计数器
	uint32_t tick_out;			//插座被拔出事件
	
	int order_remind;					//描述是否有订单请求
	
	uint16_t order_stop_req;	//描述是否存在订单结束请求
	uint16_t big_cntr;				//大电流计数器
	
	order_t order;						//当前订单内容(不具备实时性)
	
}sock_sta_t,*sock_sta_pt;

/*----------------------------------------------------------------------------------*/

enum{
	_type_hlw_sock_out = 0,	//拔插座
	_type_hlw_cyc_pul,			//周期脉冲
	_type_hlw_min_pul,			//1分钟脉冲
	
};

typedef struct{
	uint32_t type;		//数据类型
	uint32_t cyc;			//所用周期
	uint32_t val;			//本周期脉冲数
}hlw_pul_t;

typedef struct{
	//初始化hlw
	void (*init)( void );

	//获取周期脉冲
	int (*get_cyc_pul)( int road,void* data );
	
	//轮询作业
	//
	void (*listen_work)( void );
	
}sock_hlw_opt_t,*sock_hlw_opt_pt;

/*----------------------------------------------------------------------------------*/
