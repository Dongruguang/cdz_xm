#include "SWM320.h"



void SerialInit(void);

int main(void)
{
	SystemInit();			//初始化系统时钟
	
	SerialInit();			//初始化串口
   
	printf("system run...\r\n");
	
	while(1==1)
	{
		
	}
}


void UART0_Handler(void)
{
	uint32_t chr;
	
	//查询接收FIFO中断
	if(UART_INTStat(UART0, UART_IT_RX_THR))
	{
		while((UART0->FIFO & UART_FIFO_RXLVL_Msk) > 1)
		{
			if(UART_ReadByte(UART0, &chr) == 0)
			{
				//在中断中使用printf，仅作为演示，实际项目，不要使用。
				printf("%c",chr);
			}
		}
	}
//	//查询接收超时中断

	if(UART_INTStat(UART0, UART_IT_RX_TOUT))
	{
		while(UART_IsRXFIFOEmpty(UART0) == 0)
		{
			if(UART_ReadByte(UART0, &chr) == 0)
			{
				printf("%c",chr);
			}
		}
	}
	
	//发送FIFO中断
	if(UART_INTStat(UART0, UART_IT_TX_THR))
	{
		//不用发送中断
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;						//配置变量
	
	//初始化引脚功能
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
 	UART_initStruct.Baudrate = 115200;
	UART_initStruct.DataBits = UART_DATA_8BIT;		//8bit数据位
	UART_initStruct.Parity = UART_PARITY_NONE;		//无校验
	UART_initStruct.StopBits = UART_STOP_1BIT;		//1位停止位
	
	//当RX FIFO中数据个数 >  RXThreshold时触发中断
	UART_initStruct.RXThreshold = 0;							
	UART_initStruct.RXThresholdIEn = 1;						//使用接收中断
	
	//当TX FIFO中数据个数 <= TXThreshold时触发中断
	UART_initStruct.TXThreshold = 0;
	UART_initStruct.TXThresholdIEn = 0;						//不使用发送中断
	
	//超时中断，RX FIFO非空，且超过 TimeoutTime/(Baudrate/10) 秒没有在RX线上接收到数据时触发中断

	UART_initStruct.TimeoutTime = 255;		//10个字符时间内未接收到新的数据则触发超时中断
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
