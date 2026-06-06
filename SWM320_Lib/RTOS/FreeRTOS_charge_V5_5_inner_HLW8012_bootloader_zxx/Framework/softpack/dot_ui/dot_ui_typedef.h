


typedef struct{
	void (*init)( void );
	void (*pixel)(uint8_t x,uint8_t y,uint8_t color);
	void (*show_ascii)(uint8_t x,uint8_t y, uint8_t ch);
	int (*show_ascii_str)(uint8_t x,uint8_t y,char ch[]);
	void (*show)( void );
}dot_ui_t,*dot_ui_pt;

