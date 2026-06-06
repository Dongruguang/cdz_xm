#include "task_inc.h"
//#include "framework.h"
#include "work.h"
#include <stdlib.h>

__task_body_start;


static task_body_t m_task_body={
	.name		= "listen_task",
	.cn_name	= "监听任务",
	.stack_size	= 1024,
	.priority = 5,
	.handle = &handle,
	.create = create,
	
}; 


static void task_entry(void *arg)
{
	/*----------------------------------------------------------------------------------*/
	sysprt->alog("run task_entry: %s\r\n",m_task_body.name);
//	printf("run task_entry: %s\r\n",m_task_body.name);
	while(1)
	{
//		hlw8012->work();
		coder->work();
		sock_hlw_opt->listen_work();
		//sock_check_service_work();
		vTaskDelay(5);
	}
}

__task_body_end;


//将本文件的task_body开放给别的文件引用
__task_body_quote_to( task_listen );

