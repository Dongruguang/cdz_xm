
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)//dprintf(X)	

//----------------------------------------
//服务器命令处理



//----------------------------------------
//轮询作业，接收并解析数据
void work_internet_recv_analysis( wk_inet_pt inet )
{
	char *ddh = NULL;
	time_t tick = 0;
	switch( inet->ssta )
	{
		//
		case 0://初始化工作数据
			PRT_LOG("work_internet_recv_analysis \r\n");
			//接收缓冲区复位
			wira_work_data_rst();
			
			//检查插座的情况，如果是关的，则请求，发送消息给服务器，用于结束异常断电导致的订单。
			//TODO_START:

			tick = get_sys_ticks();
		
			for( int id=0;id<MaxSock;id++ )
			{
				//正在充电就不用管
				if( cat_sock_working( id ) ) continue;
				
				//插座如果不是在工作中，则需要上报一下
				ddh = mem_alloc( 20 );
				
				//如果分配失败，则此功能就不用了
				if( ddh == NULL ) break; 
				
				//按照(id+i+tick)%8的方式生成订单号，让订单号看起来是随机的
				for( int i=0;i<20;i++ )	ddh[i] = ( (id+i+tick)&7 )+'0';	
				ddh[16] = 0; ddh[17] = '\0';
				
				//发送事件,根据id知道是哪个插座
				sock_mq->put_event( ddh,_event_net,id,0,0,0 );
			
				
				mem_free( ddh );
			}
			//TODO_END	
			
			
			nwdt->feed( nwdt,inet ); //喂狗
			inet->ssta++;
			
			break;
		
		case 1://与服务器交流
			//超时机制处理
			if ( nwdt->deal( nwdt,inet )<0 ) break;

			//接收数据并解析
			wira_wrok_cycle( inet );
		
			//硬件事件上报 herp
			wira_event_report();
			
			//交互事件处理
			ipool->work();
			
		
			break;
		
		default:
			break;
	}
	

}





