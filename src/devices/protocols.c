#include "protocols.h"
#include "checksum.h"
#include "../harkd-device.h"
#include <string.h>
harkd_r msg26_send(harkd_t *harkd,msg26_t *msg,u_int8_t addr) {
     msg->flag = 0xAA;
     msg->addr = addr;
     msg->chksum = checksum8((u_int8_t*)msg,25);
     /*
     for(int i=0;i<26;i++) {
	  fprintf(stderr,"%02x:",((u_int8_t*)msg)[i]);
     }
     fprintf(stderr,"\n");
     */
     return harkd_serial_write(harkd,msg,sizeof(*msg));
}
harkd_r msg26_recv(harkd_t *harkd,msg26_t *msg) {
     while(1) {
	  if(harkd_serial_read(harkd,msg,1)!=HARKD_OK) return HARKD_ERR;
	  if(msg->flag == 0xAA) break;
     }
     harkd_r r = harkd_serial_read(harkd,&(msg->addr),sizeof(*msg)-1);
     if(r!=HARKD_OK) return r;
     u_int8_t chk = checksum8((u_int8_t*)msg,25);
     if(msg->chksum!=chk) {
	  harkd_errorf(harkd,"371X: Checksum error.");
	  return HARKD_ERR;
     }
     return HARKD_OK;
}
void    msg26_init(msg26_t *msg,u_int8_t cmd) {
     memset(msg,0,sizeof(*msg));
     msg->cmd = cmd;
}
