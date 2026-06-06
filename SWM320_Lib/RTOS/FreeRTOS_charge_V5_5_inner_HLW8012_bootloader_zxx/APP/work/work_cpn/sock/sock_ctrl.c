
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	


extern sock_mq_pt sock_mq;
extern sock_hlw_opt_pt sock_hlw_opt;

static int msta[ MaxSock ];	//描述2个插座的主状态

static int ssta[ MaxSock ];	//描述2个插座的子状态

static int order_remind[ MaxSock ];	//描述是否有订单请求
static order_t order[ MaxSock ];		//记录订单内容
//static event_t event[ MaxSock ];		//



//描述订单时间用
static uint64_t otick_order[ MaxSock ];			//描述开始时间
//static time_t ntick_order[ MaxSock ];			//描述当前时间
static uint64_t dtick_order[ MaxSock ];			//描述目标时间


/*---------------------------------------------------------------------*/
//硬件操作
static void open( int road )
{
	sysprt->alog( " open ( %d) \r\n",road );
	switch( road )
	{
		case 0:io_pin->out( 2, __H );break;
		case 1:io_pin->out( 1, __H );break;
		default:
			break;
	}
}
static void close( int road )
{
	sysprt->alog( " close ( %d) \r\n",road );
	switch( road )
	{
		case 0:io_pin->out( 2, __L );break;
		case 1:io_pin->out( 1, __L );break;
		default:
			break;
	}
}


static double cat_curr( int id )
{
	return sock_hlw_opt->cat_ecurr( id )  ;//curr_val[id];
}

/*---------------------------------------------------------------------*/
//保险丝操作
static int fuse_err = 0;

static int cat_fuse_err(void)
{
	return fuse_err;
}
static void listen_fuse( void )
{
	static time_t otick_fuse;
	static time_t ntick_fuse;
	static uint32_t cntr = 0;
	static int sta = 0;
	uint32_t val = 0;
	
	val = io_pin->in( 5 );
	
	switch( sta )
	{
		case 0://等待0
			if( 0 == val ) 
			{		
				sta++;
			}
			break;
		case 1://等待1
			if( 1 == val )
			{
				cntr ++;
				sta--;
			}
			break;
		default:
			break;
		
	}
	
	ntick_fuse = get_sys_ticks();
	
	if( 1000 >( ntick_fuse - otick_fuse ) )	return ;
	otick_fuse = ntick_fuse;
	
	
//	sysprt->talog("listen_fuse:%08d \r\n",cntr );
	
	if( cntr <40 )
	{	
		fuse_err = 1;
		led_board->set_fuse_led( __OFF );
		sysprt->talog("listen_fuse:%08d \r\n",cntr );
	}
	else
	{
		fuse_err = 0;
		led_board->set_fuse_led( __ON );
	}
	cntr = 0;
	return;
}
/*---------------------------------------------------------------------*/
//用于显示屏刷新

static void display( int id )
{
	uint32_t val_on = 0;			//描述应该亮几颗灯
	
	uint64_t ntick = get_sys_ticks();
	
	//定时1s刷新
	static time_t otick_disp[ MaxSock ];
	if( ( ntick - otick_disp[id] )<1000 ) return;
	otick_disp[ id ] = ntick;
	
	//获得计划充电时长
	uint64_t all_tick = 0;
	all_tick = dtick_order[ id ] - otick_order[ id ];		
	
	//获得实际充电时长
	uint64_t con_tick = 0;
	if( ntick > otick_order[ id ] )
		con_tick = ntick - otick_order[id];								
	else
		con_tick = 0;			
	
	//将本次订单时长分为7份(因为有7个灯)
	uint32_t temp = all_tick / 7 ;																//将所有时间分为7份
	
	//计算充电时长 占几份
	if( con_tick >= all_tick )
		val_on = 0;																									//如果运算结果，不合法，则从0开始逐渐亮7个灯。
	else		
		val_on = con_tick / temp;
		
	led_board->mode_req( id,_e_mode_charging,val_on,NULL );	//

	//在充电过程中，打印以下内容
	if(  dtick_order[ id ] > ntick )
		sysprt->alog("display[%02d]: %.3f,%010llu led[%ld]\r\n",id,cat_curr( id ), dtick_order[ id ] - ntick,val_on);
	else
		sysprt->alog("display[%02d]: %.3f,%010llu \r\n",id,cat_curr( id ),0 );
}

