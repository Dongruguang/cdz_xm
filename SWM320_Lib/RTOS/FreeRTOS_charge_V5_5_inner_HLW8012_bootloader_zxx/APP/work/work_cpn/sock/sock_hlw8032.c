
#include "work.h"
#define PRT_LOG(X...)	sysprt->dlog(X)	

#define MaxLen 48
#define Debug_Mode 2
static uint8_t cntr[MaxSock] = {0,0};
static uint8_t rbuf[MaxSock][MaxLen];

static double vol_c [ MaxSock ] = { 1.88,1.88 };

//static double curr_c[ MaxSock ] = { 1.00,1.00 };
//static double curr_c[ MaxSock ] = { 0.50,0.64 };
static uint32_t cal_cntr[ MaxSock ] = { 0,0 }; 		//计算 计数器

double dvol[ MaxSock ] ={220,220};
double dcurr[ MaxSock ] = { 1,1 };
double dpwr[ MaxSock ]={ 0,0 };

double last_dvol[ MaxSock ] ={220,220};
double last_dcurr[ MaxSock ] = { 1,1 };
double last_dpwr[ MaxSock ]={ 0,0 };


uint32_t dover[ MaxSock ]={ 0,0 };

typedef struct{
	uint8_t state_reg[1];					//状态寄存器
	uint8_t check_reg[1];					//检测寄存器
	uint8_t vol_para_reg[3];			//电压参数寄存器
	uint8_t vol_reg[3];						//电压寄存器
	uint8_t curr_para_reg[3];			//电流参数寄存器
	uint8_t curr_reg[3];					//电流寄存器
	uint8_t pwr_para_reg[3];			//功率参数寄存器
	uint8_t pwr_reg[3];						//功率寄存器
	uint8_t dat_updata_reg[1];		//数据更新寄存器
	uint8_t pf_reg[2];						//PF寄存器
	uint8_t check_sum_reg[1];			//校验和寄存器
}hlw_buf_t,*hlw_buf_pt;

typedef struct{
	uint32_t state_reg;						//状态寄存器
	uint32_t check_reg;						//检测寄存器
	uint32_t vol_para_reg;				//电压参数寄存器
	uint32_t vol_reg;							//电压寄存器
	uint32_t curr_para_reg;				//电流参数寄存器
	uint32_t curr_reg;						//电流寄存器
	uint32_t pwr_para_reg;				//功率参数寄存器
	uint32_t pwr_reg;							//功率寄存器
	uint32_t dat_updata_reg;			//数据更新寄存器
	uint32_t pf_reg;							//PF寄存器
	uint32_t check_sum_reg;				//校验和寄存器
}hlw_val_t,*hlw_val_pt;

/*----------------------------------------------------------------------------------*/

static hlw_val_t hlw_val[MaxSock];

