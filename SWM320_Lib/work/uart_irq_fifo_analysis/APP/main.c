#include "project.h"


/*
(1)串口按照协议要求初始化
(2)接收中断把数据存起来
	一般使用环形缓冲区，收到数据，往环形缓冲区里面写
	
	①串口初始化之前，构建环形缓冲区
	②在串口的接收中断，往环形缓冲区里面写数据
		超时接收中断
		FIFO溢出/达标中断
		
		
	
	
(3)main()函数的while(1)循环种，把收到的数据，解析出来
	使用状态机，从环形缓冲区里面把数据读出来，并解析。


*/

void rx_cmd_data( void );

int main(void)
{
	SystemInit();			//初始化系统时钟
	
	SerialInit( 115200 );			//初始化串口
   
	printf("system run...\r\n");
	

	while(1==1)
	{
			rx_cmd_data();
		
	}
}

/*
使用状态机的思维
状态1:等待帧头
状态2:等待接收完数据
状态3:判断收到的数据是否合法
	判断是否正确收到了帧尾
	判断校验和是否正确
状态4:数据是合法的，执行解析


*/
#define FrameHead 	0xA5					//固定帧头
#define FrameTail	0x5A					//固定帧尾
#define FrameLen 12					//固定帧长度
int rx_sta = 0;							//描述接收的状态
int rx_len = 0;							//描述已经接收到的数据长度
uint8_t rx_buf[ FrameLen ];	//接收缓冲区
uint32_t cntr = 0;
void rx_cmd_data( void )
{
	int ret =0,i=0;
	
	uint8_t sum = 0;
	switch( rx_sta )
	{
		//等待帧头
		case 0:
			ret = loopbuf_read(lb_uart0, rx_buf, 1);
			if( ret == 0 ) break;
			
			if( rx_buf[0] !=  FrameHead ) break;
		
			//执行到这里，说明可能收到了帧头
			rx_len++;
			rx_sta++;
			break;
		
		//等待接收完数据
		case 1:
			ret = loopbuf_read(lb_uart0, &(rx_buf[rx_len]), FrameLen - rx_len );
			rx_len += ret;
			if( rx_len < FrameLen ) break;
			
			//执行到这里，说明接收到了数据长度为 帧长度 FrameLen
			
			//①判断 rx_buf[ FrameLen - 1 ] 是否为帧尾
			if( rx_buf[ FrameLen - 1 ] != FrameTail )
			{//成立，说明接收到的数据不合法
				rx_sta = 0;
				rx_len = 0;
				break;
			}
			//②判断检验和是否正确
			sum = 0;
			for( i=0;i<9;i++ )
			{
				sum = sum + rx_buf[1+i];
			}
			
			if( rx_buf[ 10 ] != sum )
			{//成立，说明接收到的数据不合法
				rx_sta = 0;
				rx_len = 0;
				break;
			}
		
			//如果都正确，则说明命令合法，解析数据
			printf("%d\r\n",cntr++);
			analysis_cmd( rx_buf );
			rx_sta = 0;
			rx_len = 0;
			
			break;
		
		default:
			break;
		
	}	
	
}








