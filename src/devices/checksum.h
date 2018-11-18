/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_
#ifdef _WIN32
#  include <stdint.h>
typedef uint8_t  u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
#else
#  include <sys/types.h>
#endif
u_int8_t checksum8(u_int8_t *array,u_int8_t len);
#endif
