#include "project.h"





int main(void)
{
	SystemInit();			//初始化系统时钟
	
	SerialInit( 115200 );			//初始化串口
   
	printf("system run...\r\n");
	
	while(1==1)
	{
			cmd_line_work();
		
	}
}
