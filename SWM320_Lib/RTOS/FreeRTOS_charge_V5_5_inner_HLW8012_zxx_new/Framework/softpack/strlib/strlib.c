
#include "framework.h"

#define PRT_LOG(X...)	sysprt->alog//dprintf(X)	

//往缓冲区里面填充内容
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
//srcbuf:		数据源
//destbuf:	目标地址
//start:		开始位置
//end:			结束地址
static void copy_section( uint8_t* srcbuf,uint8_t* destbuf,int start,int end )
{
	int i = 0;
	for( i=start;i<end;i++ )
	{
		destbuf[ i ] = srcbuf[i];
	}
	
}


static char *my_strstr(const char *haystack, const char *needle) {
    // 如果 needle 为空，返回 haystack
    if (*needle == '\0') {
        return (char *)haystack;
    }

    // 遍历 haystack 字符串
    for (; *haystack != '\0'; haystack++) {
        const char *h = haystack;
        const char *n = needle;

        // 比较 haystack 中的当前字符与 needle
        while (*h == *n && *n != '\0') {
            h++;
            n++;
        }

        // 如果整个 needle 字符串匹配成功
        if (*n == '\0') {
            return (char *)haystack;
        }
    }

    // 没有找到 needle
    return NULL;
}


//字符串查找子串
//成功，返回字符串中子串的偏移量
//失败， 返回-1
static int findstr( char* haystack,char* needle  )
{
	char *a = NULL;
//	a = strstr( haystack,needle );
	a = my_strstr( haystack,needle );
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


//切割字符串
static int split_string_n(int max,char *str, char **result, const char *delimiters) 
{
    int count = 0;
    char *token;
    char *saveptr; // 用于保存上下文的指针

    // 使用 strtok_r 函数拆分字符串
    token = strtok_r(str, delimiters, &saveptr);

    while (token != NULL) {
        result[count] = token; // 保存当前的 token
        count++;
				if( count>=max ) break;
        // 获取下一个 token
        token = strtok_r(NULL, delimiters, &saveptr);
    }

    return count;
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
	split_string_n,
};


strlib_pt strlib = &do_strlib;






