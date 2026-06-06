#ifndef __Task_inc_h__
#define __Task_inc_h__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"




#include "./task_body_lib.h"


__task_body_quote( task_start );
__task_body_quote( task_listen );
__task_body_quote( task_ctrl );
__task_body_quote( task_host );





#endif


