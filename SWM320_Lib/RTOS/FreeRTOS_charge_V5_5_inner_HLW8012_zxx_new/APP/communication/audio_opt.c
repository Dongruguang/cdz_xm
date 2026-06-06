
#include "task_inc.h"
#include "framework.h"

#include "communication.h"
//此文件提供串口音频设备JQ8400的操作方法，具体依赖什么串口，调用者自行填充参数即可
/*--------------------------------------------------------------------------------*/

static void delay_ms( uint32_t time )
{
	vTaskDelay( time );
}

static uint8_t sum_data(uint8_t *buf,uint8_t len)
{
	uint8_t ret = 0;
	for(int i=0;i<len;i++)
		ret = ret + buf[i];
	return ret;
}
/*--------------------------------------------------------------------------------*/

//初始化音频设备
//设置工作模式
static void init_dev( uart_pt uart )
{
	uint8_t buf0[5];
	uint32_t i=0;
	
	buf0[0] = 0xAA;
	buf0[1] = 0X0B;
	buf0[2] = 0x01;
	buf0[3] = 0X02;
	buf0[4] = 0xB8;
	uart->send(buf0,5);
	delay_ms( 100 );
	i=1000;while(i--);
	buf0[0] = 0xAA;
	buf0[1] = 0X1D;
	buf0[2] = 0x01;
	buf0[3] = 0X00;
	buf0[4] = 0xC8;
	uart->send(buf0,5);
	delay_ms( 100 );
}

//设置音量
static void set_volume(uart_pt uart, uint8_t vol)
{
	uint8_t buf0[5];
	if(vol>30) vol = 30;
	buf0[0] = 0xAA;
	buf0[1] = 0X13;
	buf0[2] = 0x01;
	buf0[3] = vol;
	buf0[4] = sum_data(buf0,4);
	uart->send(buf0,5);
}

//设置单曲循环
static void set_gingle_cycle(uart_pt uart )
{
	uint8_t buf0[5];
	buf0[0] = 0xAA;
	buf0[1] = 0x18;
	buf0[2] = 0x01;
	buf0[3] = 0x01;
	buf0[4] = 0xc4;
	uart->send(buf0,5);
}

//开始播放
static void play_now( uart_pt uart )
{
	uint8_t buf0[5];
	buf0[0] = 0xAA;
	buf0[1] = 0x02;
	buf0[2] = 0x00;
	buf0[3] = 0xAC;
	uart->send(buf0,4);
}

static void play_music( uart_pt uart, uint8_t val)
{
	uint8_t buf0[6];
	buf0[0] = 0xAA;
	buf0[1] = 0X07;
	buf0[2] = 0x02;
	buf0[3] = val>>8;			
	buf0[4] = (uint8_t)val;
	buf0[5] = sum_data(buf0,5);
	uart->send(buf0,6);
}


static audio_opt_t m_aduio_opt={
	.init_dev = init_dev,
	.set_volume = set_volume,
	.set_gingle_cycle = set_gingle_cycle,
	.play_now = play_now,
	.play_music = play_music,
};


audio_opt_pt audio_opt = &m_aduio_opt;
