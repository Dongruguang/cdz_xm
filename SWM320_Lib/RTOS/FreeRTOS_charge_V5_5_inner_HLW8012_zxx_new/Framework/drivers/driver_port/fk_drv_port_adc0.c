
/*----------------------------------------------------------------------------------*/

#include "framework.h"
#if 0
/*----------------------------------------------------------------------------------*/
static void fk_drv_adc0_Init(void);															/*初始化*/
static void fk_drv_adc0_Open(void);															/*开启*/
static void fk_drv_adc0_Close(void);														/*关闭*/
static void fk_drv_adc0_Tigger(void);														/*触发一个转换通道*/
static void fk_drv_adc0_SetIntRxFunc(int Ch,IntFunc2 func);			/*设置中断调用*/
static void* fk_drv_adc0_GetConfigInfo(void);										/*获取配置信息*/
static void fk_drv_adc0_ReInit(void *Info);											/*重新初始化*/

/*----------------------------------------------------------------------------------*/

fk_drv_adc0_t DrvAdc0 ={
	.Init		= fk_drv_adc0_Init,
	.ReInit	= fk_drv_adc0_ReInit,
	.Open		= fk_drv_adc0_Open,
	.Close	= fk_drv_adc0_Close,
	.Tigger = fk_drv_adc0_Tigger,
	.SetIntRxFunc 	= fk_drv_adc0_SetIntRxFunc,
	.GetConfigInfo 	= fk_drv_adc0_GetConfigInfo,
};
ADC_InitStructure ADC_initStruct;
/*中断调用*/
static void IntError_0(unsigned int Dat){printf("rec = %c\r\n",Dat);}
static IntFunc2 thisDrvIntFunc[ADC0_UseCh]={IntError_0};


/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_Init(void)
{
	
	/*初始化具体的引脚为ADC输入引脚*/
	PORT_Init(PORTE, PIN4,  PORTE_PIN4_ADC0_IN0,  0);		//PE.4  => ADC0.CH0
//	PORT_Init(PORTA, PIN15, PORTA_PIN15_ADC0_IN1, 0);		//PA.15 => ADC0.CH1
//	PORT_Init(PORTA, PIN14, PORTA_PIN14_ADC0_IN2, 0);		//PA.14 => ADC0.CH2
//	PORT_Init(PORTA, PIN13, PORTA_PIN13_ADC0_IN3, 0);		//PA.13 => ADC0.CH3
//	PORT_Init(PORTA, PIN12, PORTA_PIN12_ADC0_IN4, 0);		//PA.12 => ADC0.CH4
//	PORT_Init(PORTC, PIN7,  PORTC_PIN7_ADC0_IN5,  0);		//PC.7  => ADC0.CH5
//	PORT_Init(PORTC, PIN6,  PORTC_PIN6_ADC0_IN6,  0);		//PC.6  => ADC0.CH6
	
	/*设置ADC0外设时钟*/
	ADC_initStruct.clk_src 	= ADC_CLKSRC_HRC_DIV8;
	
	/*接通哪些通道?可使用 按位与 同时使能多个通道 */
	ADC_initStruct.channels = ADC_CH0;				//|ADC_CH1|ADC_CH2|ADC_CH3
	
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;	//转换次数
	ADC_initStruct.trig_src = ADC_TRIGGER_SW;		//软件触发
	ADC_initStruct.Continue = 0;								//非连续模式，即单次模式
	ADC_initStruct.EOC_IEn 	= ADC_CH0;					//转换结束中断	|ADC_CH1|ADC_CH2|ADC_CH3
	ADC_initStruct.OVF_IEn 	= 0;								//溢出中断
	ADC_Init(ADC0, &ADC_initStruct);						//配置ADC
}
/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_Open(void)		{ ADC_Open(ADC0);}
/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_Close(void)	{ ADC_Close(ADC0);}
/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_Tigger(void)	{ ADC_Start(ADC0);}
/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_SetIntRxFunc(int Ch,IntFunc2 func)
{
	thisDrvIntFunc[Ch] = func;
}
/*----------------------------------------------------------------------------------*/
static	void* fk_drv_adc0_GetConfigInfo(void)
{
	ADC_InitStructure* iInfo = &ADC_initStruct;
	return (void*)iInfo;
}

/*----------------------------------------------------------------------------------*/

static void fk_drv_adc0_ReInit(void *Info)
{
	ADC_Init(ADC0, Info);						//配置ADC
}

/*----------------------------------------------------------------------------------*/
//中断服务函数
void GPIOC4_ADC0_Handler(void)
{
	unsigned int Val = 0;
	/*先看看是什么通道触发了中断*/				
	if( ADC_IntEOCStat(ADC0,ADC_CH0) )		
	{
		Val = ADC_Read(ADC0, ADC_CH0);			//读出值
		//printf("Val = %d \r\n",Val);
		thisDrvIntFunc[0](Val);							//数据处理函数
		ADC_IntEOCClr(ADC0,ADC_CH0);				//清中断
	}
	
	//ADC_Start(ADC0);
}

/*----------------------------------------------------------------------------------*/
#endif








