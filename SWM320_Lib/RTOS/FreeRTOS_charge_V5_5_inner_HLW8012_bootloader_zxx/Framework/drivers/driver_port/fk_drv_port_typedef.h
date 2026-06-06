

#define Using_Interrupt
typedef void(*IntFunc0)(void);														//
typedef void(*IntFunc1)(uint8_t Dat);								//
typedef void(*IntFunc2)(uint32_t Dat);								//


/*----------------------------------------------------------------------------------*/
/* fk_drv_gpio.c */
#define PIN_LOW                 				0x00
#define PIN_HIGH                				0x01

#define PIN_MODE_OUTPUT         				0x00
#define PIN_MODE_INPUT          				0x01
#define PIN_MODE_INPUT_PULLUP   				0x02
#define PIN_MODE_INPUT_PULLDOWN 				0x03
#define PIN_MODE_OUTPUT_OD      				0x04

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01

#define PIN_IRQ_PIN_NONE                -1

typedef void (*pin_callback_t)(void *args);


typedef struct{
	int (*read)( int pin);
	void (*write)( int pin, int value);
	void (*set_mode)( int pin, int mode);
	int (*attach_irq)( int pin,int mode,pin_callback_t cb,void *args);
	int (*detach_irq)( int pin );
	int (*irq_enable)( int pin,int enabled);
}drvp_gpio_t,*drvp_gpio_pt;



/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
/* fk_drv_uart0.c */
typedef struct{
	Using_Interrupt
	void (*Init)(uint32_t baudrate);				/*初始化*/
	void (*Open)(void);											/*开启*/
	void (*Close)(void);										/*关闭*/
	void (*Write)(uint8_t Dat);				/*发送一个字节数据*/
	void (*SetIntRxFunc)(IntFunc1 func);		/*设置中断调用*/
	void (*EnableIRQ)( void );							/*开中断*/
	void (*DisableIRQ)( void );							/*关中断*/
}drv_uart_port_t,*drv_uart_port_pt;
/*----------------------------------------------------------------------------------*/
/* fk_drv_btim0.c */
typedef struct{
	Using_Interrupt
	void (*Init)(void);											/*初始化*/
	void (*Run)(void);											/*启动定时器*/
	void (*Stop)(void);											/*停止定时器*/
	void (*SetIntFunc)(IntFunc0 func);			/*设置中断调用*/
}fk_drv_btim0_t,*fk_drv_btim0_Pt;


/*----------------------------------------------------------------------------------*/
/* fk_drv_spi0.c */

typedef struct{
	void (*Init)(void);															/*初始化*/
	void (*Open)(void);															/*开启*/
	void (*Close)(void);														/*关闭*/
	unsigned char (*Swap)(uint8_t Dat);							/*交换一个字节数据*/
	void (*Div)(uint8_t div);												/*设置外设速度*/
}fk_drv_spi0_t,*fk_drv_spi0_Pt;

extern fk_drv_spi0_t DrvSpi0;
/*----------------------------------------------------------------------------------*/
/* fk_drv_adc0.c */

typedef struct{
	void (*Init)(void);															/*初始化*/
	void (*ReInit)(void *Info);											/*重新初始化*/
	void (*Open)(void);															/*开启*/
	void (*Close)(void);														/*关闭*/
	void (*Start)(void);														/*开始转换*/
	int (*Cat_Ready)(int road);													/*查看是否准备*/
	uint32_t (*Read)(int road);											/*读取数据*/
}drvp_adc_bus_t,*drvp_adc_bus_pt;

/*----------------------------------------------------------------------------------*/
//hc165

typedef struct{
	void (*init)(void);
	void (*enable)(void);
	void (*disable)(void);
	uint8_t (*read_byte)(void);
	void (*update_bus)(void);
	
}drvp_hc165_t,*drvp_hc165_pt;
/*----------------------------------------------------------------------------------*/

