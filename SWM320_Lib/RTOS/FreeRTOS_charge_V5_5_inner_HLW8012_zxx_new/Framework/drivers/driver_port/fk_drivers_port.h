/*----------------------------------------------------------------------------------*/
extern drvp_gpio_pt drvp_gpio;

extern drv_uart_port_pt drvp_uart0;
extern drv_uart_port_pt drvp_uart1;
extern drv_uart_port_pt drvp_uart2;
extern drv_uart_port_pt drvp_uart3;

#define ADC1_UseCh 2

extern drvp_adc_bus_pt drvp_adc1_bus;

//extern drvp_hc165_pt drvp_hc165;

//extern drvp_hc595_pt drvp_hc595;

extern drvp_pwm_pt drvp_pwm;

extern drvp_io_bus_pt drvp_io_bus;
/*----------------------------------------------------------------------------------*/
extern drvp_hc595_port_t do_drvp_hc595_bus_0;
extern drvp_hc595_port_t do_drvp_hc595_bus_1;
extern drvp_hc595_port_t do_drvp_hc595_bus_2;
/*----------------------------------------------------------------------------------*/

enum{
	ioPin_Set_IN=0,
	ioPin_Set_OUT,
	
};

enum{
	ioPin_0 = 0,
	ioPin_1,
	ioPin_2,
	ioPin_3,
	ioPin_4,
	ioPin_5,
	ioPin_6,
	ioPin_7,
	ioPin_8,
	ioPin_max	
};
extern drvp_iopin_pt drvp_iopin[ ioPin_max ];

/*----------------------------------------------------------------------------------*/

extern drvp_iic_pt drvp_iic;
//extern drvp_soft_iic_pt drvp_siic;



/*----------------------------------------------------------------------------------*/
extern drvp_rtc_pt drvp_rtc;

/*----------------------------------------------------------------------------------*/
extern drvp_cntr_pt drvp_cntr;
