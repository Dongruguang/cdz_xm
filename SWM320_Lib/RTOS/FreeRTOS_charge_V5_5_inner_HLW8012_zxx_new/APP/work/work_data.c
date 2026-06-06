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
	int save = 0;
	
	read_fac_conf_data( );
	if( wdat->storage->boot_code != BOOT_CODE )
	{
		sysprt->alog( "It's first boot \r\n" );
		//初始化数据
		wdat->storage->boot_code = BOOT_CODE;
		wdat->storage->boot_app = 0;
		wdat->storage->net_mode = NetMode_Air724;
		wdat->storage->rp_ota_first_boot = 0;

		
		
		wdat->storage->k[0] = 5.2;
		wdat->storage->k[1] = 5.34;
		
		wdat->storage->b[0] = 0;
		wdat->storage->b[1] = 0;


		
		save ++;
	}
	else
	{
		sysprt->alog( "It's not first boot \r\n" );
		
		//判断不合法的值
		for( int id=0;id<MaxSock;id++ )
		{

				
		}
		
	/*-------------------------------------------------------*/
		if( wdat->storage->rp_ota_first_boot )
		{//此标识用于向服务器描述第一次启动(更新完APP)
			sysprt->alog( "rp_ota_first_boot = 1 \r\n" );
			wdat->work.rp_serv_first_boot = 1;
			wdat->storage->rp_ota_first_boot = 0;
			save ++;
		}
	}
	
	
	/*-------------------------------------------------------*/
	//如果本次需要存储，则写数据
	if( save != 0 )
	{
		work_data_write();
	}
	
	wdat->storage->net_ptf = 1;
}





