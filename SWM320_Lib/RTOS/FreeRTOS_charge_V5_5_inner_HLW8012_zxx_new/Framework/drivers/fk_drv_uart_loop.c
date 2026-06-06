
#include "framework.h"
/*--------------------------------------------------------------------------------*/
static loop_t do_loop;

/*----------------------------------------------------------------------*/
//从环形缓冲区中读出数据
static int read(loopbuf_pt ploop,void *vbuf,unsigned char len)
{
	int i = 0;
	char *buf = (char *)vbuf;
	
	for(i=0; i<len; i++)
	{
		if(ploop->rear == ploop->head)
		{
			break;
		}
		buf[i] = ploop->buffer[ploop->rear];
		ploop->rear ++;
		if(ploop->rear >= Loop_MAXBUFLEN)
			ploop->rear = 0;
		
	}
	
	return i;
}
/*----------------------------------------------------------------------*/

static void write(loopbuf_pt ploop,uint8_t  dat)
{
	
	//printf("%c \r\n",dat);
	if( ( (ploop->head+1) & (Loop_MAXBUFLEN-1) ) == ploop->rear )
	{
		/*TODO: Add error handling here.*/
		//debug_Assert(0,DPFL,"UART0:buffer full.");
	}
	else
	{
		ploop->buffer[ploop->head] = dat;
		ploop->head++;
		if(ploop->head >= Loop_MAXBUFLEN)
			ploop->head = 0;
	}
	
}
/*----------------------------------------------------------------------*/
static void reset( loopbuf_pt ploop )
{
	ploop->head = 0;
	ploop->rear = 0;
}

/*----------------------------------------------------------------------*/

static loopbuf_pt create(void)
{
	loopbuf_pt tloop = (loopbuf_pt) mem_alloc( sizeof(loopbuf_t) );

	return tloop;
}
/*----------------------------------------------------------------------*/
static loop_t do_loop={
	.read = read,
	.write = write,
	.reset = reset,
	.create = create,
};
loop_pt loop = &do_loop;

/*----------------------------------------------------------------------*/


