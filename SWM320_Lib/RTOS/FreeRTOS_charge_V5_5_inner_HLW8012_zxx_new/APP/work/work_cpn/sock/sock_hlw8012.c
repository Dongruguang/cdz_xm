
#include "work.h"
#define PRT_LOG(X...)	sysprt->dlog(X)	

/*----------------------------------------------------------------------------------*/


volatile static int HlwPinMap[MaxHLW] ={ HL_TX1,HL_TX0 };
/*----------------------------------------------------------------------------------*/

static uint32_t total_pul[MaxHLW] = {0,0};



static int init_flag  = 0;


/*----------------------------------------------------------------------------------*/
static thread_mq_t event_mq[MaxHLW];

static void mq_init(void)
{
	for( int road=0;road<MaxHLW;road++ )
	{	
		event_mq[ road ] = thread_mq_init("event_mq", sizeof(hlw_pul_t), 5);
	}
}
static void put( int road,int type,uint32_t cyc,uint32_t pul  )
{
	int ret = 0;
	hlw_pul_t dat;
	if( init_flag == 0 ) return;
	if( road >=MaxHLW ) return;
	dat.type = type;
	dat.cyc = cyc;
	dat.val = pul;
	
	ret = thread_mq_send( event_mq[road],&dat, sizeof(hlw_pul_t), 0x01 );
	if( ret == bFALSE )
	{
//		sysprt->aerr("over: %s\r\n",__FUNCTION__);
	}
	else
	{
//		sysprt->aerr("put[%d]: %d\r\n",road,pul);
	}
}

static int get( int road,void* data )
{
	if( init_flag == 0 ) return -1;
	if( road >=MaxHLW ) return -1;
	
	hlw_pul_t* dat = (hlw_pul_t*)data;
	
	
	int ret = thread_mq_recv( event_mq[road], dat, sizeof(hlw_pul_t), 0);
	if(ret != bFALSE) 
		return 0;
	else
		return -1;
	
}

/*----------------------------------------------------------------------------------*/

static uint32_t last_tick[MaxHLW] = {0,0};					//描述上一个脉冲的时间，用于判断是否拔掉插座

static uint32_t sock_cntr[MaxHLW] = {0,0};					//本次周期采样的脉冲数


typedef struct{
	uint8_t num;				//编码器序号
	uint8_t pin;				//gpio 引脚号
	uint8_t enable;			//描述编码器是否使能
	uint8_t flag;			//用于使用滤波器的情况下，描述是否触发了中断
	
//	uint32_t val;				//描述编译器计数值	
//	uint32_t dtick;			//描述记录编码器使用软件滤波时，滤波周期
//	uint32_t filter;			//描述编码器是否软件滤波
//	uint32_t otick;			//用于滤波器记录编码器触发时间
}dat_t;

static dat_t cdat[MaxSock];

static void do_isr(void *args)
{
	dat_t* pdat = (dat_t*)args;
	

	if( pdat->num >= MaxSock) return;
	
//	if( pdat->enable == __Disable ) return;
	
	

	total_pul[pdat->num]++;
	sock_cntr[pdat->num]++;
	last_tick[pdat->num] = get_sys_ticks();
		

	
}

static void init( void )
{
//	uint8_t pin = 0;
	if( init_flag == 1 ) return;
	
	
	
	for( int index=0;index<MaxSock;index++)
	{
		cdat[index].num = index;
		cdat[index].pin = HlwPinMap[index];
		cdat[index].enable = __Enable;
		cdat[index].flag = 0;
#if ( BoardVision == BoardV5_4)
		gpio->set_mode( HlwPinMap[index], PIN_MODE_INPUT_PULLUP );
		gpio->attach_irq( HlwPinMap[index],PIN_IRQ_MODE_FALLING,do_isr,&(cdat[index]) );
		gpio->irq_enable( HlwPinMap[index], PIN_IRQ_ENABLE );
#endif
		
#if ( BoardVision == BoardV5_5)
		gpio->set_mode( HlwPinMap[index], PIN_MODE_INPUT );
		gpio->attach_irq( HlwPinMap[index],PIN_IRQ_MODE_FALLING,do_isr,&(cdat[index]) );
		gpio->irq_enable( HlwPinMap[index], PIN_IRQ_ENABLE );
		
		
#endif

		
		
	}
	

	
	
	//创建队列，用于定时传输周期脉冲给业务逻辑程序
	
	mq_init();
	
	
	init_flag = 1;
}


