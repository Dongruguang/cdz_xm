#include "framework.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define PRT_LOG(X...)	sysprt->alog(X)	

extern cmdline_man_pt cmdline_man;

#define debug_uart uart0

#define MAX_ARG_NUM 				5							//最大支持输入参数个数
#define MAX_OneLineLen			512						//
#define PrintConsole_Args		0							//是否打印输入参数内容

static uint8_t line[MAX_OneLineLen];					//行缓冲
static uint8_t Cursor;							//描述控制台当前光标位置


/*-----------------------------------------------------------------------*/
#if 0
int split_string(char *str, char **result, char *delimiters) {
    int count = 0;
    char *token;

    // 使用 strtok 函数拆分字符串
    token = strtok(str, delimiters);

    while(token != NULL) {
        result[count] = token;
        count++;
        token = strtok(NULL, delimiters);
    }
		
    return count;
}
#endif
//int split_string(char *str, char **result, const char *delimiters) {
//    int count = 0;
//    char *token;
//    char *saveptr; // 用于保存上下文的指针

//    // 使用 strtok_r 函数拆分字符串
//    token = strtok_r(str, delimiters, &saveptr);

//    while (token != NULL) {
//        result[count] = token; // 保存当前的 token
//        count++;
//        // 获取下一个 token
//        token = strtok_r(NULL, delimiters, &saveptr);
//    }

