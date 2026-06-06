
#include "task_inc.h"
#include "framework.h"

#include "communication.h"



/*---------------------------------------------------------*/

static uint8_t buf[20];
static cframe_pt frame = (cframe_pt)buf;
static uint32_t sta = 0;
static uint32_t read = 0;
static uint32_t dest = 0;

void cmd_analysis_handle(void)
{
	int i = 0;
	int ret = 0;
	
	switch( sta )
	{
		case 0://收取帧头
			read = 0;	
			dest = 1; 
			
			ret = uart1->read_recv_buf( &buf[read], dest-read );
			read = read + ret;
			
			if( read != dest ) break;
			
			if( buf[0] != cmd_start ) break;
			
			dest = cmd_len;				
			sta ++;						
			break;
		case 1://收满数据
			//收满数据
			ret = uart1->read_recv_buf( &buf[read],dest-read );
			read = read + ret;
			
			if( read != dest ) break;
			//贴出来
			APP_LogOUT(PRINT_FONT_BLU);
			for( int i=0;i<cmd_len;i++)\
				APP_LogOUT("%02x ", ( (uint8_t *)frame )[i] );\
			APP_LogOUT("\t");
			APP_LogOUT(PRINT_ATTR_REC); 
			//校验帧尾
			if( cmd_end != buf[ cmd_len-1 ] )
			{
				APP_LogOUT("cmd_end-->");
				goto __cmd_error;
			}
			//校验和
			ret = 0;
			for( i=0;i<(cmd_len-2);i++ )
			{
				ret = ret + buf[i];
			}
			if( ret != buf[i] ) 
			{
				APP_LogOUT("check sum-->");
				goto __cmd_error;
			}
			
//			frame = (cframe_pt)buf;
			goto __cmd_analysis;
			//break;
		default:
			break;
__cmd_error:
	sta = 0;
	APP_LogOut_ERROR("__cmd_error\r\n");
	return;		
__cmd_analysis:
	sta = 0;
	
	cmd_man->deal( frame );
	
	return;
		
	}//end switch( sta )
	

}