/*---------------------------------------------------------------------*/
//循环作业
static void cycle_work( int id )
{
	static uint32_t currs_err[ MaxSock ];			//描述电流过小错误的次数	
	static uint32_t currb_err[ MaxSock ];			//描述电流过大错误的次数
	static uint32_t cnct_err[ MaxSock ];			//描述未插插座错误的次数
	static time_t otick_cr[ MaxSock ];				//用于控制电流访问频率
	static time_t otick_ct[ MaxSock ];				//用于插座插入检测访问频率
	
	static double scurr[ MaxSock ]={0,0};						//用于控制平均电流
	static uint32_t scurr_cntr[ MaxSock ]={0,0};		//用于平均电流 读取次数
	static time_t otick_scurr_cycle[ MaxSock ];			//用于控制平均电流传输周期
	static time_t otick_scurr_read[ MaxSock ];			//用于控制平均电流读取周期
	
	
	time_t ntick = get_sys_ticks();						//描述当前系统滴答
	double curr = 0.00;												//用于存储当前电流
	switch( ssta[ id ] )
	{
		case 0://等待订单
			//在此处什么都不用做，收到订单后会修改ssta[id]的值
			
			break;
		case 1://开始订单
			open( id );						//接通插座
			currs_err[ id ] = 0;	//
			currb_err[ id ] = 0;
			cnct_err[ id ] = 0;		//
		
			scurr[ id ] = 0;			//开始订单时，电流清零
			scurr_cntr[ id ] = 0 ;
			otick_scurr_cycle[ id ] = get_sys_ticks();
			otick_scurr_read [ id ] = otick_scurr_cycle[ id ];
			
			ssta[id]++;						//
			break;
		case 2://充电中 1
			
			//定时读取并记录
			if( ( ntick - otick_scurr_read[ id ] ) >= 1000 )
			{
				otick_scurr_read[id] = ntick;
				
				scurr[id] += cat_curr( id );
				scurr_cntr[id]++;
			}
			
			//定时上传
			if( ( ntick - otick_scurr_cycle[ id ] ) >= (5*1000) )//单位ms
			{
				otick_scurr_cycle[id] = ntick;
				
				curr = scurr[id] / scurr_cntr[id];
				
				//限制数值
				if( curr >5.00 ) curr = 5.00;
				
				//请求传输给服务器
				sock_mq->put_event( order[id].ddh,_event_curr,id,NULL,0,curr );
				
				//重置读取
				scurr[id] = 0;
				scurr_cntr[id] = 0;
				otick_scurr_read[id] = ntick;
			}
			
			//-----------------------------------------------------
			//检测是否插入插座
#if 0
			if( ( ntick - otick_ct[ id ] ) >= 2000 )
			{
				otick_ct[id] = ntick;
				
				if( 0 ==  sock_hlw_opt->cat_connect( id ) )
				{//本次检测到没有插入插头
					cnct_err[ id ]++;
					sysprt->talog("id=%02d,cat_connect %d \r\n",id,cnct_err[ id ] );
					if( cnct_err[ id ] >= 10 )
					{//确实没有插入插头
						ssta[ id ] = 0xE0+oeb_no_ins;
						break;
					}
				}
				else{	cnct_err[ id ] = 0;	}
			}
#endif
			//-----------------------------------------------------

			//检测电流
#if 1
			if( ( ntick - otick_cr[ id ] ) >= 100 )
			{
				otick_cr[id] = ntick;
				curr = cat_curr( id );
				
				
				
				if( curr < 0.1 )
				{//本次检测电流过小
					currs_err[ id ]++;
					sysprt->talog("id=%02d,%fA curr is to small:%02d \r\n",id,curr,currs_err[ id ] );
					if( currs_err[ id ] >= (180*10) ) 		//10就是1s 3分钟就是180s
					{//电流电测确实太小
						ssta[ id ] = 0xE0+oeb_curr_too_small;	
						//请求传输给服务器
						sock_mq->put_event( order[id].ddh,_event_recore,oeb_curr_too_small,NULL,0,0 );
						break;

					}
				}
				else
				{
					if( curr > 4.0 )
					{//本次检测电流过大
						currb_err[ id ]++;
						sysprt->talog("id=%02d,%fA curr is to big:%02d \r\n",id,curr,currb_err[ id ] );
						if( currb_err[ id ] >= (60*10) )			//10就是1s 3分钟就是180s
						{//电流电测确实太大了
//							ssta[ id ] = 0xE0+oeb_curr_too_big;	
//							break;
							//请求传输给服务器
							sock_mq->put_event( order[id].ddh,_event_recore,oeb_curr_too_big,NULL,0,0 );
						}
					}
					else
					{//电流正常
						currs_err[ id ] = 0;
						currb_err[ id ] = 0;
					}
					
				}
			}
#endif
			//-----------------------------------------------------
			//检测保险丝是否烧坏
			if( 1 == cat_fuse_err() )
			{//保险丝损坏，报错
				ssta[ id ] = 0xE3;
				
			}
			//-----------------------------------------------------
			//检测订单超时
			if( ntick >= dtick_order[ id ] )
			{
				ssta[ id ] = 0xE0;
				break;
			}
			//-----------------------------------------------------
			//定时 刷新显示
			display( id );
			break;
		//----------------------------------------------------
		case 0xE0+oeb_time:					//充电时间耗尽结束
		case 0xE0+oeb_curr_too_small:				//充电过程电流异常(拔掉/充满)		
		case 0xE0+oeb_curr_too_big:					//充电过程电流异常(拔掉/充满)		
		case 0xE0+oeb_no_ins:								//充电过程插座被拔掉
		case 0xE0+oeb_fuse_broken:					//充电过程保险丝异常
			sysprt->taerr("id=%02d,event:%02d \r\n",id,ssta[id]-0XE0 );
			
			//关闭插座
			close( id );											
			
			//提交事件
			sock_mq->put_event( order[id].ddh,_event_sock,	id,ssta[id] - 0xE0 ,0,0 );
		
			//改变跑灯状态为，待机状态
			led_board->mode_req( id,_e_mode_standing,NULL,NULL );
			ssta[ id ] = 0;
			break;
			
		default:
			break;
	}
}



