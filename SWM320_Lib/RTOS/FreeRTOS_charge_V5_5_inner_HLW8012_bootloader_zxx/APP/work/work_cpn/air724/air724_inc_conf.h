
//------------------------------------------------------------------------------
extern air_dev_pt air724;

extern void create_dev_air724(void);

extern void air724_rst(void);




//http 获取日期模块
extern char* get_date_str(void);
extern int air724_http_date_get( void* airdev );

extern char http_file_path[128];
extern void dump_http_file_path( int MainVersion,int SubVersion );



//------------------------------------------------------------------------------


