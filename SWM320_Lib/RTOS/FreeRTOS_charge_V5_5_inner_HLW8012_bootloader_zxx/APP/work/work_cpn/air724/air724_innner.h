
//------------------------------------------------------------------------------
//匹配波特率
extern int air724_auto_baudrate( void* airdev,int argc,char** argv );

//------------------------------------------------------------------------------
//用于中止之前的不合法命令
extern int air724_cmd_AT( void* airdev,int argc,char** argv );

//------------------------------------------------------------------------------
//读取模块厂商信息
extern int air724_cmd_AT_CGMI( void* airdev,int argc,char** argv );

//------------------------------------------------------------------------------
//读取固件版本信息
extern int air724_cmd_AT_CGMR( void* airdev,int argc,char** argv );

//查询卡是否插好
extern int air724_cmd_AT_CPIN( void* airdev,int argc,char** argv );
//查询设置信号质量
extern int air724_cmd_AT_CSQ( void* airdev,int argc,char** argv );

//查询网络注册状态
extern int air724_cmd_AT_CREG( void* airdev,int argc,char** argv );

//查询附着GPRS网络
extern int air724_cmd_AT_CGATT( void* airdev,int argc,char** argv );

//设置自动获取APN
extern int air724_cmd_AT_CSTT( void* airdev,int argc,char** argv );

//激活移动场景，激活后能获取到IP
extern int air724_cmd_AT_CIICR( void* airdev,int argc,char** argv );

//查询IP，只有获取到IP后才能上网
extern int air724_cmd_AT_CIFSR( void* airdev,int argc,char** argv );

//------------------------------------------------------------------------------
//设置透传模式
extern int air724_cmd_AT_CIPMODE( void* airdev,int argc,char** argv );
//设置IP为单链接
extern int air724_cmd_AT_CIPMUX( void* airdev,int argc,char** argv );

//创建TCP/UDP连接
extern int air724_cmd_AT_CIPSTART( void* airdev,int argc,char** argv );
//关闭TCP/UDP连接
extern int air724_cmd_AT_CIPCLOSE( void* airdev,int argc,char** argv );

extern int air724_cmd_AT_RESET( void* airdev,int argc,char** argv );