/*---------------------------------------------------------------------*/
//处理订单请求
static void cycle_wait_ordef_req( int id )
{
	
	if( 1 == order_remind[id] )
	{
		order_remind[id] = 0;
		switch( order[id].cmd )
		{
			//充电订单
			case oct_start_charge:
				sysprt->talog(" id=%02d:oct_start_charge [%08d] \r\n",id,get_sys_ticks());
				//获取订单起始时间	
				
				ssta[ id ] = 1;		//切换状态，在 cycle_work()中起作用。
			
				//记录:订单开始时间
				otick_order[id] = get_sys_ticks();			
				
				//记录:订单结束时间
				dtick_order[id] = otick_order[id] + order[id].dat;
				break;
		
			//取消充电	
			case oct_cancel_charge://
				dtick_order[id] = get_sys_ticks();
				break;
			
			default:
				break;
		}
	}
}
/*---------------------------------------------------------------------*/

//单个插座控制
static void ctrl_one_sock( int id )
{
	switch( msta[ id ] )
	{
		case 0://初始化
			
			close( id );
			otick_order[id] = 0;		
			order_remind[id] = 0;
		
			msta[ id ]++;
			ssta[ id ] = 0;
			break;
		
		case 1://轮询作业
			
			cycle_wait_ordef_req( id );
			
			cycle_work( id );
				
			break;
			
		default:
			break;
	}
	
}
/*---------------------------------------------------------------------*/

//查询插座是否正在充电
int cat_sock_working( int id )
{
	if( (msta[id] == 1)&&( ssta[id] == 2 ) )
		return 1;
	else
		return 0;
}

/*---------------------------------------------------------------------*/

static void wait_order( void )
{
	order_t torder;
	uint8_t id = 0;
	int i = 0;
	if( sock_mq->recv_order( &torder ) >= 0  )
	{
		
		sysprt->aerr( "recv_order :\r\n");
		sysprt->aerr( "torder.ddh :%s\r\n",torder.ddh );
		sysprt->aerr( "torder.num :%d\r\n",torder.num );
		sysprt->aerr( "torder.cmd :%d\r\n",torder.cmd );
		sysprt->aerr( "torder.dat :%d\r\n",torder.dat );
		
		//终端号转控制id
		id = torder.num - 1;
		if( id > MaxSock )
		{
			sysprt->aerr( "recv_order id(%d) > MaxSock \r\n",id );
			return;
		}
		//按id进行订单分发
		order_remind[ id ] = 1;
		sysprt->aerr( "recv_order id->%02 \r\n",id );
		for( i=0;i<20;i++ )
			order[ id ].ddh[i] = torder.ddh[i];
		
		order[ id ].num = torder.num;
		order[ id ].cmd = torder.cmd;
		order[ id ].dat = torder.dat;
		
	}
}

void sock_check_service_work( void )
{
	
}

void sock_ctrl_work( void )
{
	
	//监听保险丝状态
	listen_fuse();
	
	//等待订单指令
	wait_order();

	//监听电流状态
	sock_hlw_opt->work();
	
	for( int id=0;id<MaxSock;id++ )
	{
		//控制一个插座
		ctrl_one_sock( id );
	}
}

