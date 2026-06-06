#include "SWM320.h"


void led_ctrl( int pwr )
{
	if( 0 == pwr )
	{
		GPIO_ClrBit(GPIOB, PIN12);	//设置输出0
		GPIO_ClrBit(GPIOM, PIN2 );	//设置输出0
	}
	else
	{
		GPIO_SetBit(GPIOB, PIN12);	//设置输出1
		GPIO_SetBit(GPIOM, PIN2 );	//设置输出1
	}
}

void delay_some_time(void)
{
	volatile uint32_t d= 0;
	d = 10000000;
	while(d--);
	
}

int main(void)
{	
	SystemInit();
	
	//初始化B12 M2为输出功能
	GPIO_Init(GPIOB, PIN12, 1, 0, 0);			//输出， 接LED
	GPIO_Init(GPIOM, PIN2 , 1, 0, 0);			//输出， 接LED
	
	while(1==1)
	{
		led_ctrl( 1 );
		delay_some_time();
		led_ctrl( 0 );
		delay_some_time();
	}
}

