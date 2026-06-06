
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

static void air724_connect_internet( wk_inet_pt inet );
//------------------------------------------------------------------------------

void work_internet_work_env( wk_inet_pt inet )
{
//	static time_t otick=0,ntick=0;
	switch( inet->ssta )
	{
		case 0:
			air724_connect_internet( inet );
			inet->msta++;
			inet->ssta=0;
			
			break;
		
		default:
			break;
	}
	
}

//------------------------------------------------------------------------------

#define CMD_MaxErr	5
#define __ERR_AIR_RST( ) 	{ line = __LINE__;goto __AIR_RST; }

static void air724_connect_internet( wk_inet_pt inet )
{
	
//	char* argv[5];

	int err = 0,ret = 0,line = 0;

		
	air_dev_pt  this_dev;
	this_dev = air724;
	
	//查询IP，只有获取到IP后才能上网
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIFSR( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		task_delay(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	
	return ;
//	air_man->send_str( this_dev,"+++" );
__AIR_RST:
	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
	
	//请求复位
	inet->msta = 0;
	inet->ssta = 1;
	return;
}







