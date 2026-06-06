#include "task_inc.h"
#include "framework.h"
#include <stdlib.h>
extern void SystemInit(void);



int main(void)
{ 	
	
 	SystemInit();
	
	
	task_start->create(NULL,NULL);
//	xTaskCreate(TaskADC, (const char *)"ADC", 128, 0, 2, 0); 
	
	vTaskStartScheduler();
}


