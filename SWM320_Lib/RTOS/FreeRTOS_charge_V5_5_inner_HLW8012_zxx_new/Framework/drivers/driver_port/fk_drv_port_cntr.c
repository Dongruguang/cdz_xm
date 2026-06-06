#include "framework.h"

//cntr本质上是定时器，使用定时器去计数，目的是减少进入中断的次数。

#define FirstVal 0xffffffff			//定时器初值

static volatile uint32_t d = 0;

//CNTR0	P8_FUN0		TIM0
//CNTR1 N9_FUN1		TIM1	HLW0
//CNTR2	N6_FUN0		TIM2
//CNTR3 N5_FUN1		TIM3	HLW1


TIMR_TypeDef* get_tim[] ={ TIMR0,TIMR1,TIMR2,TIMR3 };
static void init(void)
{
	
//	GPIO_Init( GPIOP, PIN8 , 0,1,0 );
//	GPIO_Init( GPIOP, PIN5 , 0,1,0 );
	GPIO_Init( GPION, PIN5 , 0,1,0 );
	GPIO_Init( GPION, PIN9 , 0,1,0 );
	
//	PORT_Init( PORTP, PIN8 , FUNMUX0_TIMR0_IN, 1 );	//TIM0
	PORT_Init( PORTN, PIN5 , FUNMUX1_TIMR1_IN, 1 );	//TIM1
//	PORT_Init( PORTP, PIN0 , FUNMUX0_TIMR2_IN, 1 );	//TIM2
	PORT_Init( PORTN, PIN9 , FUNMUX1_TIMR3_IN, 1 );	//TIM3
	
//	TIMR_Init( TIMR0, TIMR_MODE_COUNTER, FirstVal, 0 );
	TIMR_Init( TIMR1, TIMR_MODE_COUNTER, FirstVal, 0 );
//	TIMR_Init( TIMR2, TIMR_MODE_COUNTER, FirstVal, 0 );
	TIMR_Init( TIMR3, TIMR_MODE_COUNTER, FirstVal, 0 );
	
}


static void start( int road )
{
		TIMR_Start( get_tim[road] );
}

static void stop( int road )
{
		
		TIMR_Stop( get_tim[road] );
		
}

static uint32_t cat(int road)
{
	uint32_t ret = 0;
	ret = TIMR_GetCurValue( get_tim[road] );
	ret = FirstVal - ret;
	return ret;
}

static void clr( int road )
{
	TIMR_Stop( get_tim[road] );
	d = 10;while(d--);
	TIMR_SetPeriod( get_tim[road],FirstVal );
	d = 10;while(d--);
}

static void reset( int road )
{
	TIMR_Stop( get_tim[road] );
	d = 10;while(d--);
	TIMR_SetPeriod( get_tim[road],FirstVal );
	d = 10;while(d--);
	TIMR_Start( get_tim[road] );
	d = 10;while(d--);
}

static drvp_cntr_t do_drvp_cntr ={
	.init = init,
	.start = start,
	.stop = stop,
	.cat = cat,
	.clr = clr,
	.reset = reset
};

drvp_cntr_pt drvp_cntr = &do_drvp_cntr;

