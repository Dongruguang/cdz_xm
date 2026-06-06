
#include "framework.h"
#define PRT_LOG(X...)	dprintf(X)	


at_dev_pt air724;



void init_air724(void)
{
	int ret = 0;
	int line = 0;
	int err = 0;
	char* action_name = NULL;
	
//------------------------------------------------------------------------------
//创建AT设备
	air724 = at_man->create_dev( uart1,20 );
	
//------------------------------------------------------------------------------
//自动波特率,单片机固定，AT模块自动配对
	action_name = "aoto_baudrate";
	ret = at_man->aoto_baudrate( air724,1000 );
	if( ret < 0 )
	{
		line = __LINE__;goto __AT_Init_ERROR;
	}
	task_delay(1000);
	while(1)
	{
		if( 0 == air724->uart->read_recv_buf( air724->rxbuf,1 ) )
		{
			break;
		}
		else
		{
			aprintf("%s\r\n",air724->rxbuf);
		}
	}
	
//------------------------------------------------------------------------------
//因为握手的时候发送了很多"AT"，所以这条命令无论发什么都是返回+CME ERROR: 58
	err = 0;
__CMD_AT_1st:
	action_name = "__CMD_AT";
	ret = at_man->send_cmd( air724,"AT",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_1st;
	}
//------------------------------------------------------------------------------
//读取模块厂商信息
	err = 0;
__CMD_AT_CGMI:
	action_name = "__CMD_AT_CGMI";
	ret = at_man->send_cmd( air724,"AT+CGMI",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CGMI;
	}
	
//------------------------------------------------------------------------------
//读取固件版本信息	
	err = 0;
__CMD_AT_CGMR:
	action_name = "__CMD_AT_CGMR";
	ret = at_man->send_cmd( air724,"AT+CGMR",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CGMR;
	}
//------------------------------------------------------------------------------
//查询卡是否插好
	err = 0;
__CMD_AT_CPIN:
	action_name = "__CMD_AT_CPIN";
	ret = at_man->send_cmd( air724,"AT+CPIN?",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CPIN;
	}
//------------------------------------------------------------------------------
//查询设置信号质量
	err = 0;
__CMD_AT_CSQ:
	action_name = "__CMD_AT_CSQ";
	ret = at_man->send_cmd( air724,"AT+CSQ",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CSQ;
	}
//------------------------------------------------------------------------------
//查询网络注册状态
	err = 0;
__CMD_AT_CREG:
	action_name = "__CMD_AT_CREG";
	ret = at_man->send_cmd( air724,"AT+CREG?",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CREG;
	}
	
	ret = at_man->send_cmd( air724,"\r\n",0XFFFFFFFF,2000 );
//------------------------------------------------------------------------------
#if 1
//查询附着GPRS网络

	err = 0;
__CMD_AT_CGATT:
	action_name = "__CMD_AT_CGATT";
	ret = at_man->send_cmd( air724,"AT+CGATT?\r",0XFFFFFFFF,2000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		eaprintf("\r\n(%d) ret = %d \r\n",__LINE__,ret);
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CGATT;
	}
#endif
//------------------------------------------------------------------------------
//设置自动获取APN
	err = 0;
__CMD_AT_CSTT:
	action_name = "__CMD_AT_CSTT";
	ret = at_man->send_cmd( air724,"AT+CSTT=\"\",\"\",\"\"",0XFFFFFFFF,2000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CSTT;
	}

	
//------------------------------------------------------------------------------
//激活移动场景，激活后能获取到IP
	err = 0;
__CMD_AT_CIICR:
	action_name = "__CMD_AT_CIICR";
	ret = at_man->send_cmd( air724,"AT+CIICR",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIICR;
	}

//------------------------------------------------------------------------------
//查询IP，只有获取到IP后才能上网
	err = 0;
__CMD_AT_CIFSR:
	action_name = "__CMD_AT_CIFSR";
	ret = at_man->send_cmd( air724,"AT+CIFSR",0xFFFFFFFF,3000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIFSR;
	}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//非透传发送数据发送模式-快模式
	err = 0;
__CMD_AT_CIPQSEND:
	action_name = "__CMD_AT_CIPQSEND";
	ret = at_man->send_cmd( air724,"AT+CIPQSEND=1",1000,3000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIPQSEND;
	}	
//------------------------------------------------------------------------------
//设置发送数据长度
	err = 0;
__CMD_AT_CIPSEND:
	action_name = "__CMD_AT_CIPSEND";
	ret = at_man->send_cmd( air724,"AT+CIPSEND=78",1000,3000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIPSEND;
	}
//------------------------------------------------------------------------------
//发送数据长
	err = 0;
__CMD_AT_SEND_DATA:
	action_name = "__CMD_AT_SEND_DATA";
	PRT_LOG("GET /skin2/save/fudai/riqi.php HTTP/1.1\nHOST: \nUser.Agent: \nConnection:Close\n\n");
	ret = at_man->send_cmd( air724,"GET /skin2/save/fudai/riqi.php HTTP/1.1\nHOST: \n\n",0XFFFFFFFF,5000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_SEND_DATA;
	}



	
//------------------------------------------------------------------------------
//关闭连接
	err = 0;
__CMD_AT_CIPCLOSE:
	action_name = "__CMD_AT_CIPCLOSE";
	ret = at_man->send_cmd( air724,"AT+CIPCLOSE=0",1000,3000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIPCLOSE;
	}
//------------------------------------------------------------------------------
	return;	
//------------------------------------------------------------------------------

//错误处理
__AT_Init_ERROR:
	
	eaprintf("\r\n%s (%d) __AT_Init_ERROR: %s \r\n",__FILE__,line,action_name);
	
	return;
}

