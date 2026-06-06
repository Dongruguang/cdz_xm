#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	


//重启之前，调用此程序
void save_order_them_shutdown( void )
{
//	int change = 0;

//	//检查是否有插座存在订单
//	time_t now_tick = get_sys_ticks();
//	for( int id = 0;id<MaxSock;id++ )
//	{
//		if( cat_sock_working( id ) )
//		{//说明正在充电
//			PRT_LOG("[%d] create restart_order \r\n",id);
//			
//			//保存工作状态
//			wdat->last_order[id].work_code = WorkIngCode;
//			//保存订单剩余时间
//			wdat->last_order[id].tick = socks[id].dtick_order - now_tick;//socks[id].otick_order;	
//			
//			//保存订单号
//			for( int i=0;i<20;i++ )
//			{
//				wdat->last_order[id].ddh[i] = socks[id].order.ddh[id];
//			}
//			
//			PRT_LOG("\t tick = %d\r\n",wdat->last_order[id].tick);
//			PRT_LOG("\t ddh  = %s\r\n",wdat->last_order[id].ddh);
//			
//			
//			change ++;
//		}
//	}
//	
//	if( change > 0 )
//	{//说明需要保存数据
//		work_data_write();
//	
//	}
}