//74HC595族
typedef struct{
	
	void (*init)(void);
	void (*enable)(void);
	void (*disable)(void);
	void (*set_bit)(int Port);
	void (*clr_bit)(int Port);
	uint8_t (*read_bit)(int Port);
	//以下为扩展接口,用于数码管等静态刷新器件	
	void (*set_bit_but_no_out)(int Port);
	void (*clr_bit_but_no_out)(int Port);
	void (*update_out_from_drv)(void);
	void (*update_out_from_usr)(uint8_t* buf,uint8_t len);
	void (*update_bus)(void);
	//	bool (*read_bit)(int Port);
}drvp_hc595_t,*drvp_hc595_pt;

/*----------------------------------------------------------------------------------*/
#define PWM_0A	0
#define PWM_1A	1
#define PWM_0B	2
#define PWM_1B	3

#define PWM_2A	4
#define PWM_3A	5
#define PWM_2B	6
#define PWM_3B	7

#define PWM_4A	8
#define PWM_5A	9
#define PWM_4B	10
#define PWM_5B	11

#define PWM_MAX 12

typedef void (*pwm_isr)(void);
typedef struct{
	void (*init)( void );																				//初始化模块
	void (*conf_gpio)( uint32_t road );													//初始化GPIO
	void (*conf_pwm_cycle)( uint32_t road, uint32_t tick );			//设置PWM周期滴答数
	void (*conf_pwm_high)( uint32_t road,uint32_t tick );				//设置PWM高电平持续滴答数
	void (*start)( uint32_t road );															//开启PWM输出
	void (*stop)( uint32_t road );															//关闭PWM输出
	void (*set_isr)( uint32_t road,pwm_isr func );							//配置中断调用
}drvp_pwm_t,*drvp_pwm_pt;



//drv_port_io_bus
//
typedef struct{
	void (*init)( void );
	void (*load)( void );
	void (*update)( void );
	uint8_t (*swap)( uint8_t dat );
	
}drvp_io_bus_t,*drvp_io_bus_pt;

typedef void (*pin_out)(void);



//drv_port_hc595_bus
//typedef struct{
//	void (*init)(void);		//初始化
//	opin_pt CS;						//片选引脚
//	opin_pt UD;						//更新总线引脚
//	opin_pt CLK;					//时钟引脚
//	opin_pt DAT;					//数据引脚
//}drvp_hc595_pin_t,*drvp_hc595_pin_pt;

//drv_port_hc595_bus
typedef struct{
	void (*init)(void);		//初始化
	pin_out CS_H;						
	pin_out CS_L;						
	pin_out UD_H;						
	pin_out UD_L;		
	pin_out CLK_H;						
	pin_out CLK_L;						
	pin_out DAT_H;						
	pin_out DAT_L;						
	
}drvp_hc595_port_t,*drvp_hc595_port_pt;

//----------------------------------------

typedef struct{
	void (*init)( uint8_t type );
	pin_out H;				//输出高电平
	pin_out L;				//输出低电平
	int (*get)(void);	//获取输入状态
	void* (*opt)( uint8_t action,void* dat );
	
}drvp_iopin_t,*drvp_iopin_pt;


//----------------------------------------
typedef struct{
	void (*init)(void);
	uint8_t (*start)( uint8_t addr);
	void (*stop)(void);
	uint8_t (*write)( uint8_t data);
	uint8_t (*read)( uint8_t ack);
	
}drvp_iic_t,*drvp_iic_pt;


typedef struct{
	void (*init)(void);	
	void (*Start)(void);
	void (*Stop)(void);
	void (*Ack)(void);
	void (*NoAck)(void);
	uint8_t (*GetACK)(void);
	uint8_t (*SendByte)(uint8_t Data);  
	uint8_t (*RecByte)(void);
}drvp_soft_iic_t,*drvp_soft_iic_pt;
//----------------------------------------

typedef struct{
	void (*init)(void);

}drvp_adc_t,*drvp_adc_pt;


typedef struct{
	void (*init)( RTC_DateTime* DT,int mINT,int sINT );
	void (*start)( void );
	void (*stop)( void );
	void (*read)( RTC_DateTime* DT );
	
}drvp_rtc_t,*drvp_rtc_pt;

//----------------------------------------