static void clr_recv_buf( at_dev_pt dev )
{
	uint8_t* rxbuf = dev->rxbuf;							//接收缓冲区指针，指针会被修改
	//先把接收缓冲区清空
	while( 1 )
	{
		if( 0 == dev->uart->read_recv_buf( rxbuf,1 ) )
		{
			//PRT_LOG("clr uart rxbuf\r\n");
			break;
		}
	}
}


//发送字符串
static void send_str(at_dev_pt dev,char *string)
{
	uint32_t ptr=0;
	while ( string[ptr] != '\0' )
	{
		dev->uart->send( (uint8_t*)(&string[ptr]),1 );
		ptr++;
	}
}

/*

	创建TCP连接

*/
static int create_tcp( at_dev_pt dev )
{
	int err = 0,ret = 0,line = 0;
	
	uint32_t err_tick = 0;										//描述错误滴答
	uint32_t old_tick = 0,now_tick = 0;				//tick 对						
	uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len
	uint8_t* rxbuf = dev->rxbuf;							//接收缓冲区指针，指针会被修改
	uint32_t cto = 3000;
	
	char * action_name = "__CMD_AT_CIPSTART";
	//创建TCP连接
	err = 0;
	//清空接收缓冲区
	clr_recv_buf( dev );
	//发送命令
	send_str( dev,"AT+CIPSTART=\"TCP\",\"14.116.153.234\",80" );
	//发送回车
	send_str( dev,"\r\n" );

__again:	
	//准备读数据
	dev->rxlen = 0;
	err_tick =0;
	old_tick = get_sys_ticks();
	
	while(1)
	{
		//读数据到dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );
		dev->rxlen += rd_len;
		
		//本轮没有收到任何数据
		if( rd_len == 0  ) 
		{
			ret = strlib->findstr( (char*)rxbuf,"\r\n");			
			if( ret > 0 )
			{//说明收到了回车符号
				if( 0 < strlib->findstr( (char*)rxbuf,"OK" ) )
				{//说明收到OK
					PRT_LOG("TCP create success \r\n");
					return 1;
				}
				if( 0 < strlib->findstr( (char*)rxbuf,"ERROR" ) )
				{//说明收到ERROR
					PRT_LOG("TCP create error(%d) \r\n",err);
					if( err<5 ) err++;
					else
						return 0;
				}	
			}
			
			now_tick = get_sys_ticks();
			if( cto < ( now_tick - old_tick ) )
			{//说明命令接收超时了
				return -2;
			}
			continue;
		}
		err_tick = 0;
	}
	
}


