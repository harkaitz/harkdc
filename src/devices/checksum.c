#include "checksum.h"
u_int8_t checksum8(u_int8_t *array,u_int8_t len) {
     u_int32_t chk = 0;
     for(u_int8_t i=0;i<len;i++) {
	  chk = (chk + array[i]); //  % 256
     }
     return chk % 256;
}