static void deal_data( uint8_t road )
{
	uint8_t index = 0;
	uint8_t sum = 0;
	uint8_t sta = 0;
	int p = 0;
	if( cntr[road] < MaxLen ) return; 	//判断接收数据是否溢出
	index = 1;
	/*	[ 0][ 1]...[23]
			[40][41]...[63]
	*/
	
	for( int i=1;i<42;i++	)
	{
//		PRT_LOG(".");
		if( rbuf[road][i] != 0x5A ) continue;
		if( ( rbuf[road][i-1] != 0X55) && ( rbuf[road][i-1] != 0xAA ) && ( rbuf[road][i-1] < 0xf0 ) )
		 continue;
		
		
		
		
		
		
		//来到这里，说明收到帧头了，校验一下，后续的数据是否完整
		for(  p=0;p<21;p++ )
		{
			sum = sum + rbuf[road][ i + 1 + p ];
		}
		if( sum != rbuf[road][ i+22 ] ) continue;
		
		
		
		
#if 0
//#if ( Debug_Mode == 1 )
			PRT_LOG( "[ %02d ] sum:%02x ,rbuf[%d]:%02x ",road,sum,i+22,rbuf[road][i+22] );
			//贴出来
			PRT_LOG("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
				rbuf[road][i- 1],rbuf[road][i   ],rbuf[road][i+ 1],rbuf[road][i+ 2],rbuf[road][i+ 3],rbuf[road][i+ 4],
				rbuf[road][i+ 5],rbuf[road][i+ 6],rbuf[road][i+ 7],rbuf[road][i+ 8],rbuf[road][i+ 9],rbuf[road][i+10]
				);
			PRT_LOG("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \r\n",
				rbuf[road][i+11],rbuf[road][i+12],rbuf[road][i+13],rbuf[road][i+14],rbuf[road][i+15],rbuf[road][i+16],
				rbuf[road][i+17],rbuf[road][i+18],rbuf[road][i+19],rbuf[road][i+20],rbuf[road][i+21],rbuf[road][i+22]
				);
#endif			
		hlw_buf_pt hu = (hlw_buf_pt)( &(rbuf[road][i-1]) );
			//解析
		hlw_val[ road ].state_reg = hu->state_reg[0];	
		hlw_val[ road ].check_reg = hu->check_reg[0];
		hlw_val[ road ].vol_para_reg = ( hu-> vol_para_reg[0]<<16 )	+ ( hu-> vol_para_reg[1]<<8 ) 	+ ( hu-> vol_para_reg[2] );
		hlw_val[ road ].vol_reg = ( hu-> vol_reg[0]<<16 ) + ( hu-> vol_reg[1]<<8 ) + ( hu-> vol_reg[2] );
		hlw_val[ road ].curr_para_reg = ( hu-> curr_para_reg[0]<<16 ) + ( hu-> curr_para_reg[1]<<8 ) + ( hu-> curr_para_reg[2] );
		hlw_val[ road ].curr_reg = ( hu-> curr_reg[0]<<16 ) + ( hu-> curr_reg[1]<<8 ) + ( hu-> curr_reg[2] );
		hlw_val[ road ].pwr_para_reg = ( hu-> pwr_para_reg[0]<<16 ) + ( hu-> pwr_para_reg[1]<<8 ) + ( hu-> pwr_para_reg[2] );
		hlw_val[ road ].pwr_reg = ( hu-> pwr_reg[0]<<16 ) + ( hu-> pwr_reg[1]<<8 ) + ( hu-> pwr_reg[2] );
		hlw_val[ road ].dat_updata_reg = ( hu-> dat_updata_reg[0] );
		hlw_val[ road ].pf_reg = ( hu-> pf_reg[0]<<8 ) + hu-> pf_reg[1] ;
		hlw_val[ road ].check_sum_reg =  hu-> check_sum_reg[0] ;
		
//		if( hlw_val[ road ].state_reg > 0Xf0  ){ dover[ road ] = 1; }
		dvol[road] 	=  hlw_val[road].vol_para_reg * vol_c[road] / hlw_val[road].vol_reg ;
		dcurr[road] =  hlw_val[road].curr_para_reg *wdat->curr_ratio[road] / hlw_val[road].curr_reg ;		
		dpwr[road] 	=  hlw_val[road].pwr_para_reg * vol_c[road] * wdat->curr_ratio[road] / hlw_val[road].pwr_reg ;
		

		//-------------------------------------------------------------------------
		sta = rbuf[road][i-1];

#if 0
		//0xAA 这种情况下，命令合法，但是数据不可用。
		if( sta == 0xAA )
		{ 
			dvol[road] = 0.01;		
			dcurr[road] = 0.01;	
			dpwr[road] = 0.01;
			if( cat_sock_working( road ) )
					PRT_LOG( "Charge[%d] sta --- 0xAA			\r\n",road );
			goto __deal_next_cmd;
		}
		//>=0xF0 这种情况下，命令合法，但是数据不一定可用。
		if( sta >= 0xF0 )
		{
			if( ( sta >> 0) & 0x01 ) 
			{//电压参数寄存器、电流参数寄存器和功率参数器寄存器不能使用
				dvol[road] = 0.01;		
				dcurr[road] = 0.01;	
				dpwr[road] = 0.01;
				if( cat_sock_working( road ) )
					PRT_LOG( "Charge[%d] sta >>0 over \r\n",road );
				goto __deal_next_cmd;
			}
			
			if( ( sta >> 1) & 0x01 ) 
			{//1: 功率寄存器溢出
				dpwr[road] = 0.1;
				dcurr[road] = 0.01;
				if( cat_sock_working( road ) )
					PRT_LOG( "Charge[%d] sta >>1 over \r\n",road );
				goto __deal_next_cmd;
			}
			
			if( ( sta >> 2) & 0x01 ) 
			{//电流寄存器溢出
				dcurr[road] = 0.01;
				if( cat_sock_working( road ) )
					PRT_LOG( "Charge[%d] sta >>2 over \r\n",road );
				goto __deal_next_cmd;
			}
			
			if( ( sta >> 3) & 0x01 ) 
			{//电压寄存器溢出
				dvol[road] = 0.01;
				if( cat_sock_working( road ) )
					PRT_LOG( "Charge[%d] sta >>3 over \r\n",road );
				goto __deal_next_cmd;
			}

		}			
#endif


__deal_next_cmd:		
		cal_cntr[ road ] ++;			//计数
		cntr[road] = 0;						//清零
		return;
	}
	cntr[road] = 0;
}

