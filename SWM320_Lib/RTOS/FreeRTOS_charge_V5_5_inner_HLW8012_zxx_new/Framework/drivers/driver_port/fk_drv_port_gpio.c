#include "framework.h"
#define MaxLine 14

struct pin
{
    uint32_t package_index;		//引脚号
    const char *name;					//引脚名字
    GPIO_TypeDef *port;				//寄存器组地址
    uint32_t group_index;			//GPIO 号
    IRQn_Type irq;						//中断开关号 Cortex-M4 specific Interrupt Numbers,位于swm320.h文件定义
    uint32_t irq_mode;				//中断号
    pin_callback_t callback;	//中断回调函数
    void *callback_args;
};
typedef struct pin pin_t;

#define SWM32_PIN(a, b, c, d)             \
    {                                     \
        a, #b, GPIO##c, d, GPIO##c##_IRQn \
    }
#define GPIO0 ((GPIO_TypeDef *)(0))
#define GPIO0_IRQn (GPIOA0_IRQn)


volatile static pin_t swm32_pin_map[] =
{
	SWM32_PIN( 0, NoPin , 0, 0 ),
	SWM32_PIN( 1, 3V3   , 0, 0 ),   SWM32_PIN( 2, CAP0  , 0, 0 ),   SWM32_PIN( 3, B12   , B, 12),   SWM32_PIN( 4, 3V3   , 0, 0 ),
	SWM32_PIN( 5, 3V3   , 0, 0 ),   SWM32_PIN( 6, GND   , 0, 0 ),   SWM32_PIN( 7, CAP2  , 0, 0 ),   SWM32_PIN( 8, N9    , N, 9 ),
	SWM32_PIN( 9, N10   , N, 10),   SWM32_PIN(10, CAP1  , 0, 0 ),   SWM32_PIN(11, GND   , 0, 0 ),   SWM32_PIN(12, 3V3   , 0, 0 ),
	SWM32_PIN(13, N2    , N, 2 ),   SWM32_PIN(14, N1    , N, 1 ),   SWM32_PIN(15, N0    , N, 0 ),   SWM32_PIN(16, 3V3   , 0, 0 ),
	SWM32_PIN(17, C4    , C, 4 ),   SWM32_PIN(18, C5    , C, 5 ),   SWM32_PIN(19, C6    , C, 6 ),   SWM32_PIN(20, C7    , C, 7 ),
	SWM32_PIN(21, C2    , C, 2 ),   SWM32_PIN(22, C3    , C, 3 ),   SWM32_PIN(23, XI    , 0, 0 ),   SWM32_PIN(24, XO    , 0, 0 ),
	SWM32_PIN(25, RESET , 0, 0 ),   SWM32_PIN(26, M2    , M, 2 ),   SWM32_PIN(27, M3    , M, 3 ),   SWM32_PIN(28, M4    , M, 4 ),
	SWM32_PIN(29, M5    , M, 5 ),   SWM32_PIN(30, M6    , M, 6 ),   SWM32_PIN(31, M7    , M, 7 ),   SWM32_PIN(32, GND   , 0, 0 ),
	SWM32_PIN(33, M1    , M, 1 ),   SWM32_PIN(34, M0    , M, 0 ),   SWM32_PIN(35, P0    , P, 0 ),   SWM32_PIN(36, P1    , P, 1 ),
	SWM32_PIN(37, P2    , P, 2 ),   SWM32_PIN(38, P3    , P, 3 ),   SWM32_PIN(39, P4    , P, 4 ),   SWM32_PIN(40, P5    , P, 5 ),
	SWM32_PIN(41, P6    , P, 6 ),   SWM32_PIN(42, P7    , P, 7 ),   SWM32_PIN(43, P8    , P, 8 ),   SWM32_PIN(44, P9    , P, 9 ),
	SWM32_PIN(45, P10   , P, 10),   SWM32_PIN(46, P11   , P, 11),   SWM32_PIN(47, P12   , P, 12),   SWM32_PIN(48, P13   , P, 13),
	SWM32_PIN(49, B0    , B, 0 ),   SWM32_PIN(50, A0    , A, 0 ),   SWM32_PIN(51, A1    , A, 1 ),   SWM32_PIN(52, A2    , A, 2 ),
	SWM32_PIN(53, A3    , A, 3 ),   SWM32_PIN(54, A4    , A, 4 ),   SWM32_PIN(55, A5    , A, 5 ),   SWM32_PIN(56, N7    , N, 7 ),
	SWM32_PIN(57, N6    , N, 6 ),   SWM32_PIN(58, N5    , N, 5 ),   SWM32_PIN(59, N4    , N, 4 ),   SWM32_PIN(60, N3    , N, 3 ),
	SWM32_PIN(61, A9    , A, 9 ),   SWM32_PIN(62, A10   , A, 10),   SWM32_PIN(63, A11   , A, 11),   SWM32_PIN(64, A12   , A, 12),
};

