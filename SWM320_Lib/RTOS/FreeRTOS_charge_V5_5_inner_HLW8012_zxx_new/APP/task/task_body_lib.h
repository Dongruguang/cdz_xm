
#define task_handle_t TaskHandle_t

typedef struct{

	task_handle_t* handle;
	char* name;				//任务名称
	char* cn_name;		//中文名称
	int stack_size;		//堆栈尺寸
	int priority;			//任务优先级
	void (*create)(void* argc,void** argv);			//创建
	void (*del)(void);													//删除
	void (*suspend)(void);											//暂停
	void (*recovery)(void);											//恢复
}task_body_t,*task_body_pt;

#define __task_body_quote_to( name ) \
	task_body_pt name = &m_task_body;

#define __task_body_quote( name ) \
	extern task_body_pt name;


#define __task_body_start \
	static task_handle_t handle;							 		\
	static void task_entry(void *arg);						\
	static void create(void* argc,void** argv); 

#define __task_body_end \
	static task_body_pt task_body = &m_task_body; \
	static void create(void* argc,void** argv) \
	{ \
		xTaskCreate( \
			task_entry, 												/*入口函数*/ \
			(const char *)task_body->name, 			/*任务名称*/ \
			task_body->stack_size,							/*堆栈大小*/ \
			NULL,																/*输入参数*/ \
			task_body->priority,								/*优先级*/   \
			task_body->handle										/*任务句柄*/ \
		); \
	}
	


