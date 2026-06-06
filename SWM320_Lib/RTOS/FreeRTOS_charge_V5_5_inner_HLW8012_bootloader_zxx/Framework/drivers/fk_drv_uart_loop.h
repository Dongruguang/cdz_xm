
/*--------------------------------------------------------------------------------*/
//»·ÐÎ»º³åÇø
#define Loop_MAXBUFLEN	(1024)
#if (Loop_MAXBUFLEN>1024)
#error "Rx buffer must be less than 512 bytes because of head/rear data size is 8-bit unsigned char."
#endif
typedef struct{

	int head;
	int rear;
	uint8_t buffer[Loop_MAXBUFLEN];
}loopbuf_t,*loopbuf_pt;

typedef struct{
	int (*read)(loopbuf_pt ploop,void *vbuf,unsigned char len);
	void (*write)(loopbuf_pt ploop,uint8_t dat);
	loopbuf_pt (*create)(void);
}loop_t,*loop_pt;
extern loop_pt loop;

