#include "framework.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define PRT_LOG(X...)	sysprt->alog(X)	

/*-------------------------------------------------------------------------*/
static int task_clear( char argc, char **argv )
{
	unsigned char i;
	
	for (i = 0; i < 30; i++)
	{
			PRT_LOG("\r\n");
	}
	return 0;
}
cmd_util_t __cmd_clear={
	.name	 	= "clear",
	.usage 	= "clear",
	.text 	= "clear srceen,in some terminal you can use Ctrl + L!",
	.app 		= task_clear,
};
/*-------------------------------------------------------------------------*/
//打印帮助文档
void console_PrintCmdInfo( cmd_util_pt cmd_util )
{
		PRT_LOG("Command is not standardized!\r\n");
		PRT_LOG("     cmd  :%s \r\n",cmd_util->name);
		PRT_LOG("     Usage:%s \r\n",cmd_util->usage);
		PRT_LOG("     Text :%s \r\n",cmd_util->text);
}

/*-------------------------------------------------------------------------*/
static int task_help( char argc, char *argv[])
{
	unsigned char i = 1;
	cmd_util_pt t_cmd_util;
	PRT_LOG("Support Command List:\r\n");
	/*--列举所有指令------------------------*/
	t_cmd_util = cmdline_man->get_head();
	PRT_LOG("----------------------------------------------------------------------\r\n");
	while( t_cmd_util != NULL)
	{
		PRT_LOG(" %d: %s 			",i++,t_cmd_util->name);
		//PRT_LOG("     Usage:%s 		",ptConsoleAction->Usage);
		PRT_LOG("     Text :%s \r\n",t_cmd_util->text);
		PRT_LOG("----------------------------------------------------------------------\r\n");
		t_cmd_util = t_cmd_util->next;
	}
	return 0;
}

cmd_util_t __cmd_help={
	.name 	= "help",
	.usage 	= "help",
	.text 	= "Command List!",
	.app 		= task_help,
};
/*-------------------------------------------------------------------------*/
static int task_ls( char argc, char *argv[])
{
	return 0;
}
cmd_util_t __cmd_ls={
	.name 	= "ls",
	.usage 	= "ls",
	.text 	= "Command List!",
	.app 		= task_ls,
};
/*-------------------------------------------------------------------------*/
static int task_boot( char argc, char **argv)
{
//	BootTime = ArgsBootTime;
//	EnCountdown = 1;
	return 0;
}
cmd_util_t __cmd_boot={
	.name 	= "boot",
	.usage 	= "boot",
	.text 	= "boot",
	.app 		= task_boot
};


/*-------------------------------------------------------------------------*/
static int task_reboot( char argc, char **argv)
{
//	BootTime = ArgsBootTime;
//	EnCountdown = 1;
	reset_system();
//	return 0;
}
cmd_util_t __cmd_reboot={
	.name 	= "reboot",
	.usage 	= "reboot",
	.text 	= "reboot",
	.app 		= task_reboot
};

/*-------------------------------------------------------------------------*/
//TAB键的操作
void TAB_KeyDeal(void)
{
	
}
/*-------------------------------------------------------------------------*/
extern cmd_util_t __cmd_get_app1;

void register_base_cmd(void)
{
	cmdline_man->regiser_one( &__cmd_clear );
	cmdline_man->regiser_one( &__cmd_help );
	cmdline_man->regiser_one( &__cmd_ls );
	cmdline_man->regiser_one( &__cmd_boot );
	cmdline_man->regiser_one( &__cmd_reboot );
	cmdline_man->regiser_one( &__cmd_get_app1 );

}
