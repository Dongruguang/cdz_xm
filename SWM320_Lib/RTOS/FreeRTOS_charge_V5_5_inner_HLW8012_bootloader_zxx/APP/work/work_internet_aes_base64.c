
#include "work.h"

#include "mbedtls/sha1.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include "mbedtls/platform.h"

#define PRT_LOG(X...)	sysprt->alog(X)	
//存放终端信息
uint8_t msh_info[20];

char aes_out_data[512] = {0};
static uint8_t base64_out_data[512] = {0};
static uint8_t url_out_data[512] = {0};


uint8_t* get_internet_encode_out(void)
{
	return url_out_data;
}



#define AES_IV 	"yaoqidixiaoye123"
#define AES_KEY "meikakjhuwenping"
static void dump_msh_info( char* num,char* date );
static void aes_encode( void );
static void base64_encode( void );
static void url_encode( void );
//------------------------------------------------------------------------------
//获取机器终端号

uint32_t get_msh_terminal_val( void )
{
	return wdat->zd_val;
}

static char terminal_num[16];
char* get_msh_terminal_str( void )
{
	int ret = 0;
	//将295转化为字符串
	ret = sprintf( terminal_num,"%d",get_msh_terminal_val() );
	for( int i=ret;i<16;i++ )
		terminal_num[i] = '\0';
	
	return terminal_num;
}

//------------------------------------------------------------------------------

void work_internet_aes_base64( wk_inet_pt inet )
{
	switch( inet->ssta )
	{
		case 0://得到加密明文串
			dump_msh_info( get_msh_terminal_str(),get_date_str() );

			inet->ssta++;
			
			break;
		case 1://进行加密工作
			aes_encode();
			base64_encode();
			url_encode();
			inet->ssta++;
			break;
		case 2:
			inet->msta ++;
			inet->ssta = 0;
			break;
		default:
			break;
	}
	
}

//------------------------------------------------------------------------------
//填充机器信息到缓冲区
static void dump_msh_info( char* num,char* date )
{
	int index = 0,w = 0;
	for( int i=0;i<sizeof( msh_info );i++ )	msh_info[i] = 0;
	//copy num
	index = 0;
	while( 1 )
	{
		if( num[index] == '\0' ) break;
		msh_info[w] = num[index];
		w++;
		index++;
	}
	//copy date
	index = 0;
	while( 1 )
	{
		if( date[index] == '\0' ) break;
		msh_info[w] = date[index];
		w++;
		index++;
	}
	msh_info[w] = '\0';
	
 	sysprt->alog("dump_msh_info:%s\r\n",msh_info);
}
//------------------------------------------------------------------------------
//aes加密
extern int aes_cbc_encryp(uint8_t *aes_key, uint8_t *iv, uint8_t *plaintext, uint8_t *ciphertext, uint32_t len);

static void aes_encode( void )
{
//	sysprt->alog( "key:%s \r\n",AES_KEY );
//	sysprt->alog( "iv :%s \r\n",AES_IV );
	
	sysprt->alog( "dat:%s \r\n",msh_info );

	uint8_t info[128];
	
	uint8_t key[128];
	uint8_t iv[128];
	
	sysprt->alog( "dat:%s \r\n",msh_info );
	
	memset( key,0,128 );
	memset(  iv,0,128 );
	memset( info,0,128 );
	
	
	//加密
	memcpy( info, msh_info ,strlen( (char*)msh_info ) );
	memcpy(iv, 	AES_IV, 	sizeof(iv));
	memcpy(key, AES_KEY, 	sizeof(key));
	
	aes_cbc_encryp( key, iv,(unsigned char *)info, 		(unsigned char *)aes_out_data, 64 );
	//sysprt->alog( "aes_cbc_encryp:%s \r\n",aes_out_data );
}
//------------------------------------------------------------------------------
//base编码

static void base64_encode( void )
{
	uint8_t b64_buf[64];
	
	memcpy(b64_buf, aes_out_data, 64 );
	
	size_t len =64;
	mbedtls_base64_encode( base64_out_data, sizeof(base64_out_data), &len, b64_buf, 16);
	
	
	sysprt->alog("\r\n base64 encode : %s len %d\r\n", base64_out_data,len);
}
//------------------------------------------------------------------------------
//url编码
static void url_encode( void )
{
	strlib->url_encode( (char*)base64_out_data, (char*)url_out_data );
	sysprt->alog( "url:%s\r\n",url_out_data );
}




