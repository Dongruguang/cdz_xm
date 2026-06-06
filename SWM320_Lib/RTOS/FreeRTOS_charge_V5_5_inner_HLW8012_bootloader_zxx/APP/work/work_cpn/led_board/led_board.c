#include "work.h"

#define PRT_LOG(X...)	//sysprt->alog(X)	

//静态分配显存
static ledb_buf_t 		m_disp_buf;
#define MaxRoad	2



static uint8_t led_map[ MaxRoad ][8] ={

#if ( BoardVision == BoardV5_4)
	{ 14,15,16,17,18,19,20,22 },
	{ 9 , 6, 5, 4, 3, 2, 1,10 },
#endif

	
#if ( BoardVision == BoardV5_5)

	{  8, 9,10,11,12,13,14,15 },
	{  7, 6, 5, 4, 3, 2, 1, 0 },
#endif
	
};

//左边插座显示保险丝，范围0~7
//右边插座显示网络灯，范围0~7
#if ( BoardVision == BoardV5_4)
static uint8_t sta_led[ MaxSock ] = {0,21};
#endif

#if ( BoardVision == BoardV5_5)

static uint8_t sta_led[ MaxSock ] = {0,15};		
#endif



static time_t otick[ MaxSock ]={ 0,0 };
static time_t ntick[ MaxSock ]={ 0,0 };											
static uint8_t msta[ MaxRoad ]={0,0};												//主状态
static uint8_t ssta[ MaxRoad ]={0,0};												//子状态
static uint32_t dat[ MaxRoad ][2]={ {0,0},{0,0} };					//数据

static uint8_t net_sta = 0;																	//网络状态
static uint8_t fuse_sta = 0;																//保险丝状态
static uint8_t init_flag = 0;																//初始化标志
/*----------------------------------------------------------------------------------*/

/*
hc595 总线
uint8_t road, uint8_t md, uint32_t dat0,uint32_t dat1
*/
typedef struct{
	uint8_t road;
	uint8_t md;
	uint8_t resver[2];
	uint32_t dat[2];
}rled_t,*rled_pt;

static thread_mq_t led_mq;

static void set_one_led(uint8_t road,uint8_t ibit,uint8_t pwr);
static void set_all_led( uint8_t road,uint8_t pwr );
static void show( void );

static void init( void )	
{
	
	
	m_disp_buf.led[0] =0xff;
	m_disp_buf.led[1] =0xff;
	m_disp_buf.led[2] =0xff;
	
	show();
	
	//创建队列
	led_mq = thread_mq_init("rled_t", sizeof(rled_t), 40);
	
	init_flag = 1;
}
/*----------------------------------------------------------------------------------*/

static void set_one_led(uint8_t road,uint8_t ibit,uint8_t pwr)
{
	uint8_t hwbit = led_map[road][ibit];
	uint8_t index = hwbit>>3;
	uint8_t tbit  = hwbit&7;
	uint8_t *buf = m_disp_buf.led;
	
	if( pwr == __ON ) 	{ buf[ index ] &= ~(1<<tbit); }
	else								{ buf[ index ] |=  (1<<tbit);	}
	
}

static void set_all_led( uint8_t road,uint8_t pwr )
{
	for( int i=0;i<7 ;i++ )
	{
		if( pwr == __ON ) { set_one_led( road,i,__ON  ); }
		else							{ set_one_led( road,i,__OFF ); }
	}
}
static void show( void )
{
	uint8_t len = 0;
	uint8_t* buf = m_disp_buf.led;
	len = sizeof( ledb_buf_t );
	//通过硬件发送出去
	hc595_opt->write( LED_BUS, buf,len );
	hc595_opt->update( LED_BUS );
}
/*----------------------------------------------------------------------------------*/

static void set_net_led( uint8_t net )
{
	net_sta = net;
}
static void set_fuse_led( uint8_t fuse )
{
	fuse_sta = fuse;
}
/*----------------------------------------------------------------------------------*/
static void disp_sta_led( void )
{
	uint8_t hwbit = 0,index = 0,tbit  = 0;
	uint8_t *buf =  m_disp_buf.led;
	
	hwbit = sta_led[0]; index =  hwbit>>3;	tbit  = hwbit&7;
	
	if( fuse_sta ) 	buf[ index ] &= ~(1<<tbit);
	else						buf[ index ] |=  (1<<tbit); 
	
	hwbit = sta_led[1]; index =  hwbit>>3;	tbit  = hwbit&7;
	if( net_sta ) 	buf[ index ] &= ~(1<<tbit);
	else						buf[ index ] |=  (1<<tbit); 
	
}
/*----------------------------------------------------------------------------------*/

