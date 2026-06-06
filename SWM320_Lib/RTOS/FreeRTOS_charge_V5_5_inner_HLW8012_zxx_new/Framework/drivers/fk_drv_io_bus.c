#include "framework.h"


#define PRT_LOG(X...)	sysprt->dlog//dprintf(X)	

/*--------------------------------------------------------------------------------*/
/*
init时，会根据chip分配2块缓存空间，分别是ibuf和obuf。

访问屏障|    |  数据 |     |硬件控制  | 硬件属性
[cslock]|<-- |ibuf[ ]| <-- |          |<--HC165
        |    |       |     | work()   | 
[ mq   ]|--> |obuf[ ]| --> |      　　|-->HC595
        |    |       |     |          |

ibuf[]对于work()和app访问为互斥关系，
但是只有work()会修改ibuf[]里面的值，
所以只需要使用cslock，便可方便实现互斥关系，
同样的obuf[]对于work()和app访问为互斥关系，
由于work()只读obuf[]的内容，并且app可能有多个任务访问obuf[]，
采用队列同步方式，可以提高app的执行效率。

依赖结构
fk_drv_io_bus.c
|
|-fk_drv_port_io_bus.c	
|
|-byte_array.c


本文件代码排版结构

(1)ibuf指针和obuf指针
(2)实现用于修饰ibuf的结构，bit_ctrl_t do_bit_ctrl_in
		内部借助 锁 来实现互斥访问。 
(3)实现用于修饰obuf的结构，static bit_ctrl_t do_bit_ctrl_out
		内部借助队列也实现同步操作，让访问者效率最大化。
*/


/*--------------------------------------------------------------------------------*/

static uint8_t *ibuf = 0;
static uint8_t *t_ibuf = 0;
static uint8_t *obuf = 0;

/*--------------------------------------------------------------------------------*/
static thread_cslock_t cslock;
static void init_bit_ctrl_of_in(void)
{
	cslock = thread_cslock_init("bit_ctrl_of_in");	
}

static void in_set_bit( uint8_t ibit )
{
	//正常逻辑不需要此接口
}
static void in_clr_bit( uint8_t ibit )
{
	//正常逻辑不需要此接口
}
static int  in_get_bit( uint8_t ibit )
{
	uint8_t tbyte = 0;
	uint8_t chip = ibit/8;
	thread_cslock_lock(cslock,0XFFFFFFFF);
	
	tbyte = ibuf[ chip ];								
	thread_cslock_free(cslock);
	
	int val = ( tbyte >> (ibit&7) )&0x01;
	return val;
	
}


static bit_ctrl_t do_bit_ctrl_in={
	.set_bit = in_set_bit,
	.clr_bit = in_clr_bit,
	.get_bit = in_get_bit,
};
/*--------------------------------------------------------------------------------*/
typedef struct{
	uint8_t action;
	uint8_t ibit;
	uint8_t resver[2];
}apin_t,*apin_pt;
#define _action_clr  0
#define _action_set 1
static uint8_t mq_init_flag=0;				//描述本驱动程序是否被初始化

static thread_mq_t local_mq;
static void init_bit_ctrl_of_out(void)
{
	if( 1 == mq_init_flag ) return;
	local_mq = thread_mq_init("local_mq", sizeof(apin_t), 20);
	mq_init_flag = 1;
}

static void out_set_bit( uint8_t ibit )
{
	if( mq_init_flag == 0 ) return;
	apin_t apin;
	apin.action = _action_set;
	apin.ibit = ibit;
	
	thread_mq_send( local_mq,&apin, sizeof(apin_t), 0x01 );
	
}
static void out_clr_bit( uint8_t ibit )
{
	if( mq_init_flag == 0 ) return;
	apin_t apin;
	apin.action = _action_clr;
	apin.ibit = ibit;
	
	thread_mq_send( local_mq,&apin, sizeof(apin_t), 0x01 );
	
}

static int  out_get_bit( uint8_t ibit )
{
	uint8_t tbyte = 0;
	uint8_t chip = ibit/8;
	
	tbyte = obuf[ chip ];								
//	int val = (tbyte>>(ibit%8))&0x01;
	int val = (tbyte>>(ibit&(7)))&0x01;
	return val;							
	
}

