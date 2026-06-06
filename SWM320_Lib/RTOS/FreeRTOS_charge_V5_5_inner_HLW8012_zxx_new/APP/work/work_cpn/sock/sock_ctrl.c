
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	










/*---------------------------------------------------------------------*/
//硬件操作
//用户肉眼看过去，左边是1，右边是2
//程序员眼中，左边是0，右边是1
//对应的硬件，左边是 IO_ELC1，IO_ELC0。(简单实验就知道了)
static void open( int road )
{
	sysprt->alog( " open ( %d) \r\n",road );
	switch( road )
	{

		case 0:gpio->write( IO_ELC1,1 );break;
		case 1:gpio->write( IO_ELC0,1 );break;
		default:
			break;
	}
}
static void close( int road )
{
	sysprt->alog( " close ( %d) \r\n",road );
	switch( road )
	{

		case 0:gpio->write( IO_ELC1,0 );break;
		case 1:gpio->write( IO_ELC0,0 );break;

		default:
			break;
	}
}


/*---------------------------------------------------------------------*/

static time_t otick_ch[MaxSock] = {0,0};
static void rst_check_hight( int id )
{
	otick_ch[id] = get_sys_ticks();
	coder->clr_val( id );
}

//返回1，则电流高
int check_hight_event(int id,uint32_t tick)
{
	int ret = 0;
	uint32_t val = 0;
	if( (tick - otick_ch[id]) < (5*1000) ) return 0;
	otick_ch[id] = tick;
#if ( BoardVision == BoardV5_4)
	//旧板子不存在高电流预警信号,直接返回0
	ret = 0;
#endif
	
#if ( BoardVision == BoardV5_5)
	val = coder->cat_val( id );
	if(  val >= 100 ) 
	{
		ret =1;
		PRT_LOG("check_hight_event(%d) = %d \r\n",id,coder->cat_val( id ) );
		coder->clr_val( id );
	}
#endif
	return ret;
}





/*---------------------------------------------------------------------*/
//用于显示屏刷新

static void display( int id )
{
	uint32_t val_on = 0;			//描述应该亮几颗灯
	
	uint64_t ntick = get_sys_ticks();
	sock_sta_pt pssock = &(socks[id]);
	
	//定时1s刷新
	static time_t otick_disp[ MaxSock ];
	
	if( ( ntick - otick_disp[id] )<1000 ) return;
	otick_disp[ id ] = ntick;
	
	utime_refresh( pssock,ntick);
	
	//获得计划充电时长
 	uint32_t all_time = pssock->atime_order;

	//获得实际充电时长
	uint32_t con_time = pssock->utime_order;

	
	//将本次订单时长分为7份(因为有7个灯)
	uint32_t temp = all_time / 7 ;																//将所有时间分为7份
	
	//计算充电时长 占几份
	if( con_time >= all_time )
		val_on = 0;																									//如果运算结果，不合法，则从0开始逐渐亮7个灯。
	else
		val_on = con_time / temp;
		
	led_board->mode_req( id,_e_mode_charging,val_on,NULL );	//
	
	//在充电过程中，打印以下内容
//	if(  pssock->dtick_order > ntick )
//		sysprt->alog("display[%02d]: %010llu s led[%ld]->now:[%04d/%0.5fw]%.5fA\r\n",
//								id,(pssock->dtick_order - ntick)/1000,val_on,hlw8012->read_pul(id),sock_hlw_opt->cat_epwr(id),cat_curr( id ));
//	else
//		sysprt->alog("display[%02d]: %010llu->now:%.5f\r\n",id,0,cat_curr( id ) );
}
/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
#define read_curr_tick 1000											//读取电流的滴答周期
#define avg_curr_cntr	 5												//读多少次电流，求一次平均值


static time_t otick_put_curr[MaxSock] = {0,0};								//用于定时传电流给服务器
static time_t otick_curr_diff[MaxSock] = {0,0};						//用于定时5s判断电流变化状态

