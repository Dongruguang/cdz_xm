#include "project.h"

typedef struct{
	uint8_t head;		//帧头
	uint8_t cmd;		//命令码
	uint8_t id;			//玩家ID
	uint8_t rec;		//保留固定为00
	uint16_t card;	//卡片机数量
	uint16_t egg;		//扭蛋机数量
	uint8_t rec2[2];//保留固定为00
	uint8_t check;	//检验和
	uint8_t tail;		//帧尾
	
}A1_t;

typedef struct{
	uint8_t head;		//帧头
	uint8_t cmd;		//命令码
	uint8_t dat[8];	//数据内容
	uint8_t check;	//检验和
	uint8_t tail;		//帧尾
	
}frame_t;


void analysis_cmd( uint8_t* cbuf )
{
	if( cbuf[1] == 0xA1 )
	{//说明是 退蛋退票命令
		A1_t* A1_frame = (A1_t*)cbuf;
		
		//请求玩家送出卡片
		printf( "req [%d] out card %d\r\n",A1_frame->id,A1_frame->card );
		//请求玩家送出扭蛋
		printf( "req [%d] out egg  %d\r\n",A1_frame->id,A1_frame->egg );
		
		
		
		return;
	}
	
	if( cbuf[1] == 0xA2 )
	{//说明是 IO板输出脚控制
		
		//修改输出
		printf(" change out\r\n ");
		
		return;
	}	
	
	
}
