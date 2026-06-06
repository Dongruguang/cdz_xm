#ifndef __message_h__
#define __message_h__

#include "framework.h"
#include "stdlib.h"
#include "network.h"



typedef int (*cmd_cb_t)( void* ndev,int argc,char** argv );

typedef struct{
	char* name;				//×Ö·û´®
	cmd_cb_t entry;		//²Ù×÷º¯Êý

}net_cmd_t;

extern int net_cycle( void* ndev );
extern int msg_cycle( void* ndev,int argc,char** argv );


#endif



