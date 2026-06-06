#include "SWM320.h"
void SerialInit(void);
//-----------------------------------------------------------
//系统滴答
volatile uint32_t vSysTick = 0;
uint32_t get_sysytick(void)
{
	return vSysTick;
}

void SysTick_Handler(void)
{	
	vSysTick++;
}

//-----------------------------------------------------------
//使用系统滴答，基于状态机思维，编写一个程序
//-鱼缸的鼓气泵：每工作20分钟，就休息2分钟。
static int wsta = 0; //描述工作的状态
static uint32_t otick = 0;		//旧滴答
static uint32_t ntick = 0;		//新滴答

static void init_wsta(void)
{
	wsta = 0;
}

static void work_wsta(void)
{
	switch( wsta )
	{
		case 0:		//上电,初始化一下参数
			otick = get_sysytick();		//记录当前当前时间
			wsta++;
			break;
		
		case 1:		//开启气泵
			
			otick = get_sysytick();		//记录当前当前时间
			printf( "open() ->%d\r\n",otick );
			wsta++;
			break;
		
		case 2:		//持续开启20s后，关闭水泵
			ntick = get_sysytick();		//获取当前的时间
			if( (ntick - otick) < (20*1000) ) break;
		
			//执行到这里，说明经过了20秒
			
			otick = get_sysytick();		//记录当前当前时间
			printf( "close() ->%d\r\n",otick );
			wsta++;
		
			break;
		
		case 3:		//持续关闭10s后，回到状态0
			ntick = get_sysytick();		//获取当前的时间
			if( (ntick - otick) < (10*1000) ) break;
			
			//执行到这里，说明经过了10s分钟
			printf( "cycle() ->%d\r\n",ntick );
			wsta = 1;
			break;

		default:
			break;
	}
}


//-----------------------------------------------------------

int main(void)
{	
	SystemInit();
	SerialInit();
	
	SysTick_Config( SystemCoreClock/1000 );	//每1ms进入一次中断
	//110 592 000 超过了24位
	printf( "system run...\r\n");
	init_wsta();
	while(1==1)
	{
		work_wsta();
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