//static uint32_t rtd_cntr
//定时采样电流，求平均值，并上传到服务器
static void cycle_listen_curr( int id )
{
	uint8_t com = id+1;
	rtc_dt_t *dt = 0;
	double diff_curr = 0;
	int report_curr = 0;
	sock_sta_pt pssock = &(socks[id]);
	
	time_t ntick = get_sys_ticks();									//描述当前系统滴答
	
	//每隔5s判断一次电流值是否变化较大( 20% ),如果变化较大，直接传一次
	if( ( ntick - otick_curr_diff[id] ) > 5000 )
	{
		otick_curr_diff[id] = ntick;
		
		//提高程序健壮性
		if( pssock->average_curr <0 ) pssock->average_curr = 0;
		if( pssock->base_curr <0 ) 		pssock->base_curr = 0;
		
		
		//相减,得到差值
		if( pssock->average_curr > pssock->base_curr  )
			diff_curr = pssock->average_curr - pssock->base_curr;
		else
			diff_curr = pssock->base_curr - pssock->average_curr;
		
		report_curr = 0;
		//前面40s，直接定时5s发，这样可以在后台直接看到用户插没插用电器
		if( ( ntick - pssock->otick_order ) < 40000 ) report_curr ++;
		//正常情况下，电流波动再发
		if( ( diff_curr > ( pssock->base_curr * 0.2) )&&( pssock->average_curr >0.09  ) )
			report_curr++;
		
		//
		if( report_curr >0 )
		{//传一次电流
			goto __report_curr;
		}
		
		
	}
	
	//定时 传一次电流值 给服务器
//	if( ( ntick - otick_put_curr[id] ) > (wdat->app_dat.tick_put_curr) )
	if( ( ntick - otick_put_curr[id] ) > (15*1000) )	//定时15s发一遍电流给服务器
	{
		dt = &(guide_dat->order_info->sock[id].start);
		prt_date_time( dt,"start  " );

		dt = &(guide_dat->order_info->sock[id].end);
		prt_date_time( dt,"end    " );
		goto __report_curr;

	}
	return;
	
__report_curr:

	PRT_LOG("guide_dat->rtd_num->sock[%d] = %d\r\n",id,guide_dat->rtd_num->sock[id]);
	sock_mq->put_event( pssock->order.ddh,_event_curr,com,0,guide_dat->rtd_num->sock[id],pssock->average_curr,PNULL );
	guide_dat->rtd_num->sock[id]++;

	//每获取16个电流数据存储一次
	if( 0 == ( guide_dat->rtd_num->sock[id] & 0xF ) )
		wdat_opt->rtd_num.set_to_fdb	( guide_dat->rtd_num);			//序号
	
	//更新基础电流
	pssock->base_curr = pssock->average_curr;
	
	//因为已经传了一次电流，推迟 定时 传一次电流值 给服务器 的时间
	otick_put_curr[id] = ntick;
	
	return;
	
}

