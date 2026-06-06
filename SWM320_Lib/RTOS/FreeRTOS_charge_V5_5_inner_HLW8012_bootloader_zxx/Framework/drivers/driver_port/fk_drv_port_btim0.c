

/*----------------------------------------------------------------------------------*/

#include "framework.h"

static void fk_drv_btim0_Init(void);
static void fk_drv_btim0_Run(void);
static void fk_drv_btim0_Stop(void);

static void fk_drv_btim0_SetIntFunc(IntFunc0 func);				//设置中断调用



fk_drv_btim0_t DrvBtim0={
	.Init 	= fk_drv_btim0_Init,
	.Run 		= fk_drv_btim0_Run,
	.Stop 	= fk_drv_btim0_Stop,
	.SetIntFunc = fk_drv_btim0_SetIntFunc,
};

static unsigned int gTimer0Tick;

//此处仅仅为默认中断函数，告知开发者，还没有配置此函数
//请不要在中断里使用串口，因为串口是互斥的硬件设备，并且串口速度会拖慢中断。
static void IntError(void){gTimer0Tick++;printf("gTimer0Tick = %d \r\n",gTimer0Tick);}

static IntFunc0 thisDrvIntFunc = IntError; 								//中断调用函数指针
/*----------------------------------------------------------------------------------*/
static void fk_drv_btim0_Init(void)
{
	/*设置预分频值*/
	unsigned char PerDiv = 48-1;		
	/*设置定时器初值*/
	unsigned int CounterVal = 1000-1;
	/*组合参数*/
	unsigned int periodVal =  PerDiv;
	periodVal  = periodVal << 24;
	periodVal  = periodVal  + CounterVal;
	/*应用设置*/
	TIMR_Init(BTIMR0, TIMR_MODE_TIMER, periodVal, 1);
}
/*----------------------------------------------------------------------------------*/
/*启动定时器*/
static void fk_drv_btim0_Run(void)		{TIMR_Start(BTIMR0);}
/*停止定时器*/
static void fk_drv_btim0_Stop(void)		{TIMR_Stop(BTIMR0);}

/*----------------------------------------------------------------------------------*/
//设置中断调用
static void fk_drv_btim0_SetIntFunc(IntFunc0 func)
{
	thisDrvIntFunc = func;
}

/*----------------------------------------------------------------------------------*/


//static unsigned int fk_drv_btim0_GetTick(void)
//{
//	unsigned int iTick=0;
//	NVIC_DisableIRQ(BTIMR0_IRQn);
//	iTick = gTimer0Tick;
//	NVIC_EnableIRQ(BTIMR0_IRQn);
//	return iTick;
//}
/*----------------------------------------------------------------------------------*/

void BTIMR0_Handler(void)
{
	TIMR_INTClr(BTIMR0);
	thisDrvIntFunc();
}
/*----------------------------------------------------------------------------------*/

