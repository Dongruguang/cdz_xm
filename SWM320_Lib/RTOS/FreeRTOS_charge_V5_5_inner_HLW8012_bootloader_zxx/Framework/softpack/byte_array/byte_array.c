
/*----------------------------------------------------------------------------------*/
#include "framework.h"
#define PRT_LOG(X...)	//sysprt->alog(X)	

/*----------------------------------------------------------------------------------*/

static void set_bit( uint8_t* buf,uint8_t ibit )
{
	int index = ibit>>3;	//index = ibit/8使用右移代替除法
	int offset = ibit&7;	//offset = 使用按位与代替求余操作
	buf[ index ] |= (1 << offset ); 
	
}
static void clr_bit( uint8_t* buf,uint8_t ibit )
{
	int index = ibit>>3;	//index = ibit/8使用右移代替除法
	int offset = ibit&7;	//offset = 使用按位与代替求余操作
	buf[ index ] &= ~(1 << offset ); 
}
static uint8_t read_byte( uint8_t* buf,uint8_t num )
{
	return buf[num];
}

static void lset_bit( int max_len,uint8_t* buf,uint8_t ibit )
{
	int index = ibit>>3;	//index = ibit/8使用右移代替除法
	int offset = ibit&7;	//offset = 使用按位与代替求余操作
	
	if( index >= max_len ) return;
	buf[ index ] |= (1 << offset ); 
	PRT_LOG("lset_bit index->%d,offset->%d \r\n",index,offset);
}
static void lclr_bit( int max_len,uint8_t* buf,uint8_t ibit )
{
	int index = ibit>>3;	//index = ibit/8使用右移代替除法
	int offset = ibit&7;	//offset = 使用按位与代替求余操作
	if( index >= max_len ) return;
	buf[ index ] &= ~(1 << offset ); 
	PRT_LOG("lclr_bit index->%d,offset->%d \r\n",index,offset);

}
static uint8_t lread_byte( int max_len,uint8_t* buf,uint8_t num )
{
	if( num >= max_len ) return 0;
	return buf[num];
}

static byte_array_t do_byte_array={
	.set_bit = set_bit,
	.clr_bit = clr_bit,
	.read_byte = read_byte,
	
	.lset_bit = lset_bit,
	.lclr_bit = lclr_bit,
	.lread_byte = lread_byte,
	
};
byte_array_pt byte_array = &do_byte_array;
