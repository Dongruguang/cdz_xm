#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SWM320.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "./fk_typedef.h"
#include "./drivers/fk_drivers.h"
#include "./softpack/fk_softpack.h"
#include "./fk_log_color.h"

/*flash分配计划*/
#define addr_app1					0x50000			//[0x0007 FFFF - 0x0005 0000] :  320KB~ 	512KB		==>192KB  app1 
#define addr_app0					0x20000			//[0x0004 FFFF - 0x0002 0000] :  128KB~ 	320KB		==>192KB  app0 (fac app)
#define addr_config				0x10000			//[0x0001 FFFF - 0x0001 0000] :  64 	~		128KB		==>64KB		config_data
#define addr_boot					0x0000			//[0x0000 FFFF - 0x0000 0000] :  0		~			64KB	==>64KB		boot


/*-------------------------------------------------------*/
#define BL_Version                      5               /**< App major version number */
#define BL_SubVersion                   5               /**< App minor version number */




#define BoardV5_4			0
#define BoardV5_5			1





#define BoardVision 	BoardV5_4
//#define BoardVision 	BoardV5_5
/*-------------------------------------------------------*/


#if ( BoardVision == BoardV5_4)

#define Brd_Version                      5               /**< Board major version number */
#define Brd_SubVersion                   4               /**< Board minor version number */


//继电器
//继电器
#define IO_ELC0			26
#define IO_ELC1			3

#define IO_fuse0		21					//旧板子只有一个保险丝，以0为准
#define IO_fuse1		21
#define IO_BEE			27


//合力为功率检测芯片
#define HL_TX0			22
#define HL_RX0			0
#define HL_PF0			0

#define HL_TX1			20
#define HL_RX1			0
#define HL_PF1			0

//过流检测
#define Hight0			0
#define Hight1			0

#define IO_SLED			17			

#define IO_NetRst		56			//4G模块复位引脚

#define IO_HC_DAT	36
#define IO_HC_CLK	35
#define IO_HC_UD	37


#endif

/*-------------------------------------------------------*/
#if ( BoardVision == BoardV5_5)

#define Brd_Version                      5               /**< Board major version number */
#define Brd_SubVersion                   5               /**< Board minor version number */


//继电器
#define IO_ELC0			3
#define IO_ELC1			48

#define IO_fuse0		9
#define IO_fuse1		55
#define IO_BEE			15


//合力为功率检测芯片
#define HL_TX0			8
#define HL_RX0			63
#define HL_PF0			64

#define HL_TX1			58
#define HL_RX1			56
#define HL_PF1			57

//过流检测
#define Hight0			60
#define Hight1			59

#define IO_SLED			54			

#define IO_NetRst		43			//4G模块复位引脚


#define IO_HC_DAT	35
#define IO_HC_CLK	36
#define IO_HC_UD	34


#endif




#endif



