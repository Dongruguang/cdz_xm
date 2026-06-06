
/*----------------------------------------------------------------------------------*/

#include "framework.h"


/*----------------------------------------------------------------------------------*/

ADC_InitStructure ADC_initStruct;


/*----------------------------------------------------------------------------------*/
static	void Init(void)
{
	
	/*初始化具体的引脚为ADC输入引脚*/
	PORT_Init(PORTN, PIN2,  PORTN_PIN2_ADC1_IN6, 0);	//
	PORT_Init(PORTN, PIN1,  PORTN_PIN1_ADC1_IN5, 0);	//
//	PORT_Init(PORTN, PIN0,  PORTN_PIN0_ADC1_IN4, 0);	//
//	
//	PORT_Init(PORTC, PIN4,  PORTC_PIN4_ADC1_IN3, 0);	//
//	PORT_Init(PORTC, PIN5,  PORTC_PIN5_ADC1_IN2, 0);	//
//	PORT_Init(PORTC, PIN6,  PORTC_PIN6_ADC1_IN1, 0);	//
//	PORT_Init(PORTC, PIN7,  PORTC_PIN7_ADC1_IN0, 0);	//
	/*设置ADC0外设时钟*/
	ADC_initStruct.clk_src = ADC_CLKSRC_HRC;
	ADC_initStruct.clk_div = 31;
	ADC_initStruct.pga_ref = PGA_REF_INTERNAL;
	
	
	/*接通哪些通道?可使用 按位与 同时使能多个通道 */
	ADC_initStruct.channels = 
		ADC_CH5|ADC_CH6;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGSRC_SW;
	ADC_initStruct.Continue = 1;								//连续模式
	
	//中断配置
	ADC_initStruct.EOC_IEn = 0;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_initStruct.HFULL_IEn = 0;
	ADC_initStruct.FULL_IEn = 0;
	
	
	ADC_Init(ADC1, &ADC_initStruct);						//配置ADC
}
/*----------------------------------------------------------------------------------*/
static	void Open(void)		{ ADC_Open(ADC1);}
/*----------------------------------------------------------------------------------*/
static	void Close(void)	{ ADC_Close(ADC1);}
/*----------------------------------------------------------------------------------*/
static	void Start(void)	{ ADC_Start(ADC1);}
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/

static void ReInit(void *Info)
{
	ADC_Init(ADC1, Info);						//配置ADC
}

/*----------------------------------------------------------------------------------*/
const uint8_t adc_road[ADC1_UseCh]={6,5};

static int Cat_Ready( int road )
{
	if(  ADC1->CH[ adc_road[road] ].STAT & 0x04 )//转换结束了
	{
		return 1;
	}
	else
		return 0;

}
static uint32_t Read(int road)
{
	uint32_t buf[4];
	uint32_t dat = 0;
	buf[0] = ADC1->CH[ adc_road[road] ].DATA;
	buf[1] = ADC1->CH[ adc_road[road] ].DATA;
	buf[2] = ADC1->CH[ adc_road[road] ].DATA;
	buf[3] = ADC1->CH[ adc_road[road] ].DATA;
	dat = ( buf[0]+buf[1]+buf[2]+buf[3] )/4;
	
//	ADC1->CH[road].STAT = 0x01;		//清除EOC标志
	
	return dat;
}


/*----------------------------------------------------------------------------------*/
//中断服务函数


/*----------------------------------------------------------------------------------*/

drvp_adc_bus_t do_drvp_adc_bus ={
	.Init		= Init,
	.ReInit	= ReInit,
	.Open		= Open,
	.Close	= Close,
	.Start  = Start,
	.Cat_Ready = Cat_Ready,
	.Read = Read,
};


drvp_adc_bus_pt drvp_adc1_bus = &do_drvp_adc_bus;




