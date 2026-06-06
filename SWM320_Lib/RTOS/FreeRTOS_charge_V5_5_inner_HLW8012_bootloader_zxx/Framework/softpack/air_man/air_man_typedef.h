


typedef struct{
	char* name;						//AT设备的名字
	uart_pt uart;					//绑定了什么串口
	uint8_t *rxbuf;				//接收缓冲区
	uint32_t rxlen;				//接收数据长度
	
	//匹配波特率
	int (*auto_baudrate)( void* airdev,int argc,char** argv );
	//AT命令
	int (*cmd_AT)				( void* airdev,int argc,char** argv ) ;
	int (*cmd_AT_CGMI)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CGMR)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CPIN)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CSQ)		( void* airdev,int argc,char** argv );
	int (*cmd_AT_CREG)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CGATT)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CSTT)	( void* airdev,int argc,char** argv );
	
	int (*cmd_AT_CIICR)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CIFSR)	( void* airdev,int argc,char** argv );
	
	int (*cmd_AT_CIPMODE)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CIPMUX)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CIPSTART)( void* airdev,int argc,char** argv );
	int (*cmd_AT_CIPCLOSE)( void* airdev,int argc,char** argv );
	
	int (*cmd_AT_RESET)( void* airdev,int argc,char** argv );
	
	
//	int (*cmd_AT_CIPQSEND)( void* airdev );

	
	
	
}air_dev_t,*air_dev_pt;

typedef struct{
	air_dev_pt (*create_dev)( uart_pt uart );
	
	void (*delete_dev)( air_dev_pt dev	);

	
	void (*send_str)(air_dev_pt dev,char *string);
	void (*clr_rxbuf)( air_dev_pt dev );
	void (*ptf_rxbuf)( air_dev_pt dev , int hex );
	
	
}air_man_t,*air_man_pt;

