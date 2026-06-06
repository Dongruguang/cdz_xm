#include "framework.h"

/*
		
*/
#define MAX_PWM		12

static PWM_InitStructure  PWM_initStruct[MAX_PWM/2];
static void conf_gpio( uint32_t road );

static void init( void )
{
	uint8_t index = 0;
	for( index=0;index<12;index++ )
	{
		conf_gpio( index );
	}
	for( index=0;index<(MAX_PWM/2);index++ )
	{
		PWM_initStruct[index].clk_div = PWM_CLKDIV_8;		//F_PWM = 100/8 = 12.5M
		PWM_initStruct[index].mode = PWM_MODE_INDEP;		//A路和B路独立输出					
		PWM_initStruct[index].cycleA =  12500;					//12 500K/12.5K = 1kHz			
		PWM_initStruct[index].hdutyA =   12500/4;					// 50%
		PWM_initStruct[index].initLevelA = 1;						//默认高电平	
		PWM_initStruct[index].cycleB = 	12500;
		PWM_initStruct[index].hdutyB =  12500/4;						
		PWM_initStruct[index].initLevelB = 1;
		
		PWM_initStruct[index].HEndAIEn = 0;
		PWM_initStruct[index].NCycleAIEn = 0;
		PWM_initStruct[index].HEndBIEn = 0;
		PWM_initStruct[index].NCycleBIEn = 0;
		
		
	}
	PWM_Init(PWM0, &PWM_initStruct[0]);
	PWM_Init(PWM1, &PWM_initStruct[1]);
	PWM_Init(PWM2, &PWM_initStruct[2]);
	PWM_Init(PWM3, &PWM_initStruct[3]);
	PWM_Init(PWM4, &PWM_initStruct[4]);
	PWM_Init(PWM5, &PWM_initStruct[5]);
	
	

}
//初始化GPIO
static void conf_gpio( uint32_t road )
{
	switch( road )
	{
		case  0:PORT_Init(PORTP, PIN2 , FUNMUX0_PWM0A_OUT, 0);break;			
		case  1:PORT_Init(PORTP, PIN3 , FUNMUX1_PWM1A_OUT, 0);break;			
		case  2:PORT_Init(PORTP, PIN4 , FUNMUX0_PWM0B_OUT, 0);break;			
		case  3:PORT_Init(PORTP, PIN5 , FUNMUX1_PWM1B_OUT, 0);break;
		
		case  4:PORT_Init(PORTP, PIN6 , FUNMUX0_PWM2A_OUT, 0);break;			
		case  5:PORT_Init(PORTP, PIN7 , FUNMUX1_PWM3A_OUT, 0);break;			
		case  6:PORT_Init(PORTP, PIN8 , FUNMUX0_PWM2B_OUT, 0);break;			
		case  7:PORT_Init(PORTP, PIN9 , FUNMUX1_PWM3B_OUT, 0);break;
		
		case  8:PORT_Init(PORTP, PIN10, FUNMUX0_PWM4A_OUT, 0);break;			
		case  9:PORT_Init(PORTP, PIN11, FUNMUX1_PWM5A_OUT, 0);break;			
		case 10:PORT_Init(PORTP, PIN12, FUNMUX0_PWM4B_OUT, 0);break;			
		case 11:PORT_Init(PORTP, PIN13, FUNMUX1_PWM5B_OUT, 0);break;			
		default:
			break;
	}
}

static void conf_pwm_cycle( uint32_t road, uint32_t tick )
{
	switch( road )
	{
		case  0:PWM_SetCycle( PWM0, PWM_CH_A, tick );break;
		case  1:PWM_SetCycle( PWM1, PWM_CH_A, tick );break;
		case  2:PWM_SetCycle( PWM0, PWM_CH_B, tick );break;
		case  3:PWM_SetCycle( PWM1, PWM_CH_B, tick );break;
		
		case  4:PWM_SetCycle( PWM2, PWM_CH_A, tick );break;
		case  5:PWM_SetCycle( PWM3, PWM_CH_A, tick );break;
		case  6:PWM_SetCycle( PWM2, PWM_CH_B, tick );break;
		case  7:PWM_SetCycle( PWM3, PWM_CH_B, tick );break;
		
		case  8:PWM_SetCycle( PWM4, PWM_CH_A, tick );break;
		case  9:PWM_SetCycle( PWM5, PWM_CH_A, tick );break;
		case 10:PWM_SetCycle( PWM4, PWM_CH_B, tick );break;
		case 11:PWM_SetCycle( PWM5, PWM_CH_B, tick );break;

		default: break;
	}
}

