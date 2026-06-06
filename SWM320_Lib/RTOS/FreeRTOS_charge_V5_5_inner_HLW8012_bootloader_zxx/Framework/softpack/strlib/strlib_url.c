
#include "framework.h"

#define PRT_LOG(X...)	sysprt->alog//dprintf(X)	



static int hex2dec(char c)
{
    if ('0' <= c && c <= '9') 
    {
        return c - '0';
    } 
    else if ('a' <= c && c <= 'f')
    {
        return c - 'a' + 10;
    } 
    else if ('A' <= c && c <= 'F')
    {
        return c - 'A' + 10;
    } 
    else 
    {
        return -1;
    }
}

static char dec2hex(short int c)
{
    if (0 <= c && c <= 9) 
    {
        return c + '0';
    } 
    else if (10 <= c && c <= 15) 
    {
        return c + 'A' - 10;
    } 
    else 
    {
        return -1;
    }
}


//±àÂëÒ»¸öurl char src[],char dest[]
void url_encode(char src[],char dest[])
{
    int i = 0;
    int len = strlen( src );
    int res_len = 0;
//    char res[BURSIZE];
    for (i = 0; i < len; ++i) 
    {
        char c = src[i];
        if (    ('0' <= c && c <= '9') ||
                ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') || 
                c == '/' || c == '.') 
        {
            dest[res_len++] = c;
        } 
        else 
        {
            int j = (short int)c;
            if (j < 0)
                j += 256;
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            dest[res_len++] = '%';
            dest[res_len++] = dec2hex(i1);
            dest[res_len++] = dec2hex(i0);
        }
    }
    dest[res_len] = '\0';
    strcpy( src, dest );
}

// ½âÂëurl
void urldecode( char src[],char dest[] )
{
    int i = 0;
    int len = strlen( src );
    int res_len = 0;
//    char res[BURSIZE];
    for (i = 0; i < len; ++i) 
    {
        char c = src[i];
        if (c != '%') 
        {
            dest[res_len++] = c;
        }
        else 
        {
            char c1 = src[++i];
            char c0 = src[++i];
            int num = 0;
            num = hex2dec(c1) * 16 + hex2dec(c0);
            dest[res_len++] = num;
        }
    }
    dest[res_len] = '\0';
    strcpy(src, dest);
}

//int main(int argc, char *argv[])
//{
//    char url[100] = "http://'²âÊÔ/@mike";
//    urlencode(url); //±àÂëºó
//    printf("http://'²âÊÔ/@mike  ----> %s\n", url);

//    char buf[100] = "http%3A//%27%E6%B5%8B%E8%AF%95/%40mike";
//    urldecode(buf); //½âÂëºó
//    printf("http%%3A//%%27%%E6%%B5%%8B%%E8%%AF%%95/%%40mike  ----> %s\n", buf);

//    return 0;
//}
