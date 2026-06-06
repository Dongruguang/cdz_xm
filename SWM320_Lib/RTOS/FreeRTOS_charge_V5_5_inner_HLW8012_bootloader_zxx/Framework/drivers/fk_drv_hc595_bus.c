#include "framework.h"

/*

本文件设计思想：
->不提供HC595的输出缓冲数据
->实现操作HC595硬件
因为不同的项目，HC595总线对应的数码管会变化
所以：
(1)本DRV文件，只是操作HC595的方法
(2)依赖的DRVP文件，只是描述HC595对应什么电路
从APP看过来，就是一个HC595总线，跟数码管灯位的状态没有任何关系
灯位/数码管灯概念由APP来抽象。

本文件代码排版结构
(1)实现hc595的操作方法
(2)实现hc595总线映射到底层硬件操作
*/


#define wide_rck 50
#define wide_sck 20

enum{
	hc_road_0 = 0,
	hc_road_1,
	hc_road_2,
	hc_road_3,
	MaxHcRoad,
};
static volatile uint32_t d[MaxHcRoad]={0,0,0};


static hc595_pin_t m_LED_BUS = { IO_HC_DAT,IO_HC_CLK,IO_HC_UD,0,hc_road_0 };

hc595_pin_pt LED_BUS = &m_LED_BUS;



//初始化 bus 引脚
static void init( hc595_pin_pt bus )
{
//	drvp_hc595_bus[1] = &do_drvp_hc595_bus_1;
//	drvp_hc595_bus[2] = &do_drvp_hc595_bus_2;

	gpio->set_mode( bus->dat,PIN_MODE_OUTPUT );
	gpio->set_mode( bus->clk,PIN_MODE_OUTPUT );
	gpio->set_mode( bus->ud,PIN_MODE_OUTPUT );
	gpio->set_mode( bus->cs,PIN_MODE_OUTPUT );
	
	
}


static void write_byte( hc595_pin_pt bus,uint8_t dat )
{
	int road = bus->index;
	unsigned char i;
	for(i=0;i<8;i++)
	{
		//根据数据输出
		if (dat&0x80) 
			gpio->write( bus->dat,PIN_HIGH );
		else 
			gpio->write( bus->dat,PIN_LOW );
		dat<<=1;
		//产生上升沿，寄存器移位	
		
		gpio->write( bus->clk,PIN_LOW );				//drvp_hc595_bus[road]->CLK_L();
		d[road] = wide_sck;while( d[road] --);
		gpio->write( bus->clk,PIN_HIGH );				//drvp_hc595_bus[road]->CLK_H();

		d[road] = wide_sck;while( d[road] --);
	}
}

static void write(  hc595_pin_pt bus, uint8_t* buf,uint8_t len )
{
	for( int i=0;i<len;i++ )
		write_byte( bus, buf[ len-1-i ] ); //先从远处开始送数据
}

static void update( hc595_pin_pt bus )
{
	int road = bus->index;
	gpio->write( bus->ud,PIN_LOW );
	d[road] = wide_rck;while( d[road]--);
	gpio->write( bus->ud,PIN_HIGH );
	d[road] = wide_rck;while( d[road] --);
	
}

static hc595_opt_t do_hc595_bus={init,write,update};

hc595_opt_pt hc595_opt = &do_hc595_bus;


