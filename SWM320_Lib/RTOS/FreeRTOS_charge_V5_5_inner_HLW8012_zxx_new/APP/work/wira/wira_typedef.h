

typedef struct{
	void (*feed)( wk_inet_pt inet );
	int (*deal)( wk_inet_pt inet );
	
}nwdt_t,*nwdt_pt;

typedef struct{
	uint32_t code;
	uint32_t terminal;	//终端号
	uint32_t order;			//命令
	char ddh[20];				//存放订单号，订单号认为只能是16个字节
	uint32_t sock;			//描述插座号
	uint32_t time;			//描述订单剩余充电时长 单位 ms
	uint32_t utime;			//描述订单已用时长
}rDat_t,*rDat_pt;


typedef struct{
	uint32_t socket;	//插座号
	uint32_t time;		//充电时长
}ddInfo_t,*ddInfo_pt;

//-------------------------------------------------------------------------
//交互池
typedef struct pool{
	struct pool *last;		//上一项
	
	uint32_t sta;					//状态机值
	uint32_t num;					//项目号
	uint32_t dest_cntr;		//目标发送次数
	uint32_t now_cntr;		//当前发送次数
	uint32_t wait_tick;		//等待时间
	time_t otick;					//上一次发送的时间
	
	event_t event;				//需要发送的数据
	struct pool *next;		//下一项
}pool_t,*pool_pt;

typedef struct{
	void (*init)( void );
	
	void (*register_one)( pool_pt pool );
	int (*delate_one)( pool_pt pool );
	
	pool_pt (*create)( uint32_t dest,uint32_t wait,event_pt event );
	void (*destroy)( pool_pt pool );
	
	pool_pt (*get_by_ddh)( char* ddh );
	
	void (*work)( void );
	
}ipool_t,*ipool_pt;
//-------------------------------------------------------------------------
//下面的枚举一定要和字符串 数组对的上，否则就瓜柴

enum{ 
	_srv_ret_logon = 0,				//返回登录信息
	_srv_ret_relogin,					//返回relogin
	_srv_ret_nologin,					//返回nologin
	_srv_ret_over,						//返回over
	_srv_ret_log,							//返回log 实时数据上报
	_srv_ret_pong,						//返回pong
	_srv_ret_PARAM,						//返回上报终端参数
	_srv_ret_STATU,						//返回上报终端状态
	_srv_ret_jfpg,						//返回尖峰平谷
	_srv_ret_time,						//返回时间
	_srv_cmd_setparam,				//设置参数
	_srv_cmd_getstatu,				//获取状态
	_srv_cmd_update,					//更新OTA
	_srv_cmd_on,							//送电
	_srv_cmd_off,							//停电
	_srv_cmd_reboot,					//重启机器, 协议没有, 先保留这条命令
	_srv_cmd_max,
};

//返回值定义
#define ret_RstNet		-10			//复位网络
#define ret_ReConnect	-9			//重新连接

#define ret_Unknow	-1				//解析失败 未知
#define ret_Null		0					//普通
#define ret_Success	1					//成功

typedef int (*deal_srv_cmd_t)( uint8_t* dat );
//-------------------------------------------------------------------------




//-------------------------------------------------------------------------

