#include "task_inc.h"
#include "framework.h"

#include "communication.h"
/*----------------------------------------------------------------------------------*/
//注册一条命令到命令管理器
#define cmd_reg( code,fn ) {	\
	APP_LogOut_Color( PRINT_FONT_BLU,"\r\n\t\treg 0x%02x ->"#fn,code );  \
	if( 0> cmd_man->reg(code,fn) ) { \
		APP_LogOut_Color(PRINT_FONT_RED,"\t faild!");\
	}else{\
		APP_LogOut_Color(PRINT_FONT_GRE,"\t success!");\
	} \
}


/*----------------------------------------------------------------------------------*/
#include "cmd_lib_ctrlconf.h"

static void exec_CtrlConf( void *frame )
{

	APP_LogOUT(PRINT_FONT_YEL); APP_LogOUT("exec_CtrlConf \r\n");APP_LogOUT(PRINT_ATTR_REC); 
	cmd_ctrl_conf_pt cmd = (frame);
	switch( cmd->module )
	{
		case _ctrl_conf_outputbus:
			
			
			break;
		case _ctrl_conf_intputbus:
			switch( cmd->action )
			{
				case 0://关闭引脚状态上报
					inb_report->cycle_report_set( __Disable );
					break;
				case 1://开启引脚状态上报
					inb_report->cycle_report_set( __Enable );
					break;
				case 2://关闭引脚事件上报功能
					inb_report->event_report_set( cmd->arg[0], 0x00 );
					break;
				case 3://开启引脚事件上报功能
					inb_report->event_report_set( cmd->arg[0], cmd->arg[1] );
					break;
			}
			
			break;
		
	}
}
/*----------------------------------------------------------------------------------*/

static void exec_Heartbeat( void *cframe )
{
	APP_LogOUT(PRINT_FONT_YEL); APP_LogOUT("exec_Heartbeat \r\n");APP_LogOUT(PRINT_ATTR_REC); 
}
static void exec_OutBusCtrl( void *cframe )
{
	APP_LogOUT(PRINT_FONT_YEL); APP_LogOUT("exec_OutBusCtrl \r\n");APP_LogOUT(PRINT_ATTR_REC); 
}

/*---------------------------------------------------------*/


static void exec_InBusRpt( void *cframe )
{
	APP_LogOUT(PRINT_FONT_YEL); APP_LogOUT("exec_InBusRpt \r\n");APP_LogOUT(PRINT_ATTR_REC); 
}



/*---------------------------------------------------------*/
#include "cmd_lib_audio.h"
static uart_pt dev_to_uart( uint8_t dev )
{
	switch( dev )
	{
		case 0:return uart2;
		case 1:return uart3;
		default:
			break;
	}
	return 0;
}
static void exec_AudioDev( void *cframe )
{
	APP_LogOUT(PRINT_FONT_YEL); APP_LogOUT("exec_AudioDev \r\n");APP_LogOUT(PRINT_ATTR_REC); 	
	cmd_audio_pt cmd = cframe;
	uart_pt uart = dev_to_uart( cmd->dev );
	switch( cmd->action )
	{
		case _audio_init_dev:
			audio_opt->init_dev( uart );
			break;
		case _audio_set_volume:
			audio_opt->set_volume( uart,cmd->arg[0] );
			break;
		case _audio_set_gingle_cycle:
			audio_opt->set_gingle_cycle( uart );
			break;
		case _audio_set_play_now:
			audio_opt->play_now( uart );
			break;
		case _audio_set_play_music:
			audio_opt->play_music(uart,cmd->arg[0] );
			break;
		default:
			break;
			
	}
}
/*---------------------------------------------------------*/

static int init_cmd_lib(void)
{
	APP_LogOUT("init_cmd_lib");
	cmd_reg(__cCode_of_CtrlConf,exec_CtrlConf);
	cmd_reg(__cCode_of_Heartbeat,exec_Heartbeat);
	cmd_reg(__cCode_of_OutBusCtrl,exec_OutBusCtrl);
	cmd_reg(__cCode_of_InBusRpt,exec_InBusRpt);
	cmd_reg(__cCode_of_AudioDev,exec_AudioDev);
	return 0;
}
INIT_APP_EXPORT(init_cmd_lib);





