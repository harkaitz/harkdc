/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
#ifndef _HARKD_PROTOCOLS_H_
#define _HARKD_PROTOCOLS_H_
#include "../harkd.h"
#ifdef _WIN32
#  include <stdint.h>
typedef uint8_t  u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
#else
#  include <sys/types.h>
#endif
typedef struct __attribute__((__packed__)) {
     u_int8_t flag;
     u_int8_t addr;
     u_int8_t cmd;
     u_int8_t info[22];
     u_int8_t chksum;
} msg26_t;
harkd_r msg26_send(harkd_dev_obj_t *harkd,msg26_t *msg,u_int8_t addr);
harkd_r msg26_recv(harkd_dev_obj_t *harkd,msg26_t *msg);
void    msg26_init(msg26_t *msg,u_int8_t cmd);


#endif
