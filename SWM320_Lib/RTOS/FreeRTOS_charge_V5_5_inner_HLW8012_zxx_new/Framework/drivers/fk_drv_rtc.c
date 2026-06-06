#include "framework.h"


static void init( rtc_dt_t* dt,int mINT,int sINT )
{
	drvp_rtc->init( dt,mINT,sINT );
}


static void start( void )
{
	drvp_rtc->start();
}

static void stop( void )
{
	drvp_rtc->stop();
}

static void read( rtc_dt_t* dt )
{
	drvp_rtc->read( dt );
}

static rtc_t do_rtc = {
	init,
	start,
	stop,
	read,
};

rtc_pt rtc = &do_rtc;




