

/*包含驱动底层接口*/
#include "./driver_port/fk_drivers_port.h"

extern drv_gpio_pt gpio;

extern loop_pt loop;
extern uart_pt uart0;
extern uart_pt uart1;
extern uart_pt uart2;
extern uart_pt uart3;


#define io_bus_chip 12
extern io_bus_pt io_bus;

extern pwm_bus_pt pwm_bus;


extern adc_bus_pt adc1_bus;

extern hc595_opt_pt hc595_opt;
extern hc595_pin_pt LED_BUS;

extern iopin_pt io_pin;


//模拟iic代码地址规则，(ADDR+RW)<<1 
//比方说，0X50<<1 = 0xA0
#define eep_dev_addr0 0XA0

extern eeprom_pt eep_24c0x;

extern rtc_dt_t rtc_dt;
extern rtc_pt rtc;

extern drv_coder_pt coder;
