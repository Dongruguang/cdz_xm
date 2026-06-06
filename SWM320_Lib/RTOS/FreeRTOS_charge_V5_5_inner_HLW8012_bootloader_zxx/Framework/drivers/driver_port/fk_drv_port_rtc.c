#include "framework.h"




rtc_dt_t rtc_dt;

static void init( RTC_DateTime* DT,int mINT,int sINT )
{
	RTC_InitStructure RTC_initStruct;
	
	RTC_initStruct.Year = DT->Year;
	RTC_initStruct.Month = DT->Month;
	RTC_initStruct.Date = DT->Date;
	RTC_initStruct.Hour = DT->Hour;
	RTC_initStruct.Minute = DT->Minute;
	RTC_initStruct.Second = DT->Second;
	
	RTC_initStruct.SecondIEn = sINT;
	RTC_initStruct.MinuteIEn = mINT;
	
	
	
	RTC_Init(RTC, &RTC_initStruct);
	
	
}

static void start( void )
{
	RTC_Start(RTC);
}


static void stop( void )
{
	RTC_Stop(RTC);
}

static void read( RTC_DateTime* DT )
{
	RTC_GetDateTime(RTC, DT);
}


void RTC_Handler(void)
{
//	RTC_DateTime dateTime;
	
	if(RTC_IntSecondStat(RTC))
	{
		RTC_IntSecondClr(RTC);
		
		RTC_GetDateTime(RTC, &rtc_dt);
		//printf("Now Time: %02d : %02d\r\n", dateTime.Minute, dateTime.Second);
	}
	
	if(RTC_IntMinuteStat(RTC))
	{
		RTC_IntMinuteClr(RTC);
		
		RTC_GetDateTime(RTC, &rtc_dt);
	}
	
	
}






static drvp_rtc_t do_drvp_rtc = {
	init,
	start,
	stop,
	read,
};

drvp_rtc_pt drvp_rtc = &do_drvp_rtc;



