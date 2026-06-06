#ifndef __project_h__
#define __project_h__

#include "SWM320.h"

#define ITEM_NUM(items)  ( sizeof(items) / sizeof(items[0]) )

extern void SerialInit( uint32_t Baudrate );

typedef struct {
    unsigned char* buffer;
    unsigned int size;
    unsigned int head;
    unsigned int tail;
} loopbuf_t;

extern loopbuf_t* lb_uart0;

extern loopbuf_t* loopbuf_init(unsigned int size);
extern void loopbuf_free(loopbuf_t* lb);
extern unsigned int loopbuf_write(loopbuf_t* lb, const unsigned char* data, unsigned int n);
extern unsigned int loopbuf_read(loopbuf_t* lb, unsigned char* data, unsigned int n);


extern void cmd_line_work( void );



#endif
