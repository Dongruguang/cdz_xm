#ifndef __work_h__
#define __work_h__

/*引脚的使用




*/


#define IV_Mode	1
// =0,旧版 不对称的旧电路板
// =1,新版 对称设计，但是插座和检测同时左右颠倒了



#define App_Version                      5               /**< App major version number */
#define App_SubVersion                   5               /**< App minor version number */




/*
{
发布app版本
	20230523: 3-2
}

*/



extern char version_info[20];

extern void req_goto_update( void );


#include "framework.h"
#include <flashdb.h>
/*-------------------------------------------------------*/
#define BOOT_CODE 0xA5				//启动码，必须跟bootloader一致
#define NetMode_Not			0
#define NetMode_Air724	1
#define NetMode_Wifi		2
#define NetMode_nbiot		3
#define NetMode_Air780	4


enum _program{
	_app_update = 0,
	_app_fac,
	_app_new
};

enum _ota_server {
	_ota_ydc_serv = 0,	//友德充服务器
	_ota_test_serv,			//测试服务器
};

//#define WorkIngCode		0xD0			
//typedef struct{
//	uint32_t work_code;			//工作码	 =WorkIngCode,说明上一次订单未结束
//	uint32_t tick;			//剩余时长
//	char ddh[20];				//订单号
//	
//}last_order_t,*last_order_pt;



#define MaxLen_User		32
typedef struct{

	//---- 4个字
	uint8_t boot_code;						//启动码
	uint8_t boot_app;							//选择启动什么程序
	uint8_t ota_req;							//是否存在OTA请求
	uint8_t net_mode;							//联网方式	
	
	
	uint8_t rp_ota_first_boot;		//描述是否上报OTA成功
	uint8_t net_ptf;							//描述是否允许打印网络数据内容
	uint8_t rec8[2];
	
	uint32_t rec32[2];						//保留2个字
	
	//----	4个字
	float k[2];										//校准后的拟合直线系数k
	float b[2];										//校准后的拟合直线截距b
	//----	4个字
	char user_name[ MaxLen_User ];				//用户名
	//----	4个字
	char user_pwd[ MaxLen_User ];				//用户密码
	//----	16个字(64字节) 保留	
	uint8_t rec8__[64];						//保留
}storage_t;


typedef struct{
	uint32_t rp_serv_first_boot;		//报告第一次启动
}work_t;

typedef struct{
	//---------------------------------------------
	storage_t* storage;		//存储内容
	//---------------------------------------------
	work_t work;//	工作数据
	/*----------------------------------------------*/

	
}wdat_t,*wdat_pt;
extern wdat_pt wdat;
extern void work_data_load( void );
extern void work_data_write( void );

/*-------------------------------------------------------*/
#define MaxSock 2


typedef struct{
//	wk_num_pt num;	//机器信息
	int msta;
	int ssta;
	
}wk_inet_t,*wk_inet_pt;


/*-------------------------------------------------------*/

/*-------------------------------------------------------*/
//新的work dat


//----
typedef struct{
	uint32_t code;					//启动码					=0xA5
	uint32_t cntr;					//启动计数器
}boot_sta_t;

#define MaxSolt	24		//最大支持24个时间段
#define MaxPeriodE 6	//最大支持6个区间电能

typedef struct{
	uint32_t group;			//描述属于哪一组的时间
	uint32_t start;			//单位 分钟
	uint32_t end;				//单位 分钟
}slot_t;

/*

*/
typedef struct{
	uint8_t get_date;			//描述是否已经获取日期时间
	uint8_t get_slot;			//描述是否已经获取分时时段
	uint8_t segments;			//一共有多少个段 
	uint8_t groups;				//一共有多少组
	slot_t val[MaxSolt];	//各段的数据
}time_slot_t;


//----
typedef struct{
	uint32_t pul[2];		//电能脉冲数
}total_e_t;
//----

typedef struct{
	uint32_t sock[2];		//电流传输序号
}rtd_num_t;
//----
#define MaxDDH_Len 32
typedef struct{
	
	uint8_t working;							//描述订单是否正在工作 =0,不工作 =1 正在工作
	uint8_t rec[3];								//对齐
	
	//订单号
	char ddh[MaxDDH_Len];	
	
	//开始时间
	rtc_dt_t start;								//订单开始时间
	rtc_dt_t end;									//计划结束时间
	
	uint32_t pluse[MaxSolt];			//区间电能
	
	
}order_dat_t;
//----
typedef struct{
	order_dat_t sock[2];		//电能脉冲数
}order_info_t;

//typedef uint32_t dval_t;
/*-------------------------------------------------------*/
//集中处理
typedef struct{
	
	//需要存储的数据
	boot_sta_t* boot_sta;
	total_e_t* total_e;						//插座总电能
	rtd_num_t* rtd_num;						//实时数据序列号	
	order_info_t* order_info;			//插座订单信息

	//不存储的数据，只引导
	time_slot_t* time_slot;				//24个时间段,如果没使用，则为0

	
}wdat_guide_t;

extern wdat_guide_t *guide_dat;

/*-------------------------------------------------------*/
typedef struct{
	int (*get_by_fdb)( void );
	int (*set_to_fdb)( void* dat );
	
}db_base_t;

typedef struct{
	db_base_t boot_sta;
	db_base_t total_e;
	db_base_t rtd_num;
	db_base_t order_info;
	
}wdat_opt_t,*wdat_opt_pt;

extern wdat_opt_pt wdat_opt;
extern void wdat_guide_init(void);
/*-------------------------------------------------------*/


// 计算时间段的重叠分钟数
extern int calculate_overlap( slot_t period0 ,slot_t period1 ) ;
//判断处于那个时间段
extern int get_time_segments( slot_t* list,int len,int hour,int min );
//给日期增加时间
extern void addTime( rtc_dt_t *dateTime, int years, int months, int days, int hours, int minutes, int seconds);
// 函数：将日期时间转换为总分钟数
extern uint32_t datetime_to_minutes( rtc_dt_t dt );
// 函数：计算过去了多少分钟
extern uint32_t calculate_minutes_difference( rtc_dt_t current, rtc_dt_t past) ;

//通过字符串填充rtc 字符串格式 年月日时分秒 20241025102210
extern void dump_rtc_by_str( char* time );

extern void work_internet(void);

extern void prt_date_time( rtc_dt_t* dt,char* str );

extern void dump_msh_info( char* num,char* date );

extern uint32_t get_msh_terminal_val( void );
extern char* get_msh_terminal_str( void );

extern char *serv_ip;


extern void save_order_them_shutdown( void );		//重启之前，调用此程序


#include "./work_cpn/work_cpn_typedef.h"

#include "./work_cpn/work_cpn_inc_conf.h"

#include "./network/network.h"


#endif

