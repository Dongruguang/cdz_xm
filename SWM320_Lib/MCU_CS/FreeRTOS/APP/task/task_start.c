
#include "work.h"
#include "network.h"
#include "led_board.h"

extern void create_task_of_host( void );
extern void create_task_of_listen( void );
extern void create_task_of_ctrl( void );

void cslock_init(void);
void myprintf( char* format, ... );




void task_start_entry(void *arg)
{
	printf("%s\r\n",__FUNCTION__);
	
	
	//��ʼ��uart0
	uart->close( _e_uart0 );
	uart->init( _e_uart0,115200 );
	uart->open( _e_uart0 );
	
	//��ʱ1s������������뷢���쳣��оƬ�޷�ʹ��DAP-Link��¼����
	//������������գ��������쳣���ʣ�����оƬ�������޷�ʹ��DAP-Link��д����
	//��Ҫ���������BOOT��+Reset�������ɿ�Reset�������ɿ�BOOT��������ISPģʽ��
	//ʹ�ùٷ���ISP���ߣ�����(CH340)���flash�����ԾȻ���
	vTaskDelay( pdMS_TO_TICKS( 1000 ) );
	
	//��ʼ�� sysprintf
	cslock_init();
	
	work_data_load();
	
	fuse->init();
	
	led_board_opt->init();
	//led_board_opt->set_net( __ON );
	//led_board_opt->set_fuse( __ON );

	
	
	
	//������������
	create_task_of_host();
	create_task_of_listen();
	create_task_of_ctrl();
	
	while(1)
	{
		//network_entry();
		
		vTaskDelay( pdMS_TO_TICKS( 10 ) );
		
	}
	
}

static TaskHandle_t TaskHandle_start;


void create_task_of_start( void )
{
	printf("%s\r\n",__FUNCTION__);
	//��������
	xTaskCreate( task_start_entry, (const char *)"start", 1024, NULL, 2, &TaskHandle_start );
}


/*----------------------------------------------------------------------------*/


static SemaphoreHandle_t cslock;
static int flag_init = 0;				//�������Ƿ��Ѿ���ʼ����
//��ʼ����
void cslock_init(void)
{
	if( flag_init == 1 ) return;
	cslock = xSemaphoreCreateMutex();
	if( cslock == NULL )
	{//˵�����ɹ�
		printf( "err:%s\r\n",__func__ );
		while(1);
	}
	flag_init = 1;
}

void cslock_get(void)
{
	if( flag_init == 0 ) return;
	
	//��ȡcslock�����ȴ�100��tick����ʱ���򷵻ش��󣻳ɹ�������TRUE
	int ret = xSemaphoreTake( cslock,0xFFFFFFFF );
	if( ret == pdTRUE )
	{//˵�����ɹ���ȡ��
	
	}
	else
	{//ʧ��
		
	}
}

void cslock_free(void)
{
	if( flag_init == 0 ) return;
	xSemaphoreGive( cslock );
}


void sysprintf( char* format, ... )
{
	if( flag_init == 0 ) return;
	
	char buf[256];		//�����Դ�ӡ���ַ�������
	
	va_list v_args;
	va_start( v_args,format );
	(void)vsnprintf( buf,sizeof(buf),format,v_args );
	
	va_end( v_args );
	
	//��ȡ��
	cslock_get();
	//ʹ�û�����Դ,��Ϊprintf���յ��õ�uart,ָ��ͬһ��Ӳ����Դ���ü�������ͬʱʹ�ã��ǲ��еġ�
	printf("%s",buf);
	//�ͷ���
	cslock_free();
}



	
/*
AT

OK
------
AT+RESET
OK
^boot.rom.'v....'!\n
RDY

^MODE: 17,17

+E_UTRAN Service

+CGEV: ME PDN ACT 1

------
��ȡģ�鳧����Ϣ
AT+CGMI

+CGMI: "AirM2M"

OK

------
��ȡģ��Ĺ̼���Ϣ
AT+CGMR

+CGMR: "AirM2M_780EP_V1010_LTE_AT"

OK
------
��ѯ��������/SIM����û�в��
AT+CPIN?

+CPIN: READY

OK
------
��ѯ�ź�����
AT+CSQ

+CSQ: 22,0

OK
------
��ѯ�����ע��״̬
AT+CREG?

+CREG: 0,1

OK
------
��ѯ����GPRS����
AT+CGATT?

+CGATT: 1

OK

------
����͸������
AT+CIPMODE=1


OK
------
����ģ��Ϊ������
AT+CIPMUX=0

OK
------
�Զ���ȡAPN

AT+CSTT="","",""

OK
------
�����ƶ�����,����֮�󣬿���ȥ��ȡIP
AT+CIICR

OK
------
��ѯIP
AT+CIFSR

10.191.209.135
------
����TCP������
AT+CIPSTART="TCP","www.armsoc.cn","9002"

AT+CIPSTART="TCP","www.armsoc.cn","9002"

OK

CONNECT




*/
