



/*
at dev 的设计
(1)可以创建独立的AT设备
(2)可以设置绑定一个UART
(3)一个设备可以注册多条AT指令
(4)发送命令，会根据AT命令+数据的格式，调用串口底层发送数据
(5)发送函数是阻塞的！！！
发送命令之后，会等AT设备返回
程序等到回车符号后，判断AT访问是成功还是失败
如果5s没有等到AT返回，则抛出访问错误


*/

typedef int (*cmd_cb_t)( uint8_t buf,uint32_t len );


#define cmd_is_unreg	0
#define cmd_is_reg		1


typedef struct{
	char* name;						//AT指令名字	
	uint32_t number;			//编号
	cmd_cb_t callback;		//接收回调
	char* usage;					//用法
}at_cmd_t,*at_cmd_pt;

typedef struct{
	uint32_t reg;					//是否被注册
	at_cmd_pt body;				//指向命令
}ele_t,*ele_pt;


typedef struct{
	char* name;						//AT设备的名字
	uint32_t max_cmd;			//最多支持多少个AT命令
	uart_pt uart;					//绑定了什么串口
	uint8_t *rxbuf;				//接收缓冲区
	uint32_t rxlen;				//接收数据长度
	
}at_dev_t,*at_dev_pt;

typedef struct{
	at_dev_pt (*create_dev)( uart_pt uart,uint32_t max_cmd );
	void (*delete_dev)( at_dev_pt dev	);

	//匹配波特率
	int (*aoto_baudrate)( at_dev_pt dev,uint32_t cto );			
	
	//发送命令
	int (*send_cmd)( at_dev_pt dev,char* at,uint32_t dto,uint32_t cto );
	
	//打印接收缓冲区
	void (*printf_rxbuf)( at_dev_pt dev , int hex );
	

}at_man_t,*at_man_pt;

