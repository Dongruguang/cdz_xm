
#include "framework.h"

#define PRT_LOG(X...)	sysprt->alog//dprintf(X)	

//填充内容
static int dump( char* buf,char* str )
{
	int index = 0;
	while( str[index] != '\0' )
	{
		buf[index] = str[index];
		index++;
	}
	return index;
}

//从一个缓冲区复制一块内容出来
static void copy_section( uint8_t* srcbuf,uint8_t* destbuf,int start,int end )
{
	int i = 0;
	for( i=start;i<end;i++ )
	{
		destbuf[ i ] = srcbuf[i];
	}
	
}

//字符串查找子串
//成功，返回字符串中子串的偏移量
//失败， 返回-1
static int findstr( char* haystack,char* needle  )
{
	char *a = NULL;
	a = strstr( haystack,needle );
	if( a < 0  )	return -1;
	return ((uint32_t)a - (uint32_t)haystack);
}

//查看有多少个子串，并将对应子串偏移量存放在offset_buf中
//haystack 	主串
//needle 		子串
//offset_buf			存放偏移量的数组，偏移量最大65535
//len							这个数组能存放多少个数据

//返回 子串的个数
static int get_substr_index( char* haystack,char* needle,int offset_buf[],int len )
{
	char *a=NULL;
	int n=0,l=0;
	l = strlen( needle );
	a = strstr( haystack,needle );
	if( a < 0  )	return 0;
	offset_buf[n] = 	((uint32_t)a - (uint32_t)haystack);
	
	while( a!=NULL )
	{
		n++;
		if( len <= n ) return n;
		a = strstr( a+l,needle );
		if( a < 0  )	return n;
		offset_buf[n] = 	((uint32_t)a - (uint32_t)haystack);
	}
	return n;
}



//截取字符串
static int intercept( char* src,char* dest,char* sstr,char* estr )
{
	//数据被本字段名称和下一个字段名称夹住，pos_field存放2个字段的起始位置
	int pos_field[2];
			
	char *pdat = dest;
	int index = 0;

	int start = 0,end = 0;

	pos_field[0] = findstr( (char*)src, sstr );
	if( pos_field[0] < 0 ) return -1; 
	
	pos_field[1] = findstr( (char*)src, estr );
	if( pos_field[1] < 0 ) return -1; 
	
	//获取字符串数据
	start =  pos_field[0] + strlen( sstr );
	end = pos_field[1] - 1;
	for( index = start; index <= end; index++  )
	{
		*pdat = src[index]; 
		pdat++;
	}
	*pdat = '\0';
	
	return 0;
}

static char* strsep(char **s, const char *ct)
{
	char *sbegin = *s;
	char *end;

	if (sbegin == NULL)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}

/*--读取一定长度的字符串进行比较-----------------------------------*/
static uint8_t strcmpn(char *str1, char *str2,unsigned char len)
{
	unsigned char i=0;
	for(i=0;i<len;i++)
	{
		if(str1[i] != str2[i])
			return 0;
	}
	
	return 1;
}

extern void url_encode(char src[],char dest[]);


static strlib_t do_strlib = { 
	dump,
	copy_section,
	findstr,
	get_substr_index,
	intercept,
	strsep,
	strcmpn,
	url_encode,
};


strlib_pt strlib = &do_strlib;






