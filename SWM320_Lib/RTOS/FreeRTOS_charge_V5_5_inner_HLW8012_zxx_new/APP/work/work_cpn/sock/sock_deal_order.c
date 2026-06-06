#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	



/*---------------------------------------------------------------------*/
//处理订单请求
void cycle_wait_ordef_req( int id )
{
	int i = 0;
	int ret = 0;
	sock_sta_pt pssock = &(socks[id]);
	if( 1 == pssock->order_remind )
	{
		pssock->order_remind = 0;
		switch( pssock->order.cmd )
		{
			//充电订单
			case oct_start_charge:
				sysprt->talog(" id=%02d:oct_start_charge [%010u] \r\n",id,get_sys_ticks());
				
				if( guide_dat->order_info->sock[id].working == __OFF )
				{//成立，说明没有工作中的订单, 开启订单
					PRT_LOG( "[A] no working order,start charge\r\n" );
					pssock->ssta = 1;		//切换状态，在 cycle_work()中起作用。
					
					//复制订单号
					for( i=0;i<MaxDDH_Len;i++ )
						guide_dat->order_info->sock[id].ddh[i] = pssock->order.ddh[i];
					
					guide_dat->order_info->sock[id].start = rtc_dt;		
					
					//计算结束时间
					guide_dat->order_info->sock[id].end   = rtc_dt;		
					addTime( &(guide_dat->order_info->sock[id].end),0,0,0,0,pssock->order.dat,0 );
					
					//清空区间电能计算
					for( i=0;i<MaxSolt;i++ )
						guide_dat->order_info->sock[id].pluse[i] = 0;
					
					//标记工作中
					guide_dat->order_info->sock[id].working = __ON;
					
					//从2开始传数据
					guide_dat->rtd_num->sock[id] =  2;
					
					//存储订单信息
					wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
					
					//存储序号信息
					wdat_opt->rtd_num.set_to_fdb( guide_dat->rtd_num );
					PRT_LOG("guide_dat->rtd_num->sock[id] = %d\r\n",guide_dat->rtd_num->sock[id]);
					
					//记录:订单开始时间
					pssock->otick_order = get_sys_ticks();
					
					//记录:得到订单总的时长
					uint32_t stime = datetime_to_minutes( guide_dat->order_info->sock[id].start );
					uint32_t etime = datetime_to_minutes( guide_dat->order_info->sock[id].end );
					pssock->atime_order = etime - stime;	

					pssock->average_pwr = 0;
					pssock->average_curr = 0;
					pssock->cntr_out = 0;

					//记录: 订单没有结束请求
					pssock->order_stop_req = 0;
				}
				else
				{//否则，说明有订单在工作中
					
					
					ret = strlib->strcmpn( guide_dat->order_info->sock[id].ddh,pssock->order.ddh,strlen(pssock->order.ddh) );
					if( 0 == ret )
					{//说明订单号不一样,不能更改当前订单,因为区间电能很有可能被清空掉,不允许服务器做这个事
						PRT_LOG( "[B] has working order,ddh is not same,no to do\r\n" );
						sysprt->aerr( "Err:ddh is no same,can't change sock[%d],ddh err \r\n",id);
						return;
					}
					else
					{//说明订单号相同,服务器需要修改订单
						PRT_LOG( "[C] has working order,ddh is same, change time\r\n" );
						pssock->ssta = 1;		//切换状态，在 cycle_work()中起作用。
						
						//计算结束时间
						guide_dat->order_info->sock[id].end   = rtc_dt;	
						addTime( &(guide_dat->order_info->sock[id].end),0,0,0,0,pssock->order.dat,0 );
						
						
						
						//存储订单信息
						wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
						
						//记录:订单开始时间
						pssock->otick_order = get_sys_ticks();			
						
						//记录:得到订单总的时长
						uint32_t stime = datetime_to_minutes( guide_dat->order_info->sock[id].start );
						uint32_t etime = datetime_to_minutes( guide_dat->order_info->sock[id].end );
						pssock->atime_order = etime - stime;

						//记录: 订单没有结束请求
						pssock->order_stop_req = 0;
						
						pssock->average_pwr = 0;
						pssock->average_curr = 0;
						pssock->cntr_out = 0;
						
					}
					
				}
				break;
				
			//取消充电	
			case oct_cancel_charge://
//				pssock->dtick_order = get_sys_ticks();
				
//				//计算结束时间
//				guide_dat->order_info->sock[id].end   = rtc_dt;		//当前时间等于结束时间	
//				
//				//存储订单信息
//				wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
				//记录: 订单没有结束请求
				pssock->order_stop_req = 1;	
				
				break;
			
			default:
				break;
		}
	}
}
/*---------------------------------------------------------------------*/
//等待网络订单
void wait_net_order( void )
{
	order_t torder;
	sock_sta_pt pssock;
	uint8_t id = 0;
	int i = 0;
//	int ret = 0;
	if( sock_mq->recv_order( &torder ) >= 0  )
	{
		
		sysprt->aerr( "recv_order :\r\n");
		sysprt->aerr( "\tddh   :%s\r\n",torder.ddh );
		sysprt->aerr( "\tnum   :%d\r\n",torder.num );
		sysprt->aerr( "\tcmd   :%d\r\n",torder.cmd );
		sysprt->aerr( "\tdat   :%d\r\n",torder.dat );
		sysprt->aerr( "\tparam :%d\r\n",torder.param );
		
		//终端号转控制id
		id = torder.num ;
		
		if( id >= MaxSock )
		{
			sysprt->aerr( "recv_order id(%d) > MaxSock \r\n",id );
			return;
		}
		pssock = &(socks[id]);
		
		//先看是否为结束订单命令，如果结束的不是当前订单，不用理会
		if( torder.cmd == oct_cancel_charge  )
		{
			if( 0 == strlib->strcmpn( pssock->order.ddh,torder.ddh,strlen( torder.ddh )  ) )
			{//说明订单号不一样,不能结束当前订单
				sysprt->aerr( "Err:oct_cancel_charge sock[%d],ddh err \r\n",id );
				return;
			}
		}
		//按id进行订单分发
		
		pssock->order_remind = 1;
		sysprt->aerr( "recv_order id->%d \r\n",id );
		for( i=0;i<MaxDDH_Len;i++ )
			pssock->order.ddh[i] = torder.ddh[i];
		
		pssock->order.num = torder.num;
		pssock->order.cmd = torder.cmd;
		pssock->order.dat = torder.dat;
		pssock->order.param = torder.param; 
		
	}
}

