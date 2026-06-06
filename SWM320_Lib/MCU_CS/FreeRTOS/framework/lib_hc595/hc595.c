#include "framework.h"



#define wide_rck 50
#define wide_sck 20

static void init(hc595_pin_t* psc)
{
	gpio->set_mode(psc->dat,PIN_MODE_OUTPUT);
	gpio->set_mode(psc->clk,PIN_MODE_OUTPUT);
	gpio->set_mode(psc->ud,PIN_MODE_OUTPUT);
	gpio->set_mode(psc->cs,PIN_MODE_OUTPUT);
	
}

static void hc595_write_b(hc595_pin_t* psc,uint8_t data)
{
	volatile uint32_t dealy = 0;
	
	for( int i=0;i<8;i++ )
	{
		//根据数据进行输出
		if( data & 0x80 )
			gpio->write( psc->dat,PIN_HIGH );
		else
			gpio->write( psc->dat,PIN_LOW  );
		
		data<<=1;
		
		//产生上升沿,移位寄存器会读出1bit数据，并推出1bit数据
		gpio->write( psc->clk,PIN_LOW  );
		dealy = wide_sck; while( dealy-- );
		gpio->write( psc->clk,PIN_HIGH );
		dealy = wide_sck; while( dealy-- );	
	}
}

static void hc595_write(hc595_pin_t* psc,uint8_t* data,int len)
{
	for(int i=0;i<len;i++)
	{
		hc595_write_b(psc,data[i]);
	}
	
}



static void hc595_updata(hc595_pin_t* psc)
{
		volatile uint32_t dealy = 0;
		gpio->write( psc->ud,PIN_LOW  );
		dealy = wide_sck; while( dealy-- );
		gpio->write( psc->ud,PIN_HIGH );
		dealy = wide_sck; while( dealy-- );
}


static hc595_opt_t do_hc595_opt={
	.init = init,
	.write = hc595_write,
	.updata = hc595_updata,
};

hc595_opt_t* hc595_opt = &do_hc595_opt;