static void conf_pwm_high( uint32_t road, uint32_t tick )
{
	switch( road )
	{
		case  0:PWM_SetHDuty( PWM0, PWM_CH_A, tick );break;
		case  1:PWM_SetHDuty( PWM1, PWM_CH_A, tick );break;
		case  2:PWM_SetHDuty( PWM0, PWM_CH_B, tick );break;
		case  3:PWM_SetHDuty( PWM1, PWM_CH_B, tick );break;
		
		case  4:PWM_SetHDuty( PWM2, PWM_CH_A, tick );break;
		case  5:PWM_SetHDuty( PWM3, PWM_CH_A, tick );break;
		case  6:PWM_SetHDuty( PWM2, PWM_CH_B, tick );break;
		case  7:PWM_SetHDuty( PWM3, PWM_CH_B, tick );break;
		
		case  8:PWM_SetHDuty( PWM4, PWM_CH_A, tick );break;
		case  9:PWM_SetHDuty( PWM5, PWM_CH_A, tick );break;
		case 10:PWM_SetHDuty( PWM4, PWM_CH_B, tick );break;
		case 11:PWM_SetHDuty( PWM5, PWM_CH_B, tick );break;
		
		default:break;
	
	}
}

static void start( uint32_t road )
{
	switch( road )
	{
		case  0: PWMG->CHEN |= 1 << PWMG_CHEN_PWM0A_Pos; break;
		case  1: PWMG->CHEN |= 1 << PWMG_CHEN_PWM1A_Pos; break;
		case  2: PWMG->CHEN |= 1 << PWMG_CHEN_PWM0B_Pos; break;
		case  3: PWMG->CHEN |= 1 << PWMG_CHEN_PWM1B_Pos; break;
		
		case  4: PWMG->CHEN |= 1 << PWMG_CHEN_PWM2A_Pos; break;
		case  5: PWMG->CHEN |= 1 << PWMG_CHEN_PWM3A_Pos; break;
		case  6: PWMG->CHEN |= 1 << PWMG_CHEN_PWM2B_Pos; break;
		case  7: PWMG->CHEN |= 1 << PWMG_CHEN_PWM3B_Pos; break;
		
		case  8: PWMG->CHEN |= 1 << PWMG_CHEN_PWM4A_Pos; break;
		case  9: PWMG->CHEN |= 1 << PWMG_CHEN_PWM5A_Pos; break;
		case 10: PWMG->CHEN |= 1 << PWMG_CHEN_PWM4B_Pos; break;
		case 11: PWMG->CHEN |= 1 << PWMG_CHEN_PWM5B_Pos; break;
		
		
		default: break;
	}
}	
static void stop( uint32_t road )
{
	switch( road )
	{
		case  0: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM0A_Pos);break;
		case  1: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM1A_Pos);break;
		case  2: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM0B_Pos);break;
		case  3: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM1B_Pos);break;
		
		case  4: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM2A_Pos);break;
		case  5: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM3A_Pos);break;
		case  6: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM2B_Pos);break;
		case  7: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM3B_Pos);break;
		
		case  8: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM4A_Pos);break;
		case  9: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM5A_Pos);break;
		case 10: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM4B_Pos);break;
		case 11: PWMG->CHEN &= ~( 1 << PWMG_CHEN_PWM5B_Pos);break;
		
		
		default: break;
	}
}


static pwm_isr do_pwm_isr[ MAX_PWM ];

static void set_isr(uint32_t road,pwm_isr func)
{
	if( road >= MAX_PWM )
	{
		DRV_LogOUT("%s:set_isr ,road[%d] > MAX_PWM\r\n",__FILE__,road);
		return;
	}
	
}
/*----------------------------------------------------------------------------------*/

drvp_pwm_t m_drvp_pwm={
	.init = init,
	.conf_gpio = conf_gpio,
	.conf_pwm_cycle = conf_pwm_cycle,
	.conf_pwm_high  = conf_pwm_high,
	.start = start,
	.stop = stop,
	.set_isr = set_isr,
};

drvp_pwm_pt drvp_pwm = &m_drvp_pwm;

/*----------------------------------------------------------------------------------*/

#define OnePWM_Handler( PWMx,CHx,INTx ) \
{ \
	if( PWM_IntNCycleStat(PWMx, CHx) ) \
	{ \
		PWM_IntNCycleClr(PWMx, CHx); \
		do_pwm_isr[ INTx ](); \
	} \
}

void PWM_Handler( void )
{
	
	OnePWM_Handler( PWM0,PWM_CH_A,PWM_0A );
	OnePWM_Handler( PWM1,PWM_CH_A,PWM_1A );
	OnePWM_Handler( PWM0,PWM_CH_B,PWM_0B );
	OnePWM_Handler( PWM1,PWM_CH_B,PWM_1B );
	
	OnePWM_Handler( PWM2,PWM_CH_A,PWM_2A );
	OnePWM_Handler( PWM3,PWM_CH_A,PWM_3A );
	OnePWM_Handler( PWM2,PWM_CH_B,PWM_2B );
	OnePWM_Handler( PWM3,PWM_CH_B,PWM_3B );
	
	OnePWM_Handler( PWM4,PWM_CH_A,PWM_4A );
	OnePWM_Handler( PWM5,PWM_CH_A,PWM_5A );
	OnePWM_Handler( PWM4,PWM_CH_B,PWM_4B );
	OnePWM_Handler( PWM5,PWM_CH_B,PWM_5B );
	
}