static void recv_hlw_event( int id,int working )
{
	static uint32_t prt_cntr[MaxSock] = {0,0};
	
	sock_sta_pt pssock = &(socks[id]);
	
	hlw_pul_t hlw_event;
	double dval = 0;
	int segment = 0;
	int ret = sock_hlw_opt->get_cyc_pul( id,&hlw_event );
	if( ret <0 ) return;
	
	time_t ntick = get_sys_ticks();									//描述当前系统滴答
	
	//如果非工作中,丢弃数据
	if( working == 0 ) return;
	
	
	switch( hlw_event.type )
	{
		//拔掉插座
		case _type_hlw_sock_out:
			
			if( pssock->cntr_out == 0 )
			{//说明收到了插座低功耗事件，但是还需要进一步确认
				pssock->tick_out = ntick;
				pssock->cntr_out++;
			}
			else
			{//说明
				if( ( ntick - pssock->tick_out ) < ( 10*1000 ) )
				{//10秒之内产生的，说明真的拔出了
					pssock->sock_out = 1;
				}
				else
				{
					pssock->tick_out = ntick;
					pssock->cntr_out++;
				}
			}
			
			
				
			break;
		
		//周期脉冲
		case _type_hlw_cyc_pul:
		
			if( hlw_event.val > hlw_event.cyc )
			{//说明有功率消耗
				pssock->cntr_out = 0;
			}
			
		
			//加到总电能去
			guide_dat->total_e->pul[id] += hlw_event.val;
		
			//得到是哪一个时间段
			segment = get_time_segments( guide_dat->time_slot->val,guide_dat->time_slot->segments,rtc_dt.Hour,rtc_dt.Minute );
			//把电能加到订单信息去
			guide_dat->order_info->sock[id].pluse[ segment ] += hlw_event.val;
			
			//得到本次周期的电能消耗 W*s
			dval = hlw_event.val *  wdat->storage->k[id] + wdat->storage->b[id];
			//得到本周期的平均功率 W
			pssock->average_pwr = dval / hlw_event.cyc;
			pssock->average_curr = pssock->average_pwr/220;
			//得到平均每秒脉冲数
			dval = hlw_event.val;
			dval = dval / hlw_event.cyc;
			//sysprt->talog("id=%02d,average_curr %f \r\n",id,average_curr[ id ] );
			sysprt->alog("==========================================\r\n");
			sysprt->alog("  [%.5f]---%8d\r\n",dval,prt_cntr[id]);
			sysprt->alog("  sock[%2d]curr = [%.5f]A \r\n",id,pssock->average_curr);
			sysprt->alog("  插座[%2d]电流 = [%.5f]A \r\n",id,pssock->average_curr);
			sysprt->alog("  -->%.5f W [%d]\r\n",pssock->average_pwr,coder->cat_val( id ) );
			

			
			sysprt->alog("==========================================\r\n");
			prt_cntr[id]++;
			
			break;
		
		//一分钟脉冲(消耗电量)
		case _type_hlw_min_pul:
			pssock->min_w_s = hlw_event.val *  wdat->storage->k[id] + wdat->storage->b[id];
			
			//存储数据	
			wdat_opt->total_e.set_to_fdb		( guide_dat->total_e);				//总电能
			wdat_opt->order_info.set_to_fdb	( guide_dat->order_info);			//区间电能		
			
			break;
		
		default:
			break;
	}
}


enum {
	_e_wait_order=0,		//等待订单
	_e_order_start,			//订单开始
	_e_charging,				//充电中
	
};

