#include "work.h"
#include "air724_innner.h"

#define PRT_LOG(X...)	sysprt->alog(X)	

 air_dev_pt air724;


void create_dev_air724(void)
{

//	char* argv[5];

//	int err = 0,ret = 0,line = 0;

		
//	air_dev_pt  this_dev;
	//-----------------------------------------------------------------
	//创建设备
	air724 = air_man->create_dev( uart1 );
	//-----------------------------------------------------------------
	//安装函数
	air724->auto_baudrate 	= air724_auto_baudrate;
	air724->cmd_AT 					= air724_cmd_AT;
	air724->cmd_AT_CGMI 		= air724_cmd_AT_CGMI;
	air724->cmd_AT_CGMR 		= air724_cmd_AT_CGMR;
	air724->cmd_AT_CPIN 		= air724_cmd_AT_CPIN;
	air724->cmd_AT_CSQ 			= air724_cmd_AT_CSQ;
	air724->cmd_AT_CREG 		= air724_cmd_AT_CREG;
	air724->cmd_AT_CGATT 		= air724_cmd_AT_CGATT;
	air724->cmd_AT_CSTT 		= air724_cmd_AT_CSTT;
	
	air724->cmd_AT_CIICR 		= air724_cmd_AT_CIICR;
	air724->cmd_AT_CIFSR 		= air724_cmd_AT_CIFSR;
	
	air724->cmd_AT_CIPMODE 	= air724_cmd_AT_CIPMODE;
	air724->cmd_AT_CIPMUX 	= air724_cmd_AT_CIPMUX;
	air724->cmd_AT_CIPSTART = air724_cmd_AT_CIPSTART;
	air724->cmd_AT_CIPCLOSE = air724_cmd_AT_CIPCLOSE;
	
	air724->cmd_AT_RESET 		= air724_cmd_AT_RESET;
}

void air724_rst(void)
{
	
	gpio->write( IO_NetRst,0 );
	task_delay(100);
	gpio->write( IO_NetRst,1 );
	task_delay(100);
	gpio->write( IO_NetRst,0 );
	task_delay(100);
	
}








