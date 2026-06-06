#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

/*----------------------------------------------------------------------------------*/
static storage_t m_storage;
static void read_fac_conf_data( void )
{
	
	uint32_t *dst = (uint32_t*)(&m_storage); 
	uint32_t len = sizeof( storage_t )/4;
	sysprt->alog( "size = %d \r\n",sizeof( storage_t ) );

	for( int i=0;i<len;i++   )
	{
		*dst =  *((volatile uint32_t *)(addr_config+i*4));
		dst++;
	}
//	eep_24c0x->read( eep_dev_addr0,0,(uint8_t *)(wdat),sizeof( wdat_t ) );	
}

static void erase_fac_conf_data( void )
{
	uint32_t waddr = addr_config;
	PRT_LOG("erase_fac_conf_data\r\n");
	
	//在数据量不超过4K的情况下，直接擦除这个扇区即可。
	FLASH_Erase( addr_config );
	
	
}
static void write_fac_addr_conf_data( void )
{
	uint32_t len = sizeof( storage_t );
	FLASH_Write( addr_config,(uint32_t*)(&m_storage), len/4 );
}
void work_data_write( void )
{
	erase_fac_conf_data();
	write_fac_addr_conf_data();
}
/*----------------------------------------------------------------------------------*/
//文件内部静态分配的变量空间
static wdat_t m_wdat={
	.storage = &m_storage,
};

//对外全局变量
wdat_pt wdat = &m_wdat;
/*----------------------------------------------------------------------------------*/


void work_data_load( void )
{
			
	read_fac_conf_data(   );
	if( wdat->storage->boot_code != BOOT_CODE )
	{
		sysprt->alog( "It's first boot \r\n" );
		//初始化数据
		wdat->storage->boot_code = BOOT_CODE;
//		wdat->program = _app_update;
		wdat->storage->boot_code = _app_new;					//批量的时候，直接启动app
		wdat->storage->net_mode = NetMode_Air780;
		
		work_data_write();
	}
	else
		sysprt->alog( "It's not first boot \r\n" );
	
	wdat->storage->net_ptf = 1;
}
/*----------------------------------------------------------------------------------*/








