
/*----------------------------------------------------------------------------------*/

#include "framework.h"
/*----------------------------------------------------------------------------------*/
static void Init(uint32_t baudrate);
static void Open(void);
static void Close(void);
static void Write(unsigned char Dat);
static void SetIntRxFunc(IntFunc1 func);
static void EnableIRQ( void );
static void DisableIRQ( void );

static drv_uart_port_t do_drvp_uart={
	.Init = Init,
	.Open = Open,
	.Close = Close,
	.Write = Write,
	.SetIntRxFunc = SetIntRxFunc,
	.EnableIRQ = EnableIRQ,
	.DisableIRQ = DisableIRQ

};


drv_uart_port_pt drvp_uart2 = &do_drvp_uart;

static void IntError(unsigned char Dat){printf("rec = %c\r\n",Dat);}
static IntFunc1 thisDrvIntFunc = IntError; 								//中断调用函数指针
/*----------------------------------------------------------------------------------*/

static void Init(uint32_t baudrate)
{
	UART_InitStructure UART_initStruct;
	//(1)在使用串口之前，初始化IO口的引脚功能。
	GPIO_Init( GPIOM, PIN4,0,0,1 );	//

	PORT_Init( PORTM, PIN4, FUNMUX0_UART2_RXD, 1);	//
 	//(2)分配一个结构体变量——串口配置，并填充它
	/*设置波特率---------------------------------------------------------*/
 	UART_initStruct.Baudrate = baudrate;
	/*设置数据位数------------------------------------------------------*/
	UART_initStruct.DataBits = UART_DATA_8BIT;
	/*设置奇偶校验------------------------------------------------------*/
	UART_initStruct.Parity = UART_PARITY_EVEN;
	/*设置停止位----------------------------------------------------------*/
	UART_initStruct.StopBits = UART_STOP_1BIT;
	/*设置接收FIFO高位提醒阈值----------------------------------*/
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 1;
	/*设置发送FIFO低位提醒阈值----------------------------------*/
	UART_initStruct.TXThreshold = 0;
	UART_initStruct.TXThresholdIEn = 0;
	/*设置接收超时时间 单位:个(字符周期)---------------------*/
	UART_initStruct.TimeoutTime = 250;
	/*是否使能字符接收超时中断-----------------------------------*/
	UART_initStruct.TimeoutIEn = 1;
	
	//(3)使用配置函数，将串口配置的数据生效
 	UART_Init(UART2, &UART_initStruct);
}
/*----------------------------------------------------------------------------------*/

static void Open(void)		{UART_Open(UART2);	}
static void Close(void)		{UART_Close(UART2);	}

static void Write( unsigned char Dat )
{ 
	while(1)
	{
		if( 0 == UART_IsTXBusy(UART2) )
		{
			break;
		}
	}	
	UART_WriteByte(UART2,Dat);
}

/*----------------------------------------------------------------------------------*/
static void SetIntRxFunc(IntFunc1 func){thisDrvIntFunc = func;}
/*----------------------------------------------------------------------------------*/
static void EnableIRQ( void )
{
	NVIC_EnableIRQ(UART2_IRQn);
}

static void DisableIRQ( void )
{
	NVIC_DisableIRQ(UART2_IRQn);
}

/*----------------------------------------------------------------------------------*/
void UART2_Handler(void)
{
	uint32_t chr;
#ifdef old_sdk
	/*---------------------------------------------------------------*/
	//接收类型中断:接收高位中断/接收超时中断
//	if(UART_INTRXThresholdStat(UART2) || UART_INTTimeoutStat(UART2)){
//		while(UART_IsRXFIFOEmpty(UART2) == 0)	{//成立，说明RX FIFO非空
//			if(UART_ReadByte(UART2, &chr) == 0)	{thisDrvIntFunc(chr);}
//		}
//	}
#else	
	
	if(UART_INTStat(UART2, UART_IT_RX_THR | UART_IT_RX_TOUT))
	{
		while(UART_IsRXFIFOEmpty(UART2) == 0)
		{
			if(UART_ReadByte(UART2, &chr) == 0)
			{
				{thisDrvIntFunc(chr);}
			}
		}
	}
#endif
}


/*----------------------------------------------------------------------------------*/