static int read_mq_from_bit_ctrl( apin_pt pin )
{
	int ret = thread_mq_recv( local_mq, pin, sizeof(apin_t), 0);
	if(ret != bFALSE) 
		return 0;
	else
		return -1;
}

static bit_ctrl_t do_bit_ctrl_out={
	.set_bit = out_set_bit,
	.clr_bit = out_clr_bit,
	.get_bit = out_get_bit,
};

/*--------------------------------------------------------------------------------*/
static int chip_num = 0;
static int buf_size = 0;

//用于快速复制数据
static int buf_word_size = 0;
static uint32_t* src_copy  =0;
static uint32_t* dest_copy =0;

static void init( uint8_t chip )
{
	int i=0;
	
	init_bit_ctrl_of_in();
	init_bit_ctrl_of_out();
	//初始化IO口
	drvp_io_bus->init();
	chip_num = chip;
	//向4对齐
	buf_size = (chip_num+0x03)&0xfffffffc ;
	PRT_LOG("\r\n");
	PRT_LOG("io_bus chip_num->%d / buf_size->%d \r\n",chip_num,buf_size);
	buf_word_size = buf_size/4;
	//为buf分配空间
	ibuf = (uint8_t*)mem_alloc( buf_size );
	
	obuf = (uint8_t*)mem_alloc( buf_size );
	PRT_LOG("\tio_bus ibuf addr->0x%08x \r\n",(uint32_t)ibuf);
	PRT_LOG("\tio_bus obuf addr->0x%08x \r\n",(uint32_t)obuf);

	t_ibuf = (uint8_t*)mem_alloc( buf_size );			//存储SPI 读出的数据
	

	//提前准备好字访问指针，从SPI读出数据后，快速复制到ibuf中
	src_copy  = (uint32_t*)t_ibuf;
	dest_copy = (uint32_t*)ibuf;
	
	
	memset( ibuf, 0xFF, buf_size );				//输入端 默认全是高电平
	memset( obuf, 0, buf_size );					//输出端 默认全是低电平
	
	//彩票器上电要拉高
	byte_array->lset_bit( chip_num, obuf, 44 );
	byte_array->lset_bit( chip_num, obuf, 45 );
	byte_array->lset_bit( chip_num, obuf, 46 );
	byte_array->lset_bit( chip_num, obuf, 47 );
	
	
	
	//刚上电，将数据送到HC595上面
	//数据交换
	for( i=0;i<buf_size;i++ )
	{	
		drvp_io_bus->swap( obuf[ buf_size - 1 - i ] );
	}
	
	drvp_io_bus->update();
}



static void work( void )
{
	int i = 0;
	apin_t apin;
	//每次抓取最多5个队列
	i=5;
	while(i--)
	{
		//从队列获取数据
		if( read_mq_from_bit_ctrl( &apin ) >= 0 )
		{
			switch( apin.action )
			{
				case _action_clr: 
					byte_array->lclr_bit( chip_num, obuf, apin.ibit ); 
					PRT_LOG(" byte_array->lclr_bit( chip_num, obuf, %d ) \r\n",apin.ibit);
					break;
				case _action_set:
					byte_array->lset_bit( chip_num, obuf, apin.ibit ); 
					PRT_LOG(" byte_array->lset_bit( chip_num, obuf, %d ) \r\n",apin.ibit);

					break;
				default:
					break;
			}
//			for(i=0;i<buf_size;i++)
//				PRT_LOG("%02x ",obuf[ i ]);
//			PRT_LOG("\r\n");
		}
		else
		{
			break;
		}
	}
	//数据交换
	drvp_io_bus->load();
	for( i=0;i<buf_size;i++ )
	{	
		t_ibuf[i] = drvp_io_bus->swap( obuf[ buf_size - 1 - i ] );
	}
	//复制读到的数据到ibuf
	
	thread_cslock_lock(cslock,10);
	for( i=0;i<buf_word_size;i++ )
	{	
		dest_copy[i] = src_copy[i] ;
	}
	thread_cslock_free(cslock);
	
	drvp_io_bus->update();
}

static io_bus_t do_io_bus = {
	.init = init,
	.in = &do_bit_ctrl_in,
	.out = &do_bit_ctrl_out,
	.work = work,
};

io_bus_pt io_bus = &do_io_bus;


