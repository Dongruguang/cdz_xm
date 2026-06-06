
#include "framework.h"

#define PRT_LOG(X...)	{ sysprt->dlog(X); }

//coder 依赖于gpio驱动


#define MaxCoder	2
//static uint32_t val[ MaxCoder ]={0,0};

typedef struct{
	uint8_t num;				//编码器序号
	uint8_t pin;				//gpio 引脚号
	uint8_t enable;			//描述编码器是否使能
	uint8_t filter;			//描述编码器是否软件滤波
	
	
	uint32_t val;				//描述编译器计数值	
	uint32_t dtick;			//描述记录编码器使用软件滤波时，滤波周期
	uint32_t flag;			//用于使用滤波器的情况下，描述是否触发了中断
	uint32_t otick;			//用于滤波器记录编码器触发时间
}dat_t;


static dat_t cdat[MaxCoder];



static void init( void )
{
	for( int i=0;i<MaxCoder;i++ )
	{
		cdat[i].num = i;
		cdat[i].enable = __Disable;
		cdat[i].filter = __Disable;
		cdat[i].val = 0;
		cdat[i].dtick = 5;
		cdat[i].flag = 0;
		cdat[i].otick = 0;
		
	}
		
	
}

static void set_pin(int road,int pin)
{
	if( road>=MaxCoder ) return;
	cdat[ road ].pin = pin;
}


static void do_isr(void *args)
{
	dat_t* cdat = (dat_t*)args;
	

	if( cdat->num >= MaxCoder) return;
	
	if( cdat->enable == __Disable ) return;
	
	
	if( cdat->filter == __Disable )
	{//没有使用滤波器，直接执行
		cdat->val++;
//		PRT_LOG("do_isr[%d] = %ld \r\n",cdat->num,cdat->val );
	}
	else
	{
		cdat->flag = 1;
		cdat->otick = get_sys_ticks();
	}
	
}


static void config( int road,int filter,uint32_t freq )
{
	if( road >= MaxCoder) return;
	cdat[road].filter = filter;
	
	if( filter == __Disable ) cdat[road].filter = __Disable;
	else											cdat[road].filter = __Enable;
	
	if( freq <= 1000 )
	{
		cdat[road].dtick = 1000/freq;
		cdat[road].flag = 0;
	}
	else
	{//不支持高频，关闭滤波器
		filter = __Disable;
		cdat[road].dtick = 0;
	}
	
	
	gpio->set_mode  ( cdat[road].pin,PIN_MODE_INPUT_PULLUP );	
	gpio->attach_irq( cdat[road].pin,PIN_IRQ_MODE_FALLING,do_isr,&(cdat[road]) );
	gpio->irq_enable( cdat[road].pin, PIN_IRQ_ENABLE );
	
	
}


static void start( int road )
{
	 cdat[road].enable = __Enable;
}
static void stop( int road )
{
	cdat[road].enable = __Disable;
}
static void clr_val( int road )
{	
	cdat[road].val = 0;
	
}
static uint32_t cat_val( int road )
{
	return cdat[road].val;
}

static int cat_pin( int road )
{
	return gpio->read( cdat[road].pin );
}

static void work(void)
{
	time_t tick = 0;
	for( int i=0;i<MaxCoder;i++ )
	{
		//如果没有使能,  结束本轮循环
		if( cdat[i].enable == __Disable ) continue;
		
		//如果没有使用滤波器, 结束本轮循环
		if( cdat[i].filter == __Disable ) continue;
	
		if( cdat[i].flag == 0) continue;
		
		tick = get_sys_ticks();
		//未达到滤波时间, 结束本轮循环
		if( (tick - cdat[i].otick ) < cdat[i].dtick ) continue;
		
		cdat[i].val ++;
		cdat[i].flag = 0;
//		PRT_LOG("work[%d] = %ld \r\n",cdat->num,cdat->val );
		
	}
}


static drv_coder_t drv_coder={
	init,
	set_pin,
	config,

	start,
	stop,
	clr_val,
	
	cat_val,	
	cat_pin,
	work,
};


drv_coder_pt coder = &drv_coder;


/*用法
	//初始化模块
	coder->init();
	coder->config( 5 );
	
	//开始计数编码
	coder->clr_val(5);
	coder->start( 5 );

	//查看编码数
	uint32_t val = coder->cat_val();

*/
