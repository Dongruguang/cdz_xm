

typedef struct{
	uint32_t size;
	uint32_t max;
	uint32_t head;
	uint32_t rear;
	uint32_t* array;
}rsQCB_t,*rsQCB_pt;

typedef struct{
	rsQCB_pt ( *create )( int size, int max,char* name );
	int ( *write )( rsQCB_pt QCB,void* dat );
	int ( *read  )( rsQCB_pt QCB,void* dat );
}rsQueue_t,*rsQueue_pt;


