
/*自动初始化模块*/
/*来源于rt-thread 系统源码*/
#define RT_SECTION(x)               __attribute__((section(x)))
#define RT_UNUSED                   __attribute__((unused))
#define RT_USED                     __attribute__((used))

typedef int (*init_fn_t)(void);
#ifdef _MSC_VER /* we do not support MS VC++ compiler */
#pragma section("rti_fn$f",read)
        struct rt_init_desc
        {
            const char* level;
            const init_fn_t fn;
        };
        #define INIT_EXPORT(fn, level)                                  \
                                const char __rti_level_##fn[] = level"__rt_init_"#fn;   \
                                __declspec(allocate("rti_fn$f"))                        \
                                RT_USED const struct rt_init_desc __rt_init_msc_##fn =  \
                                {__rti_level_##fn, fn };
				
#else
        #define INIT_EXPORT(fn, level)                                                       \
            RT_USED const init_fn_t __rt_init_##fn RT_SECTION(".rti_fn." level) = fn
#endif
				
/* board init routines will be called in board_init() function */

																
#define INIT_BOARD_EXPORT(fn)           INIT_EXPORT(fn, "1")

/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initilization) */
#define INIT_PREV_EXPORT(fn)            INIT_EXPORT(fn, "3")
/* device initialization */
#define INIT_DEVICE_EXPORT(fn)          INIT_EXPORT(fn, "5")
/* components initialization (dfs, lwip, ...) */
#define INIT_COMPONENT_EXPORT(fn)       INIT_EXPORT(fn, "7")
/* environment initialization (mount disk, ...) */
#define INIT_TASK_EXPORT(fn)             INIT_EXPORT(fn, "9")
/* appliation initialization (rtgui application etc ...) */
#define INIT_APP_EXPORT(fn)             INIT_EXPORT(fn, "11")				


extern void rt_components_init(void);
