
#include "message.h"
#include "network.h"

#define PRT_LOG(X...)	sysprt->alog(X)	


static char send_buf[512];

/*-----------------------------------------------------------*/

void wira_event_report( void* ndev )
{
	pool_pt pool;
	net_dev_t* netdev= (net_dev_t*)ndev;
	
	event_t tevent;
		
	if( sock_mq->recv_event( &tevent ) < 0  ) return;
	
	sysprt->aerr( "wira_event_report :");
	sysprt->aerr( "\tddh :%s",tevent.ddh );
	sysprt->aerr( "\ttype :%d",tevent.type );
	sysprt->aerr( "\tnum :%d",tevent.num );
	sysprt->aerr( "\tval :%d",tevent.val );
	sysprt->aerr( "\tdat :%d",tevent.dat );
	sysprt->aerr( "\tcurr :%f\r\n",tevent.curr );
	
	switch( tevent.type )
	{
		
		case _event_curr://电流上报
			goto _do_event_curr;
			
		case _event_sock://插座事件
			goto _do_event_sock;

		case _event_net://网络事件
			goto _do_event_net;
		
		case _event_recore://工作记录
			
			goto _do_event_recore;
		
		case _event_m_tenance://维护日志
			goto _do__event_m_tenance;
		default:
			break;
	}
	return;
	
_do_event_curr:
	//电流不用交互，直接发给服务器
	event_report_curr( netdev, &tevent );		
	return;
	
_do_event_sock:
	pool = ipool->create( 50,30*1000,&tevent );
	ipool->register_one( pool );
	return;

_do_event_net:
	pool = ipool->create( 50,20*1000,&tevent );
	ipool->register_one( pool );
	return;
	
_do_event_recore:
	//工作记录不用交互，直接发给服务器
	return;
	
_do__event_m_tenance:
	//维护记录不用交互，直接发给服务器
	event_report_m_tenance( netdev,&tevent );	
	return;
}

/*-----------------------------------------------------------*/

void show_send_data( char* send_buf )
{
	sysprt->alog( "--------------->\r\n" );
	sysprt->alog( "send:%s",send_buf );
	sysprt->alog( "len :%d\r\n",strlen( (char*) send_buf) );
}


//发送电流报文给服务器
//ddh	->订单号
//num ->插座id
//val ->报文序号
//dat ->没有使用
//curr ->电流值
void event_report_curr( void* ndev,event_pt event )
{
	int ret = 0;
	int dump_len = 0;
	char srtbuf[16];

	double kwh = 0;
	net_dev_t* netdev= (net_dev_t*)ndev;
	
	PRT_LOG("%s\r\n",__func__);
	uint8_t com = 0;		//服务器角度, 插座号
	uint8_t id = 0;			//硬件角度，插座号
	uint32_t pwr = 0;
	uint32_t stime = 0,ntime = 0;	
	
	
	com = event->num; 	//面向服务器时，当前插座号(服务器从1开始数)
	id = com - 1;				//当前插座ID 
	
	memset( send_buf,0,sizeof(send_buf) );
	
	//填充发送 缓冲区 上报情况给服务器
	
	//命令码
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"LOG all " );
	dump_len += ret;
	
	//订单号
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),event->ddh );
	dump_len += ret;
	
	//序号
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",event->dat );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;

	//当前充电口号
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",com );					
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;

	//另一个插座的状态
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",get_sock_sta( 1 - (id&0x01) ) );		//获取另一个插座的状态
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//本插座的电量
	memset( srtbuf,0,sizeof( srtbuf ) );
	kwh = guide_dat->total_e->pul[id] * wdat->storage->k[id]/3600/1000;
	sprintf( srtbuf," %.5f",kwh );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//平均电流
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %f",event->curr );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//功率
	pwr = event->curr*220;
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",pwr );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//电压
	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 220" );
	dump_len += ret;
	
	//充电时长
	stime = datetime_to_minutes( guide_dat->order_info->sock[id].start );
	ntime = datetime_to_minutes( rtc_dt );

	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",(ntime - stime) );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//回车
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"\r\n" );
	dump_len += ret;

	air_opt->send_str( netdev,send_buf);	
	show_send_data( (char*)send_buf );
		
}
/*-----------------------------------------------------------*/
//发送维护报文给服务器

void event_report_m_tenance( void* ndev,event_pt event )
{
	int ret = 0;
	int dump_len = 0;
	net_dev_t* netdev= (net_dev_t*)ndev;
	memset( send_buf,0,sizeof(send_buf) );
	
	
	switch( event->num )
	{
		case 0x00://进入更新app状态

			ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"UPDATE ok in 5\r\n" );
			dump_len += ret;
			
			air_opt->send_str( netdev,send_buf);

			show_send_data( (char*)send_buf );
		
			//请求进入更新
			task_delay(10);
			sysprt->aerr("event_report_m_tenance:req_goto_update\r\n");
			wdat->storage->boot_app = _app_update;
			req_goto_update();
			break;
		case 0x01://更新app成功，第一次启动时，告诉服务器
			ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"UPDATE ok ss 5\r\n" );
			dump_len += ret;
			air_opt->send_str( netdev,send_buf);		
			show_send_data( (char*)send_buf );

			break;
		default:
			break;
	}

	
}

/*-----------------------------------------------------------*/
//发送插座事件报文给服务器
void event_report_sock( void* ndev,event_pt event )
{
	int ret = 0;
	int dump_len = 0;
	int group = 0;
	int max = 0;
	char srtbuf[32];
	uint32_t total_time_sec = 0;
	uint32_t group_time_sec = 0;
	float group_kwh = 0;
	net_dev_t* netdev= (net_dev_t*)ndev;
	memset( send_buf,0,sizeof(send_buf) );
	
	//填充发送 缓冲区 上报情况给服务器
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"LOG over " );
	dump_len += ret;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),event->ddh );
	dump_len += ret;

	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," " );
	dump_len += ret;

	
	total_time_sec = 0;
	//填充每个组的充电电量,时长
	max = guide_dat->time_slot->groups;
	if( max> MaxPeriodE ) max = MaxPeriodE;
	for( group=0;group<max;group++ )
	{
		memset( srtbuf,0,sizeof( srtbuf ) );
		group_kwh = event->period_e[group].kwh;						//组用电 单位 千瓦时
		group_time_sec = event->period_e[group].min*60;		//组用时 单位 s
		total_time_sec += group_time_sec; 								//总用时 单位 s
		
		sprintf( srtbuf,"%.5f,%d ",group_kwh,group_time_sec );
		ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
		dump_len += ret;
	}
	
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf,"%d",total_time_sec );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	
	//订单结束原因
	switch( event->val )
	{
		case oeb_time:						ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 00" );break;
		case oeb_curr_too_small:	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 01" );break;
		case oeb_curr_too_big:		ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 02" );break;
		case oeb_no_ins:					ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 03" );break;
		case oeb_fuse_broken:			ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 04" );break;
		case oeb_manual_stop:			ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 05" );break;
		default: 
															ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 00" );break;
	}
	dump_len += ret;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"\r\n" );
	dump_len += ret;
	
	air_opt->send_str( netdev,send_buf);		
	show_send_data( (char*)send_buf );

}
/*-----------------------------------------------------------*/