#define ITEM_NUM(items)  ( sizeof(items) / sizeof(items[0]) )
static pin_t *get_pin(uint8_t pin)
{
    pin_t *index;
    if (pin < ITEM_NUM(swm32_pin_map))
    {
        index = (pin_t *)&swm32_pin_map[pin];
        if ( index->port == GPIO0 )
            index = NULL;
    }
    else
    {
        index = NULL;
    }
    return index;
};

static int read( int pin)
{
    pin_t *index;
    index = get_pin(pin);
    if ( index == NULL )
    {
        return 0;
    }
    return GPIO_GetBit(index->port, index->group_index);
}


static void write( int pin, int value)
{
    pin_t *index;
    index = get_pin(pin);
    if (index == NULL)
    {
        return;
    }
    if (value)
    {
        GPIO_SetBit(index->port, index->group_index);
    }
    else
    {
        GPIO_ClrBit(index->port, index->group_index);
    }
}


static void set_mode( int pin, int mode)
{
    pin_t *index;
    int dir = 0;
    int pull_up = 0;
    int pull_down = 0;
    index = get_pin(pin);
    if (index == NULL)
    {
        return;
    }
    /* Configure GPIO_InitStructure */
    if (mode == PIN_MODE_OUTPUT)
    {
        /* output setting */
        dir = 1;
				
    }
    else if (mode == PIN_MODE_INPUT)
    {
        /* input setting: not pull. */
        dir = 0;
    }
    else if (mode == PIN_MODE_INPUT_PULLUP)
    {
        /* input setting: pull up. */
        dir = 0;
        pull_up = 1;
    }
    else if (mode == PIN_MODE_INPUT_PULLDOWN)
    {
        /* input setting: pull down. */
        dir = 0;
        pull_down = 1;
    }
    else if (mode == PIN_MODE_OUTPUT_OD)
    {
        /* output setting: od. */
        dir = 1;
        pull_up = 0;
    }
    GPIO_Init(index->port, index->group_index, dir, pull_up, pull_down);
}

static int attach_irq( int pin,int mode,pin_callback_t cb,void *args )
{
    pin_t *index;
//    int level;
    index = get_pin(pin);
    if (index == NULL)
    {
        return 1;  //return RT_EINVAL;
    }
//    level = rt_hw_interrupt_disable();
    index->callback = cb;
    index->callback_args = args;
    index->irq_mode = mode;

//    rt_hw_interrupt_enable(level);
    return 0;
}

static int detach_irq( int pin)
{
    pin_t *index;
//    int level;
    index = get_pin(pin);
    if (index == NULL)
    {
        return 1;  //return RT_EINVAL;
    }
//    level = rt_hw_interrupt_disable();
    index->callback = 0;
    index->callback_args = 0;
    index->irq_mode = 0;
//    rt_hw_interrupt_enable(level);
    return 0;
}

