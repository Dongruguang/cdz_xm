#include "task_inc.h"
#include "framework.h"
#include <stdlib.h>
#include "work.h"
__task_body_start;


static task_body_t m_task_body={
	.name		= "host_task",
	.cn_name	= "主机任务",
	.stack_size	= 4096,
	.priority = 3,
	.handle = &handle,
	.create = create,
	
}; 

extern sock_hlw_opt_pt sock_hlw_opt;
static void task_entry( void *arg )
{
	sysprt->alog("run task_entry: %s\r\n",m_task_body.name);
	while(1)
	{
//		led_board->work( );
		sysprt->work( );
		vTaskDelay( 2 );
		
	}
}


__task_body_end;

//将本文件的task_body开放给别的文件引用
__task_body_quote_to( task_host );

