/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
#include "checksum.h"
u_int8_t checksum8(u_int8_t *array,u_int8_t len) {
     u_int8_t chk = 0;
     for(u_int8_t i=0;i<len;i++) {
	  chk = (chk + array[i]);
     }
     return chk;
}
