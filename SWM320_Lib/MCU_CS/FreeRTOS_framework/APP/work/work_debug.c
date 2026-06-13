#include "work.h"
#include "message.h"

/*
work_debug.c
编写命令行代码
(1)config
	config zd_user lz_0001 lz_123456
	config net 0
	config net air780
	config kb 0 10.1 0
	config default 						重置数据
	
(2)ota
(3)ctrl
	没有插入网卡的时候，我们想要调试引脚，校准，就用此命令
	ctrl 0 open 10
	ctrl 0 close 0
	
(4)cat_data
	打印工作参数命令
	

*/
/*----------------------------------------------------------------------------------*/

extern int cmd_reboot( int argc,char** argv );
/*----------------------------------------------------------------------------------*/

int cmd_cat_data( int argc,char** argv ) 
{
	sysprintf( "%s\r\n",__func__ );
	sysprintf( "bootcode: %08x\r\n",wdat->storage->bootcode );
	sysprintf( "boot_app: %08x\r\n",wdat->storage->boot_app );
	sysprintf( "ota_req : %08x\r\n",wdat->storage->ota_req );
	sysprintf( "net_mode: %08x\r\n",wdat->storage->net_mode );
	
	for( int road=0;road<MaxSock;road++ )
		sysprintf( "[%d] y = %f*x + (%f) \r\n",
			road,wdat->storage->k[road],wdat->storage->b[road] );
	
	sysprintf( "user_name: %s\r\n",wdat->storage->user_name );
	sysprintf( "user_pwd : %s\r\n",wdat->storage->user_pwd  );
	
	
	return 0;
	
}
/*----------------------------------------------------------------------------------*/
//在宏里面，#用于记号串化，##用于记号粘连

#define __cmd_work_label( cmd ) __##cmd##_work

int cmd_config( int argc,char** argv ) 
{
	sysprintf( "%s\r\n",__func__ );
	int road = 0;
	float k=0,b=0;
	
	if( 0 == strcmp( argv[1],"kb" ) )
	{
		goto __cmd_work_label( kb );
	}
	if( 0 == strcmp( argv[1],"rst_kb" ) )
	{
		goto __cmd_work_label( rst_kb );
	}
	if( 0 == strcmp( argv[1],"net" ) )
	{
		goto __cmd_work_label( net );
	}
	if( 0 == strcmp( argv[1],"zd_user" ) )
	{
		goto __cmd_work_label( zd_user );
	}
	if( 0 == strcmp( argv[1],"default" ) )
	{
		goto __cmd_work_label( default );
	}
	return 0;
//-----------------------------------------
__cmd_work_label( kb ):
	//config kb 0 10.1 0
	if( argc != 5 ) { sysprintf("usage err\r\n"); return -2; }
	
	road = atoi( argv[2] );	//插座号,从0开始
	if( road >= MaxSock )
	{
		sysprintf("err:sock\r\n"); 
		return -2;
	}
	k = atof( argv[3] );
	b = atof( argv[4] );
	
	wdat->storage->k[ road ] = k;
	wdat->storage->b[ road ] = b;
	
	work_data_write();
	
	return 0;
//-----------------------------------------	
__cmd_work_label( rst_kb ):
	//config rst_kb 0
	if( argc != 3 ) { sysprintf("usage err\r\n"); return -2; }
	
	road = atoi( argv[2] );	//插座号,从0开始
	if( road >= MaxSock )
	{
		sysprintf("err:sock\r\n"); 
		return -2;
	}

	
	wdat->storage->k[ road ] = 10.0;
	wdat->storage->b[ road ] = 0;
	
	work_data_write();
	return 0;
	
__cmd_work_label( net ):
	//config net air780
	if( argc != 3 ) { sysprintf("usage err\r\n"); return -2; }
	
	if( 0 == strcmp( argv[2],"air780" ) )
	{
		wdat->storage->net_mode = NetMode_Air780;
	}
	else if( 0 == strcmp( argv[2],"air724" ) )
	{
		wdat->storage->net_mode = NetMode_Air724;
	}
	else
		wdat->storage->net_mode = NetMode_Not;
	
	work_data_write();
	
	return 0;
	
__cmd_work_label( zd_user ):
	//config zd_user ABC 123
	if( argc != 4 ) { sysprintf("usage err\r\n"); return -2; }
	
	memset( wdat->storage->user_name,0,32 );
	memset( wdat->storage->user_pwd ,0,32 );
	snprintf( wdat->storage->user_name,32,"%s",argv[2] );
	snprintf( wdat->storage->user_pwd ,32,"%s",argv[3] );
	
	sysprintf( "config zd_user:%s %s\r\n",wdat->storage->user_name,wdat->storage->user_pwd );
	
	work_data_write();
	
	return 0;

__cmd_work_label( default ):
	//config default
	wdat->storage->bootcode = 0;
	sysprintf( "config default set success\r\n" );
	
	//重启设备
	cmd_reboot( NULL,NULL );
	
	return 0;
}
/*----------------------------------------------------------------------------------*/
int cmd_ota( int argc,char** argv ) 
{
	sysprintf( "%s\r\n",__func__ );
	//设置ota标志，bootloader会自动升级
	wdat->storage->ota_req = 1;
	
	sysprintf( "ota_req -> 1\r\n" );
	sysprintf( "system reboot after 1s\r\n" );
	
	//重启设备
	cmd_reboot( NULL,NULL );
	return 0;
	
}
/*----------------------------------------------------------------------------------*/
int cmd_ctrl( int argc,char** argv ) 
{
	sysprintf( "%s\r\n",__func__ );
	int type= 0;
	int road = 0;
	uint32_t time=0;
	char ddh[32];
	if( argc != 4 ) return -1;
	
	//只有网络关闭的情况下，才允许该命令
	if( wdat->storage->net_mode != NetMode_Not )  return -2;
	
	utc_t* ndt = rtc->get_dt();
	snprintf( ddh,sizeof(ddh),"%04d%02d%02d%02d%02d%02d",
		ndt->year,ndt->month,ndt->day,
		ndt->hour,ndt->minute,ndt->second
		); 
	
	road = atoi( argv[1] );
	if( road >=MaxSock ) return -3;
	
	if( 0 == strcmp( argv[2],"open" ) )
	{
		type = _e_ntc_type_on;
		time = atol( argv[3] );
	}
	else
	{
		type = _e_ntc_type_off;
		time = 0;
	}
	
	//产生离线订单
	msg_ntc_opt->put_order( type,road,time,ddh );
	sysprintf( "ctrl->sock[%d] %s ->%d min\r\n",road,argv[2],time );
	return 0;
}
/*----------------------------------------------------------------------------------*/
