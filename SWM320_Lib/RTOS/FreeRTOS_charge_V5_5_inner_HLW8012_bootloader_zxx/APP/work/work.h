#ifndef __work_h__
#define __work_h__




#include "framework.h"
/*-------------------------------------------------------*/
#define BOOT_CODE 0xA5
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


#define MaxLen_User		32
typedef struct{
	//一个字4字节，4个字16字节
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
	uint8_t user_name[ MaxLen_User ];				//用户名
	//----	4个字
	uint8_t user_pwd[ MaxLen_User ];				//用户密码
	//----	16个字(64字节) 保留	
	uint8_t rec8__[64];						//保留
}storage_t;	//存储部分设计为 32个字，也就是 128 字节


typedef struct{
	//---------------------------------------------
	storage_t* storage;		//存储内容
	//---------------------------------------------
	/*----------------------------------------------*/

	
}wdat_t,*wdat_pt;


extern wdat_pt wdat;
extern void work_data_load( void );
extern void work_data_write( void );
/*-------------------------------------------------------*/
#define MaxSock 2

//typedef struct{
//	uint8_t val[3];	//设备号
//	uint8_t sum;		//设备号检验和
//}wk_num_t,*wk_num_pt;

typedef struct{
//	wk_num_pt num;	//机器信息
	int msta;
	int ssta;
	
}wk_inet_t,*wk_inet_pt;

#include "./work_cpn/work_cpn_typedef.h"




#include "./work_cpn/work_cpn_inc_conf.h"


extern void work_internet(void);



extern void dump_msh_info( char* num,char* date );

extern uint32_t get_msh_terminal_val( void );
extern char* get_msh_terminal_str( void );



#endif

