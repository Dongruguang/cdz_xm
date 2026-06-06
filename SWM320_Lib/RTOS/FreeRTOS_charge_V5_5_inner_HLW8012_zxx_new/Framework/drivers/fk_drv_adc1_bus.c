
#include "framework.h"


#define PRT_LOG(X...)	//dprintf(X)	


//list
static int enWork[ADC1_UseCh];
static uint32_t adcbuff[ADC1_UseCh];


static void init(void)
{
	int i=0;
	for( i=0;i<ADC1_UseCh;i++ )
	{
		enWork[i] = 0;
		adcbuff[i] = 0;
	}
	drvp_adc1_bus->Init();
	drvp_adc1_bus->Open();
	drvp_adc1_bus->Start();
}

static void enable(uint8_t road)
{
	enWork[road] = __ON;
}
static void disable(uint8_t road)
{
	enWork[road] = __OFF;
}
static uint32_t read(uint8_t road)
{
	return adcbuff[road];
}

static void work(void)
{
	int road=0;
	
	
	for( road=0; road<ADC1_UseCh; road++ )
	{
		if( __ON == enWork[road] )
		{
			//看看是否准备好了
			if ( drvp_adc1_bus->Cat_Ready( road ) )
			{
				//读出数据
				adcbuff[road] = drvp_adc1_bus->Read( road );
				PRT_LOG("\r\n adc[%d] = %d \r\n",road,adcbuff[road]);
			}
		}
	}
}

static adc_bus_t do_adc_bus={
	init,enable,disable,read,work
};

adc_bus_pt adc1_bus = &do_adc_bus;

