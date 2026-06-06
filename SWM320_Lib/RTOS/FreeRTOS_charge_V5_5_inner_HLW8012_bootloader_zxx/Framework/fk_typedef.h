

typedef unsigned char U8;
typedef   signed char S8;
typedef unsigned short U16;
typedef   signed short S16;
typedef unsigned int U32;
typedef   signed int S32;


#define __ON		1
#define __OFF		0

#define __DIR_Z		1
#define __DIR_F		0

#define __H 1
#define __L 0

#define __Enable 		1
#define __Disable		0

#define bFALSE	0
#define bTRUE	1


#define Is_Empty			0			//空的
#define Is_Filled			1		  //被填充过



#define PNULL (unsigned char *)0

/*----------------------------------------------------------------------------------*/
//队列接口
#define Using_thread_mq										
#define thread_mq_t 															QueueHandle_t 
#define thread_mq_Pt															QueueHandle_t*																									
#ifdef Using_thread_mq
#define thread_mq_init(mq_name,OneSize,MqNum) 		xQueueCreate((UBaseType_t ) MqNum,(UBaseType_t ) OneSize)
#define thread_mq_destroy(mq)											//rt_mq_delete(mq)
#define thread_mq_test(mq)							
#define thread_mq_send(mq,sBuf,Size,time)					xQueueSend(mq,sBuf,time);
#define thread_mq_recv(mq,rBuf,Size,time)					xQueueReceive(mq,rBuf,time);
#define thread_mq_irq_send(mq,sBuf,Size,time)			xQueueSendFromISR(mq,sBuf,time);


#define thread_mq_send_irq(mq,sBuf,Size,time)			{					\
	BaseType_t xHigherPriorityTaskWoken;											\
  	xHigherPriorityTaskWoken = pdFALSE;											\
	xQueueSendFromISR(mq,sBuf,&xHigherPriorityTaskWoken);			\
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);							\
}
#define thread_mq_recv_irq(mq,rBuf,Size,time)			{					\
	BaseType_t xHigherPriorityTaskWoken;											\
	xHigherPriorityTaskWoken = pdFALSE;												\
	xQueueReceiveFromISR(mq,rBuf,&xHigherPriorityTaskWoken);	\
}
#endif

/*----------------------------------------------------------------------------------*/
/* 线程安全锁 */
#define Using_thread_cslock										

#define thread_cslock_t												SemaphoreHandle_t 												//安全锁类型
#define thread_cslock_Pt											SemaphoreHandle_t*												//安全锁类型指针
#ifdef Using_thread_cslock
#define thread_cslock_WaitMax									portMAX_DELAY
#define thread_cslock_init(cslock_name) 			xSemaphoreCreateMutex()	
#define thread_cslock_destroy(cslock) 				vSemaphoreDelete(cslock)
#define thread_cslock_test(cslock) 						
#define thread_cslock_lock(cslock,time) 			xSemaphoreTake(cslock,time)	
#define thread_cslock_free(cslock) 						xSemaphoreGive( cslock )		

#endif
/*----------------------------------------------------------------------------------*/
//内存管理
#define mem_get_free() xPortGetFreeHeapSize()			//例如 int vfree = mem_get_free();
#define mem_alloc(size_bytes) pvPortMalloc(size_bytes)	//eg. u8* buf = (u8*)mem_alloc(100);
#define mem_free(pv) vPortFree(pv)						//eg. mem_free(buf);
/*----------------------------------------------------------------------------------*/
#define reset_system() { \
	__disable_irq(); \
	WDT_Init(WDT, SystemCoreClock/2, WDT_MODE_RESET); \
	WDT_Start(WDT); \
	while(1); \
}
/*----------------------------------------------------------------------------------*/


//系统滴答
typedef uint32_t time_t ;
#define MaxTick 0XFFFFFFFFUL



#define get_sys_ticks() ( xTaskGetTickCount()*(1000.0/configTICK_RATE_HZ) )





#define thread_delay_ms( X ) { vTaskDelay(X*configTICK_RATE_HZ/1000.0);}

#define task_delay(tick) vTaskDelay(tick);

/*----------------------------------------------------------------------------------*/
/*自动初始化模块*/
#include "./loader/loader.h"




/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
//包含驱动的使用方法
#include "./drivers/fk_drivers_typedef.h"
/*----------------------------------------------------------------------------------*/
//包含软件包的类型
#include "./softpack/fk_softpack_typedef.h"

/*----------------------------------------------------------------------------------*/