void hlw_recv_callback_u3(uint8_t dat)
{
	if( cntr[ 0 ] >= MaxLen ) return;
	rbuf[ 0 ][ cntr[ 0 ] ] = dat;
	cntr[ 0 ]++;
	
}

void hlw_recv_callback_u2(uint8_t dat)
{
//	printf("%02x ",dat);
	if( cntr[ 1 ] >= MaxLen ) return;
	rbuf[ 1 ][ cntr[ 1 ] ] = dat;
	cntr[ 1 ]++;
	
}


/*----------------------------------------------------------------------------------*/

static void init( void )
{
	
}

static int cat_connect( int road )
{
	//if( ( dcurr[ road ] * dvol[ road ] ) < 5  ) return 0;
	
	return 1;
}

static double cat_epwr( int road )
{
//	sock_hlw_dat_pt sh = &( sock_hwl_dat[ road ]);
	return dpwr[ road ];//sh->SOCK_P;//curr_val[id];
}
static double cat_ecurr( int road )
{
//	sock_hlw_dat_pt sh = &( sock_hwl_dat[ road ]);
	
	//校准前直接返回
	if( (uint32_t)(wdat->curr_ratio[road] *10 ) >= 10 )
	{
		return dcurr[ road ];
	}
	
	if( dcurr[ road ] >0.15 )
		return ( dcurr[road]*0.88 - 0.0018 );
	else 
		return dcurr[ road ];

}

static double cat_evol( int road )
{
//	sock_hlw_dat_pt sh = &(sock_hwl_dat[ road ]);
	return dvol[ road ];//sh->SOCK_Vol;//curr_val[id];
}



static void work( void )
{
	static time_t otick = 0;
	static time_t ntick = 0;
	static uint32_t cntr = 0;
	
	for( int i=0;i<2;i++ )
		deal_data( i );
	
#if ( Debug_Mode == 2 )
	ntick = get_sys_ticks();
	if( 2000>( ntick - otick ) ) return;
	otick = ntick;
	if( wdat->hlw_ptf )
	{
		sysprt->alog("\r\n");
		for( int road=0;road<MaxSock;road++ )
			sysprt->alog( "@[%d][%08d] %fV %fA %fW    ",road,cal_cntr[road],dvol[road],dcurr[road],dpwr[road] );
		sysprt->alog("\r\n");
	}
#endif
	
}


static sock_hlw_opt_t do_sock_hlw_opt = {
	init,cat_connect,cat_epwr,cat_ecurr,cat_evol,work
};
/*---------------------------------------------------------------*/

sock_hlw_opt_pt sock_hlw_opt = &do_sock_hlw_opt;