/*----------------------------------------------------------------------------------*/

#if ( BoardVision == BoardV5_5)

void listen_sock_pul(int sock)
{
	static uint32_t last_sta[2] = {1,1};

	int val = gpio->read( HlwPinMap[sock]);
	
	if( val == last_sta[sock] ) return;
	
	//执行到此处,说明增加一个脉冲
	total_pul[sock]++;
	sock_cntr[sock]++;
	last_tick[sock] = get_sys_ticks();
	
	
	
	last_sta[sock] = val;
	
	
}
#endif

#define SockOutCyc  3
#define SockPwrCyc	6
static void listen_work( void )
{
	static uint32_t otick_sock = 0;							//描述起始时间，用于计算平均值
	static uint32_t min_cntr = 0;								//计数器,用于定时1分钟发送一次电能消耗
	static uint32_t min_pul[MaxHLW] ={0,0};								//描述一分钟电能消耗对应脉冲值
	time_t ntick = 0;
	
	ntick = get_sys_ticks();
	
	if( 0 == init_flag ) return;
	
	
	for( int road = 0;road<2;road++ )
	{
#if ( BoardVision == BoardV5_5)
		
//		listen_sock_pul( road );
#endif		
		
		if( (ntick - last_tick[road]) > (SockOutCyc*1000) )
		{//成立，说明插座被拔掉了
			last_tick[road] = ntick;
//			sysprt->alog("%s sock[%d] out\r\n ",road?"                        ":" ",road );
			//发送拔插座事件
			//TODO:
			put( road,_type_hlw_sock_out,SockOutCyc,0 );
			
		}
		
		
	}
	
	if( (ntick - otick_sock)>= ( SockPwrCyc * 1000 ) )
	{
		otick_sock = ntick;
//		sysprt->alog("[%8d] sock[0] = %6d \t sock[1] = %6d \r\n",ntick,sock_cntr[0],sock_cntr[1]);
		 

		
		//将脉冲变化以队列形式通知app
		for( int road =0;road<MaxHLW;road++ )
		{

			if( sock_cntr[road] < 1000 )//1100W ->220个边沿 /2 = 110真实脉冲, 6s，大概 660 个真实脉冲，写1000吧
			{
				put( road,_type_hlw_cyc_pul,SockPwrCyc,sock_cntr[road]  );
				min_pul[road] += sock_cntr[road];
			}
			else
			{
				put( road,_type_hlw_cyc_pul,SockPwrCyc,660  );
				min_pul[road] += 660;
			}				
			
		}
		
		//一分钟执行一次,告诉app这一分钟消耗了多少电能
		min_cntr++;
		if( min_cntr>=10 )
		{
			min_cntr = 0;
			//
			for( int road =0;road<MaxHLW;road++ )
			{
				
				put( road,_type_hlw_min_pul,60,min_pul[road]   );		
				
				min_pul[road] = 0;
			}
			
		}
		
		
		sock_cntr[0] = 0;
		sock_cntr[1] = 0;
		
	}
	
	

}





/*----------------------------------------------------------------------------------*/





/*----------------------------------------------------------------------------------*/








static sock_hlw_opt_t do_sock_hlw_opt = {
	init,
	get,
	listen_work
};
/*---------------------------------------------------------------*/

sock_hlw_opt_pt sock_hlw_opt = &do_sock_hlw_opt;


