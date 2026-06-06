#include "framework.h"
/*------------------------------------------------------------------------*/


static uint8_t sysfb[LCD_YPixel][LCD_XPixel/8];

static void init( void )
{
	
	for( int y=0;y<LCD_YPixel;y++ )
		for( int x=0;x<LCD_XPixel;x++ )
		{
			sysfb[y][x] = 0;
		}
		

}


static void pixel(uint8_t x,uint8_t y,uint8_t color)							
{
	uint8_t *LCDFrameBuff = (uint8_t*)sysfb;
	//根据坐标值，确定像素位于第几个字节和这个字节的第几位
	uint8_t x_word,x_bit;
	x_word = x>>3;	//使用右移来取代除法	x_word = x/8;
	x_bit  = x&7; 	//使用按位与来取代求余
	
	if((x<LCD_XPixel)&&(y<LCD_YPixel))
	{
		//修改此字节的内容
		if( color )
		{
			LCDFrameBuff[y*16+x_word] |=(1<<(7-x_bit)) ;
		}
		else
		{
			LCDFrameBuff[y*16+x_word] &=~(1<<(7-x_bit)) ;
		}
	}
	//y*16表示第几行的行首地址,1行16个字节
					//j表示,字符在水平方向显示到第几个字节，一共两个字节
					//x表示,字符在水平方向上从第几个字节开始显示
					//i表示,字符在垂直方向显示到第几行
					//LCDFrameBuff[0]为字符在			 	第0行的首地址
					//LCDFrameBuff[0+y*16]					为字符在	第y行开始显示的首地址
					//LCDFrameBuff[0+y*16+j]				为字符	在第y行显示的第x_word个字符首地址
}

static void show_ascii(uint8_t x,uint8_t y, uint8_t ch)
{
	uint8_t i;
	uint8_t *pch=0;
	pch = ( uint8_t* )(&ascii_1608[ch*16]);//获取ASCII码的首地址
	uint8_t x_word = x>>3;	//使用右移来取代除法	x_word = x/8;
	for( i=0;i<16;i++ )
	{
		sysfb[ y+i ][x_word]= pch[i];
	}
		
	
}

static int show_ascii_str(uint8_t x,uint8_t y,char ch[])
{
	uint8_t i,len;
	len = strlen(ch);
	/*------------出错返回--------------------------------------------*/
	if(len>16)	return 0;	//超出本函数所能处理长度	
	if((16-x)<len)	return 0;//字符串太长，改行起始坐标之后的空间显示不下
	if(y>=48)		return 0;
	
	/*------------正常显示--------------------------------------------*/
	for(i=0;i<len;i++)
	{
		show_ascii(x,y,ch[i]);
		x += 8;
	}
	return 1;	
}

static void show( void )
{
	int yline = LCD_YPixel;
	int xword	= LCD_XPixel/8;
	uint8_t dat = 0;

	printf("\r\n");
	for( int l=0;l<yline;l++ )
	{	
		for( int x=0;x< xword;x++ )
		{
			dat = sysfb[ l ][ x ];
			for( int i=0;i<8;i++ )
			{
				if( dat&0x80 ) 	printf("X");
				else						printf(" ");
				
				dat = dat<<1;
			}
		}
		printf("\r\n");
	}
}
static dot_ui_t m_dot_ui={
	init,
	pixel,
	show_ascii,
	show_ascii_str,
	show
};
dot_ui_pt dot_ui = &m_dot_ui;