//显示正在待机
static void disp_standing( uint8_t road )
{
//	static uint8_t twinkle[ MaxRoad ]={0,0};
	set_all_led( road,__OFF );	
//	disp_sta_led( );
//	show();	
}
//显示正在充电中
static void disp_charging( uint8_t road )
{
	static uint8_t cntr[ MaxRoad ] = {0,0};	
	static uint8_t val_on[ MaxRoad ] = {0,0};	//描述本次亮几颗灯
	uint8_t i = 0;
//	uint32_t temp = 0;
	
	ntick[ road ] = get_sys_ticks();
	if( 300 > ( ntick[road] - otick[road] ) ) return;
	otick[ road ] = ntick[road];
	
	
	switch( ssta[ road ] )
	{
		case 0:
			PRT_LOG( "------>[%d] ssta = %d \r\n",road,ssta[ road ] );	
			val_on[ road ] = dat[road][0];
			ssta[ road ]++;
			break;
		
		case 1:
			PRT_LOG( "------>[%d] ssta = %d \r\n",road,ssta[ road ] );	
			//点亮LED描述已充时间
			//dat[road][0]描述需要亮几个灯
			//dat[road][1]没有含义
			
			
			//点亮已充电量
			
			for( i=0;i<7;i++ )
			{
				if( i<val_on[ road ] ) 	set_one_led( road,i,__ON  );
				else										set_one_led( road,i,__OFF );
			}
			
			cntr[ road ] = val_on[ road ];
			if( cntr[ road ] >=7 )
			{
				ssta[ road ] = 0;
				break;
			}
			ssta[ road ]++;
			break;
		
		case 2:
			PRT_LOG( "------>[%d] ssta = %d \r\n",road,ssta[ road ] );	
			set_one_led( road,cntr[ road ],__ON );
			cntr[ road ]++;
			if( cntr[ road ] >=7 )
			{
				ssta[ road ] = 0;
				break;
			}
			
			break;
		
		default:
			break;
	}
}
//显示已经充满
static void disp_charge_full( uint8_t road )
{
	set_all_led( road,__ON );
	show();
}
/*----------------------------------------------------------------------------------*/


//dat0 表示需要亮几个灯
//dat1 无意义
static void mode_req( uint8_t road, uint8_t md, uint32_t dat0,uint32_t dat1 )
{
	int ret = 0;
	
	if( init_flag == 0 ) return;
	
	rled_t rled;
	rled.road = road;
	rled.md = md;
	rled.dat[0] = dat0;
	rled.dat[1] = dat1;
	
	ret = thread_mq_send( led_mq,&rled, sizeof(rled_t), 0x02 );
	if( ret == bFALSE )
	{
		sysprt->aerr("err:mode_req \r\n");
	}
}
/*----------------------------------------------------------------------------------*/
static int recv_order( rled_pt rled )
{
	if( init_flag == 0 ) return -1;
	int ret = thread_mq_recv( led_mq, rled, sizeof(rled_t), 0);
	if(ret != bFALSE) 
		return 0;
	else
		return -1;
	
}
/*----------------------------------------------------------------------------------*/

static void  deal_led_req(  )
{
	int road = 0;
	rled_t rled;
	if(  recv_order( &rled ) >= 0 )
	{
		road = rled.road;
		PRT_LOG( "[%d] req_flag :%d - %d \r\n",road,msta[road],req_mode[road] );	
		if( msta[ road ] != rled.md )
		{
			msta[ road ] = rled.md;
			ssta[ road ] = 0;
		}
		dat[ road ][0] = rled.dat[0];
		dat[ road ][1] = rled.dat[1];
		
		
	}
}

/*----------------------------------------------------------------------------------*/

static void work_one( uint8_t road )
{
	
//	return ;
	//处理流程
	switch( msta[ road ] )
	{
		//
		case _e_mode_standing:	
			disp_standing( road );
			break;
		case _e_mode_charging:
			disp_charging( road );
			break;		
		case _e_mode_charge_full:
			disp_charge_full( road );
			break;
		default:
			break;
	}
}

static void work( void )
{
	static time_t ot=0,nt=0;
//	static uint32_t cntr = 0;
	if( 0 == init_flag ) return;
//	
	nt = get_sys_ticks();
	if( 50 > ( nt - ot ) ) return;
	ot = nt;
	
	//处理请求
	deal_led_req( );
	
	for( int id=0;id<MaxRoad;id++ )
		work_one( id );
	disp_sta_led( );
	
	show( );

}
/*----------------------------------------------------------------------------------*/
	
	
static led_board_t do_led_board={
	init,	
	set_net_led,
	set_fuse_led,
	mode_req,
	work
};

led_board_pt led_board = &do_led_board;

