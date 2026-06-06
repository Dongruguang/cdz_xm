#include "project.h"
#include <string.h>

// \r 0x0D
// \n 0x0A

static int split_string_n(int max,char *str, char **result, const char *delimiters);

typedef int (*deal_cmd_t)(int argc,char** argv);

typedef struct{
	char* name;					//字符串
	deal_cmd_t entry;		//操作函数

}cmd_t;

//test/clear/help/reboot

int cmd_test( int argc,char** argv ) 
{
	printf( "%s\r\n",__func__ );
	
	for( int i=0;i<argc;i++ )
	{
		printf("argv[%d]:%s\r\n",i,argv[i] );
		
	}
	
}
int cmd_clear( int argc,char** argv ) 
{
	printf( "%s\r\n",__func__ );
}
int cmd_help( int argc,char** argv ) 
{
	printf( "%s\r\n",__func__ );
}

int cmd_reboot( int argc,char** argv ) 
{
	printf( "%s\r\n",__func__ );
}

cmd_t __cmd_list[]={
	{"test",	cmd_test},
	{"clear",	cmd_clear},
	{"help",	cmd_help},
	{"reboot",cmd_reboot},
	
};


#define MaxArgc	10		//最多支持10个输入参数
#define MaxLen 128
char rxbuf[ MaxLen ];	//接收缓冲区，存储从环形缓冲区读取出来数据
int rx_len = 0;				//描述已获取数据长度

//切割字符串
//max 最多解析几个参数
//str 需要处理的字符串
//result 存放处理的结果
//delimiters 分隔符 " \t\n\r"
static int split_string_n(int max,char *str, char **result, const char *delimiters) 
{
    int count = 0;
    char *token;
    char *saveptr; // 用于保存上下文的指针

    // 使用 strtok_r 函数拆分字符串
    token = strtok_r(str, delimiters, &saveptr);

    while (token != NULL) {
        result[count] = token; // 保存当前的 token
        count++;
				if( count>=max ) break;
        // 获取下一个 token
        token = strtok_r(NULL, delimiters, &saveptr);
    }

    return count;
}


/*
状态机实现

状态0:
	打印提示符
	


状态1:
	等回车符：如果收到的数据，不是回车符，则直接打印出来

	否则，分割字符串，解析，执行

	
	学习一个宏,获取数组的元素个数


*/
static int sta = 0;

void cmd_line_work( void )
{
	uint8_t* buf = NULL;
	
	int argc = 0;
	char* argv[MaxArgc];
	
	switch( sta )
	{
		//打印提示符
		case 0:
			printf("\r\n>");
			memset( rxbuf,0,sizeof( rxbuf ) );
			rx_len = 0;
			sta++;
			break;
		
		//等回车符
		case 1:
			
			//每次读取一个字节数据出来
			buf = (uint8_t*)( &(rxbuf[rx_len])  );
			int ret = loopbuf_read( lb_uart0,buf,1 );
			if( ret == 0 ) break;
			rx_len += ret;
			
			
		
			//需要判断缓冲区是否溢出
			if( rx_len >= MaxLen )
			{//成立，说明缓冲区满了
				sta = 0;
				break;
			}
			
			
			switch( buf[0] )
			{
				
				case 0x09://TAB键
					//检索适配命令，并打印出来提示
				
					break;
				
				case 0x7F:
				case '\b':					
					//需要先搞懂 rx_len 代表什么含义
					//rx_len 此时，代表了收到退格符之后，下一个接收数据的数组下标
					rx_len --;
					//rx_len 此时，代表了退格符的数组下标
					rxbuf[ rx_len ] = 0;	//清除退格符号
					
					//按下退格键的目的，是为了把原有的数据，清掉一个
					
					//先判断原来有没有数据
					if( rx_len == 0 )
					{//说明，原本是没有数据的
						
					}
					else
					{//说明，原本是有数据的
						rx_len--;
						//rx_len 此时,代表了原有数据的最后一个字符的数组下标
						rxbuf[ rx_len ] = '\0';	//清除原有数据的最后一个字符 
						
					}
					printf("\r\n>%s",rxbuf);
					
				
					
					break;
				
				case '\n':
					//有些串口工具发送回车换行，\n不是我们想要的
					//如果命令首部是\n，则直接丢弃。
					if( rx_len == 1 )	
						rx_len = 0;
					break;
					
				case '\r':
					argc = split_string_n( MaxArgc,rxbuf,argv," \t\n\r" );
					if( argc != 0 )
					{
						
						//在命令表里面匹配 argv[0]
						for( int i=0;i< (ITEM_NUM( __cmd_list ) );i++ )
						{
							if( 0 == strncmp( rxbuf,__cmd_list[i].name ,strlen( __cmd_list[i].name ) ) )
							{//成立，说明字符串一致，找到相应的命令了
								printf("\r\n");
								__cmd_list[i].entry( argc,argv );
								
								sta = 0;
								return;
							}

						}
						printf("\r\n\tunknow");
					}
			
					sta = 0;
		
				
					break;
				
				default:
					//直接打印出来(回显)
					printf("%c",buf[0] );
					break;
				
			}
			
			break;
		
		default:
			break;
	
	
	}
}

