#include "framework.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define PRT_LOG(X...)	sysprt->alog(X)	

/*-------------------------------------------------------------------------*/
static int regiser_one( cmd_util_pt cmd );



/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*命令行需要在此处注册,才能正常使用*/
static void init(void)
{
	extern void register_base_cmd(void);

	extern void register_debug_cmd(void);
	
	/*注册命令行*/
	register_base_cmd();
	/*扩展例程*/
	register_debug_cmd();
	
}
/*-------------------------------------------------------------------------*/

static cmd_util_pt m_cmd_map_head = NULL;
/*-------------------------------------------------------------------------*/
static cmd_util_pt get_head(void)
{
	/*返回表头指针*/
	return m_cmd_map_head;
}

/*-------------------------------------------------------------------------*/
//入链操作
static int regiser_one( cmd_util_pt cmd )
{
	cmd_util_pt t_cmd_util;
	/*判断链表是否为空*/
	if ( m_cmd_map_head == NULL )
	{//链表里只有表头一项
		m_cmd_map_head   = cmd;
		cmd->next = NULL;
	}
	else
	{//链表里除了表头,还有其他项，从表头开始找尾巴
		t_cmd_util = m_cmd_map_head;
		/*找出链尾*/
		while ( t_cmd_util->next != NULL)
		{
			t_cmd_util = t_cmd_util->next;
		}
		/*入链*/
		t_cmd_util->next	  = cmd;
		//切换到新的链尾
		t_cmd_util = t_cmd_util->next;
		//标记新的链尾
		t_cmd_util->next = NULL;
	}
	return 0;
}
/*-------------------------------------------------------------------------*/
//提取链表一项数据(按名称)
static cmd_util_pt get_by_val( int val )
{
	return NULL;
}
/*-------------------------------------------------------------------------*/
//提取链表一项数据(按名称)
static cmd_util_pt get_by_name( char* name )
{
	cmd_util_pt t_cmd_util = m_cmd_map_head;
	//printf("you want to Get the Console : %s \r\n",name);
	while ( t_cmd_util )
	{
		/*比较名称*/
		if ( strlib->strcmpn( name,t_cmd_util->name,strlen(name)) )
		{
			
			//printf("ConsoleV() return %s \r\n",t_cmd_util->name);
			/*防止出现输入"Te"时,能够执行"Test"的情况发生*/
			if(strlen(name) == strlen(t_cmd_util->name) )
				return t_cmd_util;
		}   
		//printf("ConsoleV() Dealing %s \r\n",t_cmd_util->name);
		t_cmd_util = t_cmd_util->next;
	}
	//printf("Error:ConsoleV can't find! \r\n");
	return NULL;
}

/*-------------------------------------------------------------------------*/
extern int console_work(void);
extern int boot_wait(void);
extern void quit_wait( void );
static cmdline_man_t do_cmdline_man={
	init,
	get_head,
	regiser_one,
	get_by_val,
	get_by_name,
	console_work,
	boot_wait,
	quit_wait,
};

cmdline_man_pt cmdline_man = &do_cmdline_man;
/*-------------------------------------------------------------------------*/
