/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "mbedtls/sha1.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include "mbedtls/platform.h"
#include "string.h"
#include <stdint.h>
#include <stdlib.h>

/* USER CODE END Includes */

#include "framework.h"

#if 0
static uint8_t msg[] = 
{"aaaaaaaaaaaa"};

void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
    mbedtls_printf("%s", info);
    for(uint32_t i = 0; i < len; i++) {
        mbedtls_printf("%02x ", msg[i]);
    }
    mbedtls_printf("\n");
}

void test_base64(void)
{
	size_t len;
	uint8_t rst[512];
	
	len = sizeof(msg);
	dump_buf("\r\n base64 message: ", msg, len);
	
	mbedtls_base64_encode(rst, sizeof(rst), &len, msg, len);
	mbedtls_printf("\r\n base64 encode : %s len %d\r\n", rst,len);
	
	mbedtls_base64_decode(rst, sizeof(rst), &len, rst, len);
	dump_buf("\r\n base64 decode : ", rst, len);
	printf("\r\n");
	while(1);
}
#endif

#define AES_DAT "TTTiFCzfUiwWdaLAU75IQQ=="

#define AES_IV 	"yaoqidixiaoye123"

#define AES_KEY "meikakjhuwenping"

#define BLOCK_SIZE 1024


//明文空间
unsigned char plain[64] = "22020220708";

// aes加密
// aes_key -- 最长48字节
// iv -- 最长16字节
// plaintext -- 待加密文本
// ciphertext -- 加密得到的文本
// len -- len should be 16*N bytes
// return – 0 for ok, else for error
//len should be 16*N bytes
int aes_cbc_encryp(uint8_t *aes_key, uint8_t *iv, uint8_t *plaintext, uint8_t *ciphertext, uint32_t len) 
{
    int i;
    int blk = (len + 15) >> 4;
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    //setkey_dec
    mbedtls_aes_setkey_enc(&aes_ctx, aes_key, 128);
    for (i = 0; i < blk; ++i) {
        mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, 16, iv, plaintext + (i * 16), ciphertext + (i * 16));
    }
    mbedtls_aes_free(&aes_ctx);
    return 0; //OK
}

// aes解密
// aes_key -- 最长48字节
// iv -- 最长16字节
// ciphertext -- 待解密的文本
// plaintext -- 解密好的文本
// len -- len should be 16*N bytes
// return – 0 for ok, else for error
//len should be 16*N bytes
int aes_cbc_decryp(uint8_t *aes_key, uint8_t *iv, uint8_t *ciphertext, uint8_t *plaintext, uint32_t len) 
{
    int i;
    int blk = (len + 15) >> 4;
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    //setkey_dec
    mbedtls_aes_setkey_dec(&aes_ctx, aes_key, 128);
    for (i = 0; i < blk; ++i) {
        mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, 16, iv, ciphertext + (i * 16), plaintext + (i * 16));
    }
    mbedtls_aes_free(&aes_ctx);
    return 0; //OK
}

char srcBuff[BLOCK_SIZE] = {0};
char desBuff0[BLOCK_SIZE] = {0};
char desBuff1[BLOCK_SIZE] = {0};
void main_test_aes(void)
{
//	int i;
	
	mbedtls_printf( "key:%s \r\n",AES_KEY );
	mbedtls_printf( "iv :%s \r\n",AES_IV );
	
	mbedtls_printf( "dat:%s \r\n",plain );
	
	
	uint8_t key[16];
	uint8_t iv[16];
	
	//加密	
	memcpy(iv, 	AES_IV, 	sizeof(iv));
	memcpy(key, AES_KEY, 	sizeof(iv));
	aes_cbc_encryp( key, iv,(unsigned char *)plain, 		(unsigned char *)desBuff0, 64 );
	mbedtls_printf( "aes_cbc_encryp:%s \r\n",desBuff0 );
	
	//解密
	memcpy(iv, 	AES_IV, 	sizeof(iv));
	memcpy(key, AES_KEY, 	sizeof(iv));
	aes_cbc_decryp( key, iv,(unsigned char *)desBuff0, 	(unsigned char *)desBuff1, 64 );
	mbedtls_printf( "aes_cbc_decryp:%s \r\n",desBuff1 );
	
	
	
	
	//--------------------------------------------------
	//base 64
	 
	uint8_t b64_buf[64];
	
	memcpy(b64_buf, desBuff0, 64 );
	
	size_t len =64;
	uint8_t rst[512];
	mbedtls_base64_encode(rst, sizeof(rst), &len, b64_buf, 16);
	mbedtls_printf("\r\n base64 encode : %s len %d\r\n", rst,len);
//	
//	mbedtls_base64_decode(rst, sizeof(rst), &len, rst, len);
//	mbedtls_printf("\r\n base64 decode : %s len %d\r\n", rst, len);
//	
	
	

}


