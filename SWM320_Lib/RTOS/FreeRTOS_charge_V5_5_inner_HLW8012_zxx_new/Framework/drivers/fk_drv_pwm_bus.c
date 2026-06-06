#include "framework.h"


#define PRT_LOG(X...)	sysprt->alog//dprintf(X)	

/*--------------------------------------------------------------------------------*/
//PWM_MAX

static void init( void )
{
	drvp_pwm->init();
	
}




static void set_ctick( uint32_t road, uint16_t ctick )
{
	drvp_pwm->conf_pwm_cycle( road, ctick );
	
	
}


static void set_htick( uint32_t road, uint16_t htick )
{
	drvp_pwm->conf_pwm_high( road, htick );
	
}


static void start( uint32_t road )
{
	drvp_pwm->start( road );
}

static void stop( uint32_t road )
{
	drvp_pwm->stop( road );
}

static void set_isr( uint32_t road,pwm_isr func )
{
	drvp_pwm->set_isr( road,func );
}

/*--------------------------------------------------------------------------------*/

static pwm_bus_t do_pwm_bus={
	init,set_ctick,set_htick,start,stop,set_isr
};
/*--------------------------------------------------------------------------------*/
pwm_bus_pt pwm_bus = &do_pwm_bus;
/*--------------------------------------------------------------------------------*/