//循环作业
static void cycle_work( int id )
{
	int group = 0;				//用于描述区间  属于哪一个组( JFPG )
	int segments;					//描述区间段
	double kwh;						//用于存储用电量
	
	slot_t use_period;		//用电区间
	
	slot_t usr_period[2];	//跨越天数的
	
	uint32_t overlap=0;		//重叠时间
	
	uint32_t tmin0 =0,tmin1=0;
	
//	static time_t otick_cr[ MaxSock ];				//用于控制电流/功率的访问频率
	period_e_t period_e[MaxPeriodE];						//用于存储结束订单时，区间电能
	
	
	sock_sta_pt pssock = &(socks[id]);
	

	time_t ntick = get_sys_ticks();									//描述当前系统滴答
//	double curr = 0.00;															//用于存储当前电流
	switch( pssock->ssta )
	{
		case _e_wait_order://等待订单
			//在此处什么都不用做，收到订单后会修改ssta[id]的值
			recv_hlw_event( id,0 );
			
			break;
		case _e_order_start://开始订单
			open( id );						//接通插座
			rst_check_hight( id );		//清空高电流检测
			pssock->ssta++;						//
			break;
		case _e_charging://充电中 1
			//-----------------------------------------------------
			//定时采样电流，求平均值，并上传到服务器
			cycle_listen_curr(id);
			
			recv_hlw_event( id,1 );
			//-----------------------------------------------------
			//检测是否插入插座
			if( (ntick - pssock->otick_order)>(40*1000) )
			{
				if( pssock->sock_out )
				{
					sysprt->alog("sock[%d] oeb_no_ins \r\n");
					//结束本次订单
					pssock->ssta = 0xE0+oeb_no_ins;	
					//请求传输一个警告给服务器
					
					sock_mq->put_event( pssock->order.ddh,_event_recore,id,oeb_no_ins,0,0,PNULL );
					break;
				}
			}
			else
			{
				pssock->sock_out = 0;	//前面40s就不检测插座状态了
			}
		
		
			//-----------------------------------------------------
			//10秒 后检测电流是否过大，如果过大，则结束订单
			//功率过大，则断掉订单

			if( (1 == check_hight_event( id,ntick ) )||( pssock->average_pwr >= 900 ) )
			{
				
				
				sysprt->alog("sock[%d] oeb_curr_too_big \r\n");
				//结束本次订单
				pssock->ssta = 0xE0+oeb_curr_too_big;	
				//请求传输一个警告给服务器
				sock_mq->put_event( pssock->order.ddh,_event_recore,id,oeb_curr_too_big,0,0,PNULL );
				break;
			}
			
			//-----------------------------------------------------
			//检测电能消耗是否过小 
			if( (ntick - pssock->otick_order)>(120*1000) )
				if( pssock->min_w_s < (15*60)  )
				{
					sysprt->alog("sock[%d] min_w_s is to small->%f\r\n",pssock->min_w_s);
					//电流确实太小
					pssock->ssta = 0xE0+oeb_curr_too_small;	
					//请求传输一个警告给服务器
					sock_mq->put_event( pssock->order.ddh,_event_recore,id,oeb_curr_too_small,0,0,PNULL );
					break;
				}
			
			//-----------------------------------------------------
			//检测保险丝是否烧坏
			if( 1 == cat_fuse_err(  id ) )
			{//保险丝损坏，报错
				pssock->ssta = 0xE0+oeb_fuse_broken;
				
			}
			//-----------------------------------------------------
			//检测订单超时
			
			tmin0 = datetime_to_minutes( guide_dat->order_info->sock[id].end );
			tmin1 = datetime_to_minutes( rtc_dt );
			if( tmin1>= tmin0  )
//			if( ntick >= pssock->dtick_order )
			{//成立,说明订单结束
				pssock->ssta = 0xE0+oeb_time;
				break;
			}
			//-----------------------------------------------------
			//存在订单结束请求
			if( pssock->order_stop_req )
			{//
				pssock->order_stop_req = 0;
				pssock->ssta = 0xE0+oeb_manual_stop;
				break;
			}
			
			//-----------------------------------------------------
			//定时 刷新显示
			display( id );
			break;
		//----------------------------------------------------
		case 0xE0+oeb_time:									//充电时间耗尽结束
		case 0xE0+oeb_curr_too_small:				//充电过程电流异常(拔掉/充满)		
		case 0xE0+oeb_curr_too_big:					//充电过程电流异常(拔掉/充满)		
		case 0xE0+oeb_no_ins:								//充电过程插座被拔掉
		case 0xE0+oeb_fuse_broken:					//充电过程保险丝异常
		case 0xE0+oeb_manual_stop:					//手动结束订单，请等待电量结算
			sysprt->taerr("id=%02d,event:%02d \r\n",id,pssock->ssta-0XE0 );
			
			//关闭插座
			close( id );											
			
		
		
			//计算插座用电情况
			for( int i=0;i<MaxPeriodE;i++ ) 
			{
				period_e[i].kwh = 0;
				period_e[i].min = 0;
			}
			group = 0;
			segments = guide_dat->time_slot->segments;
			kwh = 0;
			for( int seg=0; seg < segments; seg++ )
			{
				group = guide_dat->time_slot->val[ seg ].group;
				if( group < MaxPeriodE )
				{
					kwh = guide_dat->order_info->sock[id].pluse[ seg ];
					kwh = kwh * wdat->storage->k[id] / 3600000; // 功率/60秒/60分/1000W
					period_e[ group ].kwh += kwh ;
				}
			}
			
			//得到订单终止时间
			guide_dat->order_info->sock[id].end 		= rtc_dt;
			//标记订单已经结束
			guide_dat->order_info->sock[id].working = __OFF;

			//计算时间段重叠时间
			
#if 1
			use_period.start = guide_dat->order_info->sock[id].start.Hour*60 + guide_dat->order_info->sock[id].start.Minute;
			use_period.end   = guide_dat->order_info->sock[id].end.Hour*60 + guide_dat->order_info->sock[id].end.Minute;

			//如果当前日期 跟 订单日期不一样，则说明订单跨越天数了
			if( guide_dat->order_info->sock[id].end.Date != guide_dat->order_info->sock[id].start.Date )
			{
				
				use_period.end += 24* 60;
			}
			
			for (int seg = 0; seg < segments; seg++) 
			{
        overlap = calculate_overlap( guide_dat->time_slot->val[ seg ],use_period );
        PRT_LOG("%02d:%02d - %02d:%02d: %dmin\r\n",
					guide_dat->time_slot->val[ seg ].start/60, 
					guide_dat->time_slot->val[ seg ].start%60,
          guide_dat->time_slot->val[ seg ].end/60, 
					guide_dat->time_slot->val[ seg ].end%60,
          overlap);
				
				group = guide_dat->time_slot->val[ seg ].group;
				period_e[ group ].min += overlap ;
			}
#else
			//跨越天数的时候，特殊处理
			
			
#endif

			
			
			//提交插座工作事件
			sock_mq->put_event( pssock->order.ddh,_event_sock,	id,pssock->ssta - 0xE0 ,0,0,period_e );
			
			//改变跑灯状态为，待机状态
			led_board->mode_req( id,_e_mode_standing,NULL,NULL );
			
			//存储订单状态
			wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
			
			pssock->ssta = 0;
			break;
			
		default:
			break;
	}
}




