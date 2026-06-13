#ifndef __message_h__
#define __message_h__

#include "framework.h"
#include "stdlib.h"
#include "network.h"



typedef int (*cmd_cb_t)( void* ndev,int argc,char** argv );

typedef struct{
	char* name;				//字符串
	cmd_cb_t entry;		//操作函数

}net_cmd_t;

extern int cat_get_time_flag( void );

extern int net_cycle( void* ndev );
extern int msg_cycle( void* ndev,int argc,char** argv );
/*----------------------------------------------------------------------------------*/
//msg net to ctrl
#define MsgLen 64					//固定消息的长度为64字节

enum{
	_e_ntc_type_on = 0,			//启动订单消息
	_e_ntc_type_off,				//结束订单消息
};
typedef struct{
	uint32_t type;					//消息的类型
	uint32_t road;					//描述第几路插座,从0开始数
	uint32_t time;					//充电时长 单位是分钟
	uint32_t rec[5];				//保留
	
	char ddh[32];						//订单号

}order_dat_t;

typedef struct{
	void (*init)( void );
	void (*put_order)( int type,int road,uint32_t time,char* ddh );
	int (*get)( void* data,int len );
	
}msg_ntc_opt_t;
extern msg_ntc_opt_t* msg_ntc_opt;
/*----------------------------------------------------------------------------------*/
typedef struct{
	void (*init)( void );
	void (*put_curr_report)( char* ddh,int num,int road,float curr,float pwr,float vol,uint32_t usetime );
	void (*put_over_report)( char* ddh,float ekwh,uint32_t time,uint32_t event );
	void (*work)( void *ndev );
}msg_ctn_opt_t; 
	
extern msg_ctn_opt_t* msg_ctn_opt;
/*----------------------------------------------------------------------------------*/


#endif



