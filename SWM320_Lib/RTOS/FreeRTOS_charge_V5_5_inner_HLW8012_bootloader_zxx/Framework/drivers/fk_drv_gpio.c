#include "framework.h"


static int read( int pin)
{
	return drvp_gpio->read( pin );
}
static void write( int pin, int value)
{
	drvp_gpio->write( pin,value );
}
static void set_mode( int pin, int mode)
{
	drvp_gpio->set_mode( pin,mode );
}
static int attach_irq( int pin,int mode,pin_callback_t cb,void *args)
{
	drvp_gpio->attach_irq( pin,mode,cb,args );
	return 0;
}
static int detach_irq( int pin )
{
	drvp_gpio->detach_irq( pin );
	return 0;
	
}
static int irq_enable( int pin,int enabled)
{
	drvp_gpio->irq_enable( pin,enabled );
	return 0;
	
}


static drv_gpio_t m_drv_gpio={
	read,
	write,
	set_mode,
	attach_irq,
	detach_irq,
	irq_enable
};

drv_gpio_pt gpio = &m_drv_gpio;









