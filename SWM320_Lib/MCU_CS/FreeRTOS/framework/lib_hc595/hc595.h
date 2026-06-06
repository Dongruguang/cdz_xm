


typedef struct{
	uint8_t dat;
	uint8_t clk;
	uint8_t ud;	//update
	uint8_t cs;	//使能引脚，在一些售货机场合，就需要用到使能引脚。

	
}hc595_pin_t;


typedef struct{
	void (*init)(hc595_pin_t* psc);
	void (*write)(hc595_pin_t* psc,uint8_t* data,int len);
	void (*updata)(hc595_pin_t* psc);

}hc595_opt_t;

extern hc595_opt_t* hc595_opt;
