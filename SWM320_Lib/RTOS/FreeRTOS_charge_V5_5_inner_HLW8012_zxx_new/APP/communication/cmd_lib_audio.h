
typedef struct{
	uint8_t sCode;					//帧头
	uint8_t cCode;					//命令码
	uint8_t dev;						//设备
	uint8_t action;					//行为
	uint8_t arg[6];					//数据块
	uint8_t sum;						//校验和
	uint8_t eCode;					//帧尾
}cmd_audio_t,*cmd_audio_pt;
enum{
	_audio_init_dev = 0,
	_audio_set_volume,
	_audio_set_gingle_cycle,
	_audio_set_play_now,
	_audio_set_play_music,
};




