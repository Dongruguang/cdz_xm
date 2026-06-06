#include "SWM320.h"
#define Addr_GPIOB_Base	0x40012000
#define Addr_GPIOM_Base	0x40015000

#define Offset_GPIO_DATA	(0x00)
#define Offset_GPIO_DIR		(0x04)

//PORTG BASE： 0x40010000
#define Addr_PORT_Base	( 0x40010000 )

#define Offset_PORTB_SEL	(0x04)

#define Offset_PORTM_SEL0	(0x20)
#define Offset_PORTM_SEL1	(0x24)

#define Offset_PORTB_MUX0	(0x110)
#define Offset_PORTB_MUX1	(0x114)

#define Offset_PORTM_MUX0	(0x160)
#define Offset_PORTM_MUX1	(0x164)
#define Offset_PORTM_MUX2	(0x168)
#define Offset_PORTM_MUX3	(0x16C)

#define Offset_PORTB_PULLD	(0x300)	//下拉使能控制寄存器

#define Offset_PORTB_INEN	(0x610)	

#define Offset_PORTM_INEN	(0x640)	




#define rGPIOB_DATA	 ( *(volatile unsigned int*)( Addr_GPIOB_Base + Offset_GPIO_DATA ) )
#define rGPIOB_DIR	 ( *(volatile unsigned int*)( Addr_GPIOB_Base + Offset_GPIO_DIR  ) )

#define rPORTB_SEL	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTB_SEL ) )
#define rPORTB_MUX0	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTB_MUX0 ) )
#define rPORTB_MUX1	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTB_MUX1 ) )
#define rPORTB_PULLD	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTB_PULLD ) )
#define rPORTB_INEN	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTB_INEN ) )



#define rGPIOM_DATA	 ( *(volatile unsigned int*)( Addr_GPIOM_Base + Offset_GPIO_DATA ) )
#define rGPIOM_DIR	 ( *(volatile unsigned int*)( Addr_GPIOM_Base + Offset_GPIO_DIR  ) )

#define rPORTM_SEL0	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTM_SEL0 ) )
#define rPORTM_SEL1	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTM_SEL1 ) )
#define rPORTM_MUX0	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTM_MUX0 ) )
#define rPORTM_MUX1	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTM_MUX1 ) )
#define rPORTM_MUX2	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTM_MUX2 ) )
#define rPORTM_MUX3	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTM_MUX3 ) )
#define rPORTM_INEN	 ( *(volatile unsigned int*)( Addr_PORT_Base + Offset_PORTM_INEN ) )


//时钟门控寄存器	CLKEN
//CLKEN 0x08 R/W 0x00 时钟门控寄存器
#define Addr_SYSCON_Base	0x40000000
#define Offset_SYS_CLKEN	(0x08)

#define rSYS_CLKEN	 ( *(volatile unsigned int*)( Addr_SYSCON_Base + Offset_SYS_CLKEN ) )





void gpio_init_out(int pwr )
{
	//使能外设时钟 B M
	rSYS_CLKEN |= (1<<1);
	rSYS_CLKEN |= (1<<4);
	
	//<1>将引脚的基本属性设置为GPIO	B12/M2
	rPORTB_SEL &=  ~(3<<24);
	rPORTM_SEL0 &=  ~(3<<4);
	
	//<2>接通端口数据输出开关
	rGPIOB_DIR |= (1<<12);
	rGPIOM_DIR |= (1<<2 );
	
	//<3>关闭端口数据输入开关
	rPORTB_INEN &= ~(1<<12);
	rPORTM_INEN &= ~(1<<2 );

	//<4>往端口引脚写数据，实现输出指定电平值
	if( 0 == pwr )
	{
		rGPIOB_DATA &= ~(1<<12);	//B12输出低电平
		rGPIOM_DATA &= ~(1<<2 );	//M2 输出低电平
	}
	else
	{
		rGPIOB_DATA |= (1<<12);	//B12输出高电平
		rGPIOM_DATA |= (1<<2 );	//M2 输出高电平
	
	}
	
}

void led_ctrl( int pwr )
{
	if( 0 == pwr )
	{
		rGPIOB_DATA &= ~(1<<12);	//B12输出低电平
		rGPIOM_DATA &= ~(1<<2 );	//M2 输出低电平
	}
	else
	{
		rGPIOB_DATA |= (1<<12);	//B12输出高电平
		rGPIOM_DATA |= (1<<2 );	//M2 输出高电平
	
	}
}

void delay_some_time(void)
{
	volatile uint32_t d= 0;
	d = 10000000;
	while(d--);
	
}


void SerialInit(void);

int main(void)
{
	uint32_t chr;
	uint32_t err;
	
	SystemInit();
	SerialInit();
   	
	printf("hello swm320\r\n");
	printf("cpu:%d\r\n",SystemCoreClock);

	//编程让某些引脚输出高电平	ELC0 M2		/ ELC1 B12
	gpio_init_out( 1 );
	
	while(1==1)
	{
//		led_ctrl( 1 );
//		delay_some_time();
//		led_ctrl( 0 );
//		delay_some_time();
		
	}
}

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
