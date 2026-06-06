/*----------------------------------------------------------------------------------*/

#include "framework.h"
/*----------------------------------------------------------------------------------*/
static void fk_drv_spi0_Init(void);
static void fk_drv_spi0_Open(void);
static void fk_drv_spi0_Close(void);
static unsigned char fk_drv_spi0_Swap(unsigned char Dat);
static void fk_drv_spi0_Div(unsigned char div);
fk_drv_spi0_t DrvSpi0={
	.Init 	= fk_drv_spi0_Init,
	.Open 	= fk_drv_spi0_Open,
	.Close 	= fk_drv_spi0_Close,
	.Swap 	= fk_drv_spi0_Swap,
	.Div 		= fk_drv_spi0_Div,
};
/*分配一个SPI配置结构体，描述SPI0的配置*/
static 	SPI_InitStructure SPI_initStruct;

/*----------------------------------------------------------------------------------*/
static void fk_drv_spi0_Init(void)
{
	/*初始化SPI功能引脚*/
	//PORT_Init(PORTA, PIN8,  PORTA_PIN8_SPI0_SSEL,  0);
	PORT_Init(PORTA, PIN11, PORTA_PIN11_SPI0_SCLK, 0);
	PORT_Init(PORTA, PIN10, PORTA_PIN10_SPI0_MOSI, 0);
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_SPI0_MISO,  1);
	//GPIO_Init(GPIOC, PIN1, 1, 1, 0, 0); //CS

	/*分频，降低SPI模块工作速度*/
	SPI_initStruct.clkDiv 				= SPI_CLKDIV_256;		//1.5MHz
	/*配置SPI模块工作模式:SPI模式还是TI_SSI模式*/
	SPI_initStruct.FrameFormat 		= SPI_FORMAT_SPI;		//选择SPI模式
	/*配置在SPI帧格式下的采样时刻:上升沿采样还是下降沿采样
		第一个边沿是上升沿，第二个边沿是下降沿*/
	SPI_initStruct.SampleEdge 		= SPI_FIRST_EDGE;		//选择第一个边沿
	/*配置在SPI帧格式下，空闲时的电平:*/
	SPI_initStruct.IdleLevel 			= SPI_LOW_LEVEL;		//空闲时为低电平
	/*接收字长度*/
	SPI_initStruct.WordSize 			= 8;
	/*配置是否使用SPI主机模式*/
	SPI_initStruct.Master 				= 1;
	/*接收中断是否使能*/
	SPI_initStruct.RXThresholdIEn = 0;
	/*发送中断是否使能*/
	SPI_initStruct.TXThresholdIEn = 0;
	/*是否允许TX FIFO或者移位寄存器空的中断请求*/
	SPI_initStruct.TXCompleteIEn  = 0;
	/*应用这些配置*/
	SPI_Init(SPI0, &SPI_initStruct);
}
/*----------------------------------------------------------------------------------*/
static void fk_drv_spi0_Div(unsigned char div)
{
	SPI_initStruct.clkDiv 				= div;		//1.5MHz
	SPI_Init(SPI0, &SPI_initStruct);
}
/*----------------------------------------------------------------------------------*/
/*使能SPI模块*/
static void fk_drv_spi0_Open(void)												{SPI_Open(SPI0);}
/*----------------------------------------------------------------------------------*/
static void fk_drv_spi0_Close(void)												{SPI_Close(SPI0);}
/*----------------------------------------------------------------------------------*/
static unsigned char fk_drv_spi0_Swap(unsigned char Dat)	{return SPI_ReadWrite(SPI0,Dat);}
/*----------------------------------------------------------------------------------*/