static int irq_enable(int pin,int enabled)
{
    pin_t *index;
//    int level = 0;
    index = get_pin(pin);
    if (index == NULL)
    {
      return 1;  //return RT_EINVAL;
    }
    if (enabled == PIN_IRQ_ENABLE)
    {
			switch (index->irq_mode)
			{
        case PIN_IRQ_MODE_RISING:
            GPIO_Init(index->port, index->group_index, 0, 0, 1);
            EXTI_Init(index->port, index->group_index, EXTI_RISE_EDGE);
            break;
        case PIN_IRQ_MODE_FALLING:
            GPIO_Init(index->port, index->group_index, 0, 1, 0);
            EXTI_Init(index->port, index->group_index, EXTI_FALL_EDGE);
            break;
        case PIN_IRQ_MODE_RISING_FALLING:
            GPIO_Init(index->port, index->group_index, 0, 1, 1);
            EXTI_Init(index->port, index->group_index, EXTI_BOTH_EDGE);
            break;
        case PIN_IRQ_MODE_HIGH_LEVEL:
            GPIO_Init(index->port, index->group_index, 0, 0, 1);
            EXTI_Init(index->port, index->group_index, EXTI_HIGH_LEVEL);
            break;
        case PIN_IRQ_MODE_LOW_LEVEL:
            GPIO_Init(index->port, index->group_index, 0, 1, 0);
            EXTI_Init(index->port, index->group_index, EXTI_LOW_LEVEL);
            break;
				
				case PIN_IRQ_MODE_RISING_NUD:
						EXTI_Init(index->port, index->group_index, EXTI_FALL_EDGE);
						break;
        default:
//            rt_hw_interrupt_enable(level);
            return 1;  //return RT_EINVAL;
        }
		
//        level = rt_hw_interrupt_disable();
        NVIC_EnableIRQ(index->irq);
        EXTI_Open(index->port, index->group_index);
//        rt_hw_interrupt_enable(level);
    }
    else if ( enabled == PIN_IRQ_DISABLE )
    {
        NVIC_DisableIRQ(index->irq);
        EXTI_Close(index->port, index->group_index);
    }
    else
    {
        return 1;  //return RT_ENOSYS;
    }
    return 0;
}
/*----------------------------------------------------------------------------------*/

static drvp_gpio_t m_drvp_gpio = {
	read,
	write,
	set_mode,
	attach_irq,
	detach_irq,
	irq_enable
};
drvp_gpio_pt drvp_gpio = &m_drvp_gpio;
/*----------------------------------------------------------------------------------*/

void GPIOA_Handler(void)
{
    static int gpio[MaxLine];
    int index = 0;
    static int init = 0;
    pin_t *pin;
    /* enter interrupt */
//    rt_interrupt_enter();
    if (init == 0)
    {
        init = 1;
        for (pin = (pin_t *)&swm32_pin_map[1];
                pin->package_index < ITEM_NUM(swm32_pin_map);
                pin++)
        {
            if (pin->port == GPIOA)
            {
                gpio[index] = pin->package_index;
                index++;
                //RT_ASSERT(index <= MaxLine)
            }
        }
    }
    for (index = 0; index < MaxLine; index++)
    {

        pin = get_pin(gpio[index]);
//        if ( index != NULL)
        if (gpio[index] == 0 ) continue;
				if (gpio[index] >= ITEM_NUM(swm32_pin_map)  ) continue;
			
        
				if (EXTI_State(pin->port, pin->group_index))
				{
						EXTI_Clear(pin->port, pin->group_index);
						if (pin->callback)
						{
								pin->callback(pin->callback_args);
						}
				}
     
    }
    /* leave interrupt */
    //rt_interrupt_leave();
}
void GPIOB_Handler(void)
{
    static int gpio[MaxLine];
    int index = 0;
    static int init = 0;
    pin_t *pin;
    /* enter interrupt */
//    rt_interrupt_enter();
    if (init == 0)
    {
        init = 1;
        for (pin = (pin_t *)&swm32_pin_map[1];
                pin->package_index < ITEM_NUM(swm32_pin_map);
                pin++)
        {
            if (pin->port == GPIOB)
            {
                gpio[index] = pin->package_index;
                index++;
                //RT_ASSERT(index <= MaxLine)
            }
        }
    }
    for (index = 0; index < MaxLine; index++)
    {

        pin = get_pin(gpio[index]);
//        if ( index != NULL)
        if (gpio[index] == 0 ) continue;
				if (gpio[index] >= ITEM_NUM(swm32_pin_map)  ) continue;
			
        
				if (EXTI_State(pin->port, pin->group_index))
				{
						EXTI_Clear(pin->port, pin->group_index);
						if (pin->callback)
						{
								pin->callback(pin->callback_args);
						}
				}
     
    }
    /* leave interrupt */
//    rt_interrupt_leave();
}
void GPIOC_Handler(void)
{
    static int gpio[MaxLine];
    int index = 0;
    static int init = 0;
    pin_t *pin;
    /* enter interrupt */
//    rt_interrupt_enter();
    if (init == 0)
    {
        init = 1;
        for (pin = (pin_t *)&swm32_pin_map[1];
                pin->package_index < ITEM_NUM(swm32_pin_map);
                pin++)
        {
            if (pin->port == GPIOC)
            {
                gpio[index] = pin->package_index;
                index++;
                //RT_ASSERT(index <= MaxLine)
            }
        }
    }
    for (index = 0; index < MaxLine; index++)
    {

        pin = get_pin(gpio[index]);
//        if ( index != NULL)
        if (gpio[index] == 0 ) continue;
				if (gpio[index] >= ITEM_NUM(swm32_pin_map)  ) continue;
			
        
				if (EXTI_State(pin->port, pin->group_index))
				{
						EXTI_Clear(pin->port, pin->group_index);
						if (pin->callback)
						{
								pin->callback(pin->callback_args);
						}
				}
     
    }
    /* leave interrupt */
//    rt_interrupt_leave();
}
void GPIOM_Handler(void)
{
    static int gpio[MaxLine];
    int index = 0;
    static int init = 0;
    pin_t *pin;
    /* enter interrupt */
    //rt_interrupt_enter();
    if (init == 0)
    {
        init = 1;
        for (pin = (pin_t *)&swm32_pin_map[1];
                pin->package_index < ITEM_NUM(swm32_pin_map);
                pin++)
        {
            if (pin->port == GPIOM)
            {
                gpio[index] = pin->package_index;
                index++;
                //RT_ASSERT(index <= MaxLine)
            }
        }
    }
    for (index = 0; index < MaxLine; index++)
    {

        pin = get_pin(gpio[index]);
//        if ( index != NULL)
        if (gpio[index] == 0 ) continue;
				if (gpio[index] >= ITEM_NUM(swm32_pin_map)  ) continue;
			
        
				if (EXTI_State(pin->port, pin->group_index))
				{
						EXTI_Clear(pin->port, pin->group_index);
						if (pin->callback)
						{
								pin->callback(pin->callback_args);
						}
				}
     
    }
    /* leave interrupt */
//    rt_interrupt_leave();
}


