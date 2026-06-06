
/*========================================================================*/

#define __UseInExfile 
typedef void(*ConsoleAPP)(unsigned char argc, char *argv[]);								//

//一条命令的数据结构
//typedef struct ConsoleAction {
//	char  *name;																							//命令名称
//	char	*Usage;																							//使用方法
//	char	*Text;																							//详细描述
//	void	(*ConsoleAPP)(unsigned char argc, char *argv[]);		//命令调用
//	struct 	ConsoleAction *ptNext;														//Next 结构
//}T_ConsoleAction,*PT_ConsoleAction;
/*命令行表格管理器*/
//typedef struct ConsoleMan {
//	PT_ConsoleAction 	(*GetHead)(void);					
//	int (*Register)(PT_ConsoleAction ptConsoleAction);
//	PT_ConsoleAction (*GetByVal)(unsigned short pcVal);
//	PT_ConsoleAction (*GetByName)(char *name);
//}ConsoleMan_t;
//extern ConsoleMan_t ConsoleMan;
//extern void console_PrintCmdInfo(PT_ConsoleAction ptConsoleAction);
#define PrintHelp() {\
	PT_ConsoleAction ptConsoleAction = ConsoleMan.GetByName(argv[0]);	\
	console_PrintCmdInfo(ptConsoleAction);}


/*========================================================================*/

enum ptf_type{
	__type_stdlog__ = 0,		//标准日志
	__type_drvlog__,				//驱动日志
	__type_applog__,				//应用日志
	__type_drverr__,				//驱动错误
	__type_apperr__,				//应用错误	
};

typedef struct{
	void (*init)( void );
	void (*work)( void );
	
	void (*log )( char* format,... );
	void (*dlog)( char* format,... );
	void (*alog)( char* format,... );
	void (*derr)( char* format,... );
	void (*aerr)( char* format,... );
	
	void (*tlog )( char* format,... );
	void (*tdlog)( char* format,... );
	void (*talog)( char* format,... );
	void (*tderr)( char* format,... );
	void (*taerr)( char* format,... );
	
	//设置串口
	void (*set_uart)( uart_pt uart );
	//返回串口
	uart_pt (*port)(void);
	
	
}sysprt_t,*sysprt_pt;

typedef struct _cmd_util{
	char  *name;																							//命令名称
	char	*usage;																							//使用方法
	char	*text;																							//详细描述
	int (*app)(char argc,char** argv);
//	struct _cmd_util* last;
	struct _cmd_util* next;
}cmd_util_t,*cmd_util_pt;

typedef struct{
	void (*init)(void);																				//初始化命令管理器
	cmd_util_pt (*get_head)( void );													//获取表头
	int (*regiser_one)( cmd_util_pt cmd );										//注册一条命令
	cmd_util_pt (*get_by_val)( int val );										//根据命令号获取一条命令
	cmd_util_pt (*get_by_name)( char* name );									//根据名字获取一条命令
	int (*work)(void);
	int (*boot_wait)(void);
	void (*quit_wait)( void );															//退出等待
}cmdline_man_t,*cmdline_man_pt;








