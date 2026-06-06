
#include "framework.h"
/*------------------------------------*/
static int rti_start(void)
{
	SYS_LogOUT("rti_start");
	return 0;
}
INIT_EXPORT(rti_start, "0");
/*------------------------------------*/
#if 0
static int rti_board_start(void)
{
	SYS_LogOUT("rti_start_1");
	return 0;
}
INIT_EXPORT(rti_start_1, "1");
/*------------------------------------*/

static int rti_start_2(void)
{
	SYS_LogOUT("rti_start_2");
	return 0;
}
INIT_EXPORT(rti_start_2, "2");
/*------------------------------------*/

static int rti_start_3(void)
{
	SYS_LogOUT("rti_start_3");
	return 0;
}
INIT_EXPORT(rti_start_3, "3");
/*------------------------------------*/

static int rti_start_4(void)
{
	SYS_LogOUT("rti_start_4");
	return 0;
}
INIT_EXPORT(rti_start_4, "4");
/*------------------------------------*/


static int rti_start_5(void)
{
	SYS_LogOUT("rti_start_5");
	return 0;
}
INIT_EXPORT(rti_start_5, "5");
/*------------------------------------*/

static int rti_start_6(void)
{
	SYS_LogOUT("rti_start_6");
	return 0;
}
INIT_EXPORT(rti_start_6, "6");
/*------------------------------------*/
#endif
static int rti_end(void)
{
	SYS_LogOUT("rti_end");
	return 0;
}
INIT_EXPORT(rti_end,"12");
/*------------------------------------*/
void rt_components_init(void)
{
	const init_fn_t* fn_ptr;
	int i = 0;
	SYS_LogOUT("rt_components_init:\r\n");
	for (fn_ptr = &__rt_init_rti_start; fn_ptr < &__rt_init_rti_end; )
	{
		SYS_LogOUT("\t[%02d]:",i++);			
		(*fn_ptr)();
		fn_ptr ++;
		SYS_LogOUT("\r\n");		
	}	
}
/*------------------------------------*/

//void rti_board_init(void)
//{
//	const init_fn_t* fn_ptr;
//	int i = 0;
//	SYS_LogOUT("rt_components_init:\r\n");
//	for (fn_ptr = &__rt_init_rti_start; fn_ptr < &__rt_init_rti_end; )
//	{
//		SYS_LogOUT("\t[%02d]:",i++);			
//		(*fn_ptr)();
//		fn_ptr ++;
//		SYS_LogOUT("\r\n");		
//	}
//	
//	
//}

/*
static int init_module(void)
{
	APP_LogOUT("init_module \r\n");
}

INIT_PREV_EXPORT( init_module );


*/


