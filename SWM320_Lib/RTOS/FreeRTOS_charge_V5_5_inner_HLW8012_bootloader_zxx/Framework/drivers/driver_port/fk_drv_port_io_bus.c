#include "framework.h"

#define wide_sl 80
#define wide_rck 50
#define wide_sck 50

static drvp_io_bus_t do_drvp_io_bus;

drvp_io_bus_pt drvp_io_bus = &do_drvp_io_bus;

/*----------------------------------------------------------------------------------*/
static void SPI0_CLK_H(void) 			{ GPIO_SetBit( GPIOA, PIN11 ); }
static void SPI0_CLK_L(void) 			{ GPIO_ClrBit( GPIOA, PIN11 ); }

static void SPI0_MOSI_H(void) 		{ GPIO_SetBit( GPIOA, PIN10 ); }
static void SPI0_MOSI_L(void) 		{ GPIO_ClrBit( GPIOA, PIN10 ); }

static int SPI0_MISO_STA(void){ return GPIO_GetBit( GPIOA, PIN9 ); }


static void SPI0_HC595_CS_H(void) { GPIO_SetBit( GPIOA, PIN8 ); }
static void SPI0_HC595_CS_L(void) { GPIO_ClrBit( GPIOA, PIN8 ); }

static void SPI0_HC595_UD_H(void) { GPIO_SetBit( GPIOA, PIN7 ); }
static void SPI0_HC595_UD_L(void) { GPIO_ClrBit( GPIOA, PIN7 ); }

static void SPI0_HC165_LD_H(void) { GPIO_SetBit( GPIOA, PIN4 ); }
static void SPI0_HC165_LD_L(void) { GPIO_ClrBit( GPIOA, PIN4 ); }



volatile uint32_t d=0;

/*----------------------------------------------------------------------------------*/

static void init( void )
{
	//初始化模拟SPI引脚
	GPIO_Init( GPIOA, PIN11, 1, 1, 0, 0 );	//SPI0 CLK
	GPIO_Init( GPIOA, PIN10, 1, 1, 0, 0 );	//SPI0 MOSI
	GPIO_Init( GPIOA, PIN9 , 0, 1, 0, 0 );	//SPI0 MISO
		
	//初始化HC595 相关引脚
	GPIO_Init( GPIOA, PIN8, 1, 1, 0, 0 );		//HC595_CS
	GPIO_Init( GPIOA, PIN7, 1, 1, 0, 0 );		//HC595_UD
	//初始化HC165 相关引脚	
	GPIO_Init( GPIOA, PIN4, 1, 1, 0, 0 );		//HC165_LD
	
	//使能芯片
	SPI0_HC595_CS_H();
	SPI0_HC595_CS_L();
	SPI0_HC165_LD_L();	//低电平(下降沿)是无效的，保持低电平可以提高稳定性
	
	
}
static void load(void)
{
	SPI0_HC165_LD_L();		 		//HC165加载外部引脚数据到移位寄存器
	d = wide_sl; while(d--);
	SPI0_HC165_LD_H();				//HC165停止加载外部引脚数据
	d = wide_sl; while(d--);
//	SPI0_HC165_LD_L();				//低电平(下降沿)是无效的，保持低电平可以提高稳定性
}
static void update(void)
{
	SPI0_HC595_UD_L();
	d = wide_rck;while(d--);
	SPI0_HC595_UD_H();
	d = wide_rck;while(d--);
//	SPI0_HC595_UD_L();				//低电平(下降沿)是无效的，保持低电平可以提高稳定性
}
/*
对于HC165+HC595,
模拟SPI方式进行数据处理

*/

static uint8_t swap( uint8_t dat )
{
	unsigned char i;
	uint8_t ret = 0;
	for(i=0;i<8;i++)
	{
		//根据数据输出
		if (dat&0x80) SPI0_MOSI_H(); else SPI0_MOSI_L();
		dat<<=1;
		//根据状态读入
		
		if( SPI0_MISO_STA() ) ret |= 0x01;
		if( i!=7 ) 
		{
			ret <<= 1;		  
		}
		//产生上升沿，寄存器移位
		
		SPI0_CLK_L();
		d = wide_sck;while(d--);
		SPI0_CLK_H();
		d = wide_sck;while(d--);
	}

	return ret;
}
static drvp_io_bus_t do_drvp_io_bus={
	.init = init,
	.load = load,
	.update = update,
	.swap = swap,
};
