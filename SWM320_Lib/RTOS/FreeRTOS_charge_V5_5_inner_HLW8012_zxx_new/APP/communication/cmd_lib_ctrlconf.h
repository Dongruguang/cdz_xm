


typedef struct{
	uint8_t sCode;					//帧头
	uint8_t cCode;					//命令码
	uint8_t module;					//模块
	uint8_t action;					//行为
	uint8_t arg[6];					//数据块
	uint8_t sum;						//校验和
	uint8_t eCode;					//帧尾
}cmd_ctrl_conf_t,*cmd_ctrl_conf_pt;
enum{
	_ctrl_conf_outputbus = 0,
	_ctrl_conf_intputbus,
};