//    return count;
//}
/*-----------------------------------------------------------------------*/
//返回0 说明没有这条指令
//返回非0 说明是argv[0]的长度
static unsigned char process_one_line(char *cmd_line)
{
	cmd_util_pt t_cmd_util;
	unsigned char argc;
	static char * argv[MAX_ARG_NUM];
	char *pline;
	char *ret;
	
//	argc = strlib->split_string( cmd_line, argv," \t\n\r");
	argc = strlib->split_string_n( MAX_ARG_NUM,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;

	/*  检索指令并处理  */
	t_cmd_util = cmdline_man->get_by_name( argv[0] );
	if( t_cmd_util != NULL)
		t_cmd_util->app(argc,argv);
	return strlen(argv[0]);
}
/*-----------------------------------------------------------------------*/
int BootTime = 0;					//启动倒计时长度
int EnCountdown = 0;	//=1 使用开机倒计时,倒计时期间,可输入回车键,暂停系统启动

static time_t otick_down=0,ntick_down = 0;
/*
//返回值
0	:说明没有收到指令数据
1	:说明收到指令数据
2	:说明等待超时,即将启动系统
3	:说明终止启动成功
*/


int console_work(void)
{
	unsigned char i;
	int ret = 0;
	uint8_t buf[16];
	
	static unsigned char ExKey = 0;					//扩展按键标志,用于识别方向键等按键	
	static unsigned char ExKeyState = 0;		//扩展按键状态
	
	if( 1 == EnCountdown ){
		/*从串口的缓冲区里面读出数据*/
		ret = debug_uart->read_recv_buf( buf, sizeof(buf));
		if( ret <= 0 ) return 0;
		
		ret = strlib->findstr( (char *)buf,"0603\r\n" );
		if( ret >= 0 )
		{
			EnCountdown = 0;
			
		}
	}
	/*-------------------------------------------------------------*/
	/*从串口的缓冲区里面读出数据*/
	ret = debug_uart->read_recv_buf( buf, sizeof(buf));
	if (ret <= 0) return 0;
	/*-------------------------------------------------------------*/
	/*如果存在可读数据*/
	for (i = 0; i < ret; i++)
	{
		debug_uart->send( &buf[i], 1);						//发送到终端
		//PRT_LOG("\r\n								KeyVal = 0x%02X \r\n",buf[i]);
		/*--识别扩展键-----------------------------------*/
		if(ExKey)
		{
			if(ExKeyState==0){
				switch(buf[i])
				{
					/*--方向键----------------*/
					case 0X5B:{
						ExKeyState = 1;
						break;
					}
					/*--ESC-------------------*/
					case 0X0D:{
						ExKeyState = 0;
						ExKey = 0;
						/*新的命令行*/
						Cursor = 0;
						PRT_LOG("$>");
						break;
					}
					default:{
						ExKeyState = 0;
					  ExKey = 0;
					}
				}
			}
			else{
				/*分析是什么扩展键*/
				switch(buf[i])
				{
					/*--上方向键-------------------------------*/
					case 0X41:{//PRT_LOG("Up!\r\n");

						break;
					}
					/*--下方向键-------------------------------*/
					case 0X42:{//PRT_LOG("Dn!\r\n");
						
						break;
					}
					/*--右方向键-------------------------------*/
					case 0X43:{//PRT_LOG("Right!\r\n");
						
						break;
					}
					/*--左方向键-------------------------------*/
					case 0X44:{//PRT_LOG("Left!\r\n");
						
						break;
					}
					default:{
						break;
					}
				}
				//退出扩展按键模式
				ExKeyState = 0;
				ExKey = 0;
			}
		}
		/*--普通键---------------------------------------*/
		else
		{
			switch(buf[i])
			{
				/*--退格键-----------------------------------*/
				case 0x7F:
				case '\b':{
						if(Cursor>0) Cursor--;
						line[Cursor] = '\0';
						PRT_LOG("\r\n$>%s",line);
					break;
				}
				/*--TAB键------------------------------------*/
				case 0X09:{
					//补全输入功能
					line[Cursor] = '\0';						/*结束本次输入，但光标不移动*/
					//PRT_LOG("\r\n TAB:You input is %s",line);
					cmd_util_pt t_cmd_util = cmdline_man->get_head();
					uint8_t TAB_item = 0;			/*TAB键补充项的数量*/
					uint8_t TAB_item_buf[64];	/*用于存储TAB键的第一项*/
					int iLen = 0;
					PRT_LOG("\r\n ");
					
					/*遍历链表,看是否有包含输入字符串的语句*/
					while( t_cmd_util != NULL)
					{
						if( strlib->strcmpn( (char*)line,t_cmd_util->name,Cursor) ){
							PRT_LOG("\t%s \r\n",t_cmd_util->name);										
							TAB_item ++;
							/*如果存在候选命令，则复制第一条命令*/
							if(TAB_item==1) {
								for(i=0;i<64;i++) TAB_item_buf[i] = 0;
								iLen = strlen( t_cmd_util->name);
								memcpy(TAB_item_buf,t_cmd_util->name,iLen);
							}
						}
						t_cmd_util = t_cmd_util->next;
					}
					
					if(TAB_item == 0) {					/*若没找到相关命令，提示一下*/
						PRT_LOG("No list\r\n");
					}
					else if(TAB_item==1) {			/*若只有一项，自动补充命令*/
						iLen = strlen((char *)TAB_item_buf);
						memset(line,sizeof(line),'\0');
						memcpy(line,TAB_item_buf,iLen);
						Cursor = iLen;
						line[Cursor] = 0;
						PRT_LOG("\r\n$>%s",line);
					}else{											/*若有多个候选项,不管*/
						PRT_LOG("\r\n$>%s",line);
					}
					break;
				}
				/*--扩展按键---------------------------------*/
				case 0X1B:{
					ExKey = 1;
					ExKeyState = 0;
				}
				/*--Crel+Z-----------------------------------*/
				case 0X1A:{
					//在此处添加撤销代码
					break;
				}
				/*--Crel+C-----------------------------------*/
				case 0X03:{
					//在此处添加清空输入代码
					
					break;
				}
				/*--回车键-----------------------------------*/
				case 0x0D:{
					/*  命令行输入结束  */
					line[Cursor] = '\0';
					PRT_LOG("\r\n");
					/*  处理一行命令行数据  */
					if(Cursor>0)	process_one_line( (char*)line );
					//PRT_LOG("Len = %d \r\n",LenArgv0);
					/*  复制数据到行缓冲,用于恢复上一条命令  */
					/*  新的命令行  */
					
					Cursor = 0;
					//for(i=0;i<MAX_OneLineLen;i++)	{line[i] = 0;}
					
					PRT_LOG("$>");
					break;
				}
				
				/*--非控制键---------------------------------*/
				default:{
					/*输入内容超出行缓冲*/
					if( Cursor >= sizeof(line) ) {
						PRT_LOG("\r\n Input data is too long!");
						PRT_LOG("Cursor = %d \r\n",Cursor);
						return 1;
					}else{
						line[Cursor] = buf[i];
						Cursor++;
					}
					break;
				}
			}
			/*--END_switch---------------------------------*/
		}
	}
	return 1;
}

int boot_wait(void)
{
	int ret = 0;
	uint8_t buf[16];
	while(1)
	{
		//	/*-------------------------------------------------------------*/
//	//倒计时程序
		if( 0 == EnCountdown ) break;
		
		ntick_down =  get_sys_ticks();
			
		if( 1000 > ( ntick_down - otick_down ) ) continue;
		
		otick_down = ntick_down;
		
		PRT_LOG("-> = %d \r\n",BootTime);
		
		if(BootTime>0)	{BootTime--;}
		else			
		{
			EnCountdown = 0;
			break;
		}
		
		
	}
	return 1;
}