/*---------------------------------------------------------------------*/
extern void run_deal_last_shutdown(void);

enum{
	_e_ctrl_init = 0,
	_e_wait_get_date,
	_e_cycle_work,
};

//单个插座控制
static void ctrl_one_sock( int id )
{
	int i=0;
	uint32_t stime = 0,etime=0,dtime=0;
	sock_sta_pt pssock = &(socks[id]);
	switch( pssock->msta )
	{
		case _e_ctrl_init://初始化
			
			close( id );
			pssock->otick_order = 0;		
			pssock->order_remind = 0;
	
			pssock->msta++;
			pssock->ssta = 0;
			
			break;
		
		case _e_wait_get_date://等待日期和时段获取成功，处理未结束订单
			if( ( 0 == guide_dat->time_slot->get_slot ) &&( 0 != wdat->storage->net_mode )) break;
			
			//如果存在未结束订单，则生成订单，继续工作
			if( guide_dat->order_info->sock[id].working == __ON )
			{//成立，说明存在订单未结束，继续订单
				
				PRT_LOG("Sock[%d]has working not over,auto dump order\r\n",id);
				
				prt_date_time(&rtc_dt,"rtc_dt");
				prt_date_time(&(guide_dat->order_info->sock[id].end),"end   ");
				
				stime = datetime_to_minutes( rtc_dt );
				etime = datetime_to_minutes( guide_dat->order_info->sock[id].end );
//				PRT_LOG( "" )
				if( etime>= stime ) dtime = etime - stime;
				else dtime = 0;
//				sock_mq->send_order( guide_dat->order_info->sock[id].ddh,id,oct_start_charge,etime-stime,0 );
				sock_mq->send_order( guide_dat->order_info->sock[id].ddh,id,oct_start_charge,dtime,0 );
				
			}
			
			pssock->msta++;
			pssock->ssta = 0;
			
			break;
		
		case _e_cycle_work://轮询作业
			
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
	sock_sta_pt pssock = &(socks[id]);
	
	if( (pssock->msta == _e_cycle_work)&&( pssock->ssta == _e_charging ) )
		return 1;
	else
		return 0;
}

//获取插座状态
int get_sock_sta( int road )
{
	int ret = 0;
	if( 0 == cat_fuse_err( road ) )
	{//成立，说明保险丝没烧掉。上传说明插座是否正在充电中
		if( cat_sock_working( road ) )
		{//成立，说明正在充电中
			ret = 0x02;
		}
		else
		{//否则，闲置盒子
			ret = 0x00;
		}
	}
	else
	{//否则，说明保险丝烧掉了。上传说明。
		ret = 0x40;
	}
	return ret;
}

/*---------------------------------------------------------------------*/



//插座工作控制
void sock_ctrl_work( void )
{
	for( int id=0;id<MaxSock;id++ )
	{
		//控制一个插座
		ctrl_one_sock( id );
	}
	
}


