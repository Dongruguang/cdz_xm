



typedef struct{
	//填充内容
	int (*dump)( char* buf,char* str );
	
	//从缓冲区的某个区间复制数据
	void (*copy_section)( uint8_t* srcbuf,uint8_t* destbuf,int start,int end );
	
	//字符串查找子串
	int (*findstr)( char* haystack,char* needle  );
	
	//从字符串里面获取子串的偏移量，并返回成功获取的子串数
	int (*get_substr_index)( char* haystack,char* needle,int offset_buf[],int len );
	
	//截取字符串
	int (*intercept)( char* src,char* dest,char* sstr,char* estr );
	
	char* (*strsep)(char **s, const char *ct);
	
	//按长度比较,返回0则不相同，返回1则相同
	uint8_t (*strcmpn)(char *str1, char *str2,unsigned char len);
	
	//url
	void (*url_encode)(char src[],char dest[]);

	//切割字符串函数
	int (*split_string_n)(int max,char *str, char **result, const char *delimiters);
	
}strlib_t,*strlib_pt;


