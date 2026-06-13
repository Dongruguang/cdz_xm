
#include "work.h"


/*
work里面做什么事
(1)管理插座的作业
(2)处理网络过来的订单
(3)周期性重启设备
	freertos，滴答调度频率是1000Hz，系统滴答变量是32位的。
	最大值为0xFFFFFFFF => 大概能够工作49天
	所以，应当在49天之前，重启设备，以重置系统滴答。

*/

extern void work_sock_ctrl(void);
extern void work_deal_order(void);
extern void work_cycle_reboot(void);

void work_cycle_entry(void)
{
	//(1)管理插座的作业
	work_sock_ctrl();
	
	//(2)处理网络过来的订单
	work_deal_order();
	
	//(3)周期性重启设备
	work_cycle_reboot();
	
}