void GPION_Handler(void)
{
    static int gpio[MaxLine];
    int index = 0;
    static int init = 0;
    pin_t *pin;
		int val = 0;
    /* enter interrupt */
//    rt_interrupt_enter();
    if (init == 0)
    {
        init = 1;
        for (pin = (pin_t *)&swm32_pin_map[1];
                pin->package_index < ITEM_NUM(swm32_pin_map);
                pin++)
        {
            if (pin->port == GPION)
            {
                gpio[index] = pin->package_index;
                index++;
                //RT_ASSERT(index <= MaxLine)
            }
        }
    }
    for (index = 0; index < MaxLine; index++)
    {

        pin = get_pin(gpio[index]);
//        if ( index != NULL)
        if (gpio[index] == 0 ) continue;
				if (gpio[index] >= ITEM_NUM(swm32_pin_map)  ) continue;
			
        
				if (EXTI_State(pin->port, pin->group_index))
				{
						EXTI_Clear(pin->port, pin->group_index);
						if (pin->callback)
						{
								pin->callback(pin->callback_args);
						}
				}
     
    }
    /* leave interrupt */
//    rt_interrupt_leave();
}
void GPIOP_Handler(void)
{
    static int gpio[MaxLine];
    int index = 0;
    static int init = 0;
    pin_t *pin;
    /* enter interrupt */
//    rt_interrupt_enter();
    if (init == 0)
    {
        init = 1;
        for (pin = (pin_t *)&swm32_pin_map[1];
                pin->package_index < ITEM_NUM(swm32_pin_map);
                pin++)
        {
            if (pin->port == GPIOP)
            {
                gpio[index] = pin->package_index;
                index++;
                //RT_ASSERT(index <= MaxLine)
            }
        }
    }
    for (index = 0; index < MaxLine; index++)
    {
        pin = get_pin(gpio[index]);
//        if ( index != NULL)
        if (gpio[index] == 0 ) continue;
				if (gpio[index] >= ITEM_NUM(swm32_pin_map)  ) continue;
			
        
				if (EXTI_State(pin->port, pin->group_index))
				{
						EXTI_Clear(pin->port, pin->group_index);
						if (pin->callback)
						{
								pin->callback(pin->callback_args);
						}
				}
     
    }
    /* leave interrupt */
//    rt_interrupt_leave();
}



