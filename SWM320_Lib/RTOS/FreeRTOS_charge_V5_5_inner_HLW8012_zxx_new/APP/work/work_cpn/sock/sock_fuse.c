#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	



/*---------------------------------------------------------------------*/
//保险丝操作
static int fuse_err[MaxSock] = {0,0};

static uint32_t boot_sec = 0;	//记录检测程序运行了多少个5秒


int cat_fuse_err( int road )
{
	
	//至少检测了20s,才能反映保险丝的情况
	if( boot_sec>= 20 )
		return fuse_err[road];
	else
		return 0;
}

static uint8_t PinMapFuse[MaxSock] = {IO_fuse0,IO_fuse1};

void listen_fuse( void )
{
	static time_t otick_fuse;
	static time_t ntick_fuse;
	static uint32_t cntr[MaxSock] = {0,0};
	static uint32_t sta [MaxSock] = {0,0};
	uint32_t val;
#if ( BoardVision == BoardV5_4)
	//旧板子只有一个保险丝，以0为准
	val = gpio->read( PinMapFuse[0] );
	switch( sta[0] )
	{
		case 0://等待0
			if( 0 == val ) 	
			{
				sta[0]++;
			}
			break;
		case 1://等待1
			if( 1 == val )
			{
				cntr[0] ++;
				cntr[1] ++;
				sta[0]--;
			}
			break;
		default:
			sta[0]=0;
			break;
		
	}
	
#else
	
	for( int road=0;road<MaxSock;road++ )
	{
		val = gpio->read( PinMapFuse[road] );
		switch( sta[road] )
		{
			case 0://等待0
				if( 0 == val ) 	{sta[road]++;}
				break;
			case 1://等待1
				if( 1 == val )
				{
					cntr[road] ++;
					sta[road]--;
				}
				break;
			default:
				sta[road]=0;
				break;
			
		}
	}
#endif
	ntick_fuse = get_sys_ticks();
	
	if( 10000 >( ntick_fuse - otick_fuse ) )	return ;
	otick_fuse = ntick_fuse;
	
	//记录检测程序运行时间,每允许一天，重置计数器
	boot_sec = boot_sec + 5;
	if( boot_sec >24*60*60*1000 ) boot_sec = 0;
	
//	sysprt->talog("listen_fuse:%%010u \r\n",cntr );
	
	val = 0;
	for(int road=0;road<MaxSock;road++)
	{
			if( cntr[road] <20 )
			{
				val ++;
				fuse_err[road] = 1;
				sysprt->talog("[%d]listen_fuse:%010u \r\n",road,cntr[road] );
			}
			else
			{
				fuse_err[road] = 0;
			}
			cntr[road] = 0;
	
	}
	
	//指示灯
	if( val>0 )
	{
		led_board->set_fuse_led( __OFF );
	}
	else
	{
		led_board->set_fuse_led( __ON );
	}
	
	return;
}


/*----------------------------------------------------------------------------------*/



