#include "SWM320.h"
void SerialInit(void);
//-----------------------------------------------------------
//系统滴答
volatile uint32_t vSysTick = 0;
uint32_t get_sysytick(void)
{
	return vSysTick;
}


void TIMR0_Handler(void)
{
	TIMR_INTClr(TIMR0);
	vSysTick++;
	
}


//-----------------------------------------------------------

int main(void)
{	
	SystemInit();
	SerialInit();

	//系统滴答定时器是24位	通用定时器是32位
	//SystemCoreClock 表示1s中的脉冲数
	TIMR_Init(TIMR0, TIMR_MODE_TIMER, SystemCoreClock/1000, 1);	//每1ms触发一次中断
	
	TIMR_Start(TIMR0);
	
	printf( "system run...\r\n");
	while(1==1)
	{
		printf("timer0 tick = %08d\r\n",get_sysytick() );
	}
}





//-----------------------------------------------------------

void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
 	UART_initStruct.Baudrate = 115200;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
