
#include "framework.h"

//#define PRT_DATA

#define PRT_LOG(X...) sysprt->alog(X)

static void init(void)
{
	drvp_iic->init();
	
}

static void delay1ms()		//@24.000MHz
{
	volatile uint32_t d=1000000;

	while(d--);
	return ;
}

//static void delay10ms()		//@24.000MHz
//{
////	timtick->delay(10);
//	delay1ms();delay1ms();delay1ms();delay1ms();delay1ms();
//	delay1ms();delay1ms();delay1ms();delay1ms();delay1ms();
//	
//}


static uint8_t read_byte(uint8_t slv_addr,uint32_t raddr,int* err)
{
	uint32_t line = 0;
	uint8_t dat,addr,ack;
	
	addr=(raddr/0xff*2); //address 0 1 2 3， addr 0 2 4 6
	
	//选页 写：第1页A0 ，2页A2 ，3页A4，4页A6
	ack = drvp_iic->start(slv_addr+addr );
	if( ack == 0 ){line = __LINE__;goto __error;}
		
	ack = drvp_iic->write( raddr%0xff);	//写地址0-255
	if( ack == 0 ){line = __LINE__;goto __error;}
	
	//选页 读：第1页A1 ，2页A3 ，3页A4，4页A7 就是写+1
	ack = drvp_iic->start( slv_addr+1+addr );
	if( ack == 0 ){line = __LINE__;goto __error;}
	
	dat = drvp_iic->read( 0 );
	
	drvp_iic->stop( );
		
  return dat;
__error:
	PRT_LOG("\r\n__error: read %d \r\n",line);
	err[0]++ ;
	return 0;
}



//向24c02的address地址中写入一字节数据info/
static void write_byte( uint8_t slv_addr,uint32_t waddr,uint8_t dat,int* err )
{
	uint32_t line = 0;
	uint8_t addr,ack;

	addr=waddr/0xff*2; //address/0xff= 0 1 2 3， addr= 0 2 4 6
	
		//选页 写：第1页A0 ，2页A2 ，3页A4，4页A6
	ack = drvp_iic->start( slv_addr+addr );
	if( ack == 0 ){line = __LINE__;goto __error;}

	ack = drvp_iic->write( waddr%0xff);	//写地址0-255
	if( ack == 0 ){line = __LINE__;goto __error;}

	ack = drvp_iic->write( dat);	//写地址0-255
	if( ack == 0 ){line = __LINE__;goto __error;}
	
	drvp_iic->stop();
	return;
__error:
	PRT_LOG("\r\n__error: write %d \r\n",line);
	err[0]++ ;
}


static int read( uint8_t slv_addr,uint8_t addr,uint8_t *buf,uint8_t len )
{
	volatile uint32_t d=0;
	uint32_t i;
	int err = 0;
	for( i=0;i<len;i++ )
	{
		while(1)
		{
			buf[i] = read_byte( slv_addr,i+addr,&err );			
			if( 0 == err ) break;
			drvp_iic->stop();
			PRT_LOG( "read[%d]: err... \r\n",i );
			for(d = 0; d < 10000; d++) __NOP();
		}
//		printf( "read[%d]:%02x \r\n",i,buf[i] );
		for(d = 0; d < 10000; d++) __NOP();
	}
	return 1;
}


static int write( uint8_t slv_addr,uint8_t addr,uint8_t *buf,uint8_t len )
{
	volatile uint32_t d=0;
	uint32_t i;
	int err = 0;
	for( i=0;i<len;i++ )
	{
		while(1)
		{
			write_byte( slv_addr,i+addr,buf[i],&err );
			if( 0 == err ) break;
			drvp_iic->stop();
			PRT_LOG( "write[%d]: err... \r\n",i );
			for(d = 0; d < 10000; d++) __NOP();
		}
		delay1ms();
//		for(d = 0; d < 100000; d++) __NOP();
//		printf( "write[%d]:%02x \r\n",i,buf[i] );
	}
	return 1;
}



static eeprom_t do_eeprom ={init,read,write};

eeprom_pt eep_24c0x = &do_eeprom;

