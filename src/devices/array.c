/**
 *
 */
#include "../harkd-device.h"
#include "protocols.h"
#include <stdio.h>
#include <string.h>
#define MSG_371X_CMD_SET          0x90
#define MSG_371X_CMD_GET          0x91
#define MSG_371X_CMD_SET_ON_OFF   0x92
#define MSG_371X_CMD_SET_ODD_1_5  0x93
#define MSG_371X_CMD_SET_ODD_6_10 0x94
#define MSG_371X_CMD_START        0x95
#define MSG_371X_CMD_STOP         0x96

/* ------------------------------- Communication protocol ------------------------- */
typedef struct __attribute__((__packed__)) {
     u_int16_t IM;
     u_int16_t PM;
     u_int8_t  addr;
     u_int8_t  mod;
#    define MSG_371X_SET_CURRENT    0x01
#    define MSG_371X_SET_POWER      0x02
#    define MSG_371X_SET_RESISTANCE 0x03
     u_int16_t value;
} msg_371x_info_set_t;
typedef struct __attribute__((__packed__)) {
     u_int16_t I;
     u_int32_t V;
     u_int16_t P;
     u_int16_t IM;
     u_int16_t PM;
     u_int16_t R;
     u_int8_t  os;
} msg_371x_info_get_t; 



harkd_r msg_371x_send_ON(harkd_t *harkd,u_int8_t addr) {
     msg26_t msg; msg26_init(&msg,MSG_371X_CMD_SET_ON_OFF);
     msg.info[0] = 0b00000011;
     return msg26_send(harkd,&msg,addr);
}
harkd_r msg_371x_send_R(harkd_t *harkd,u_int8_t addr,double R,double IM,double PM) {
     msg26_t msg; msg26_init(&msg,MSG_371X_CMD_SET);
     msg_371x_info_set_t *set = (msg_371x_info_set_t *) msg.info;
     set->IM    = (u_int16_t) ((IM) * 30000/30 );
     set->PM    = (u_int16_t) ((PM) * 2000/200 );
     set->mod   = MSG_371X_SET_RESISTANCE;
     set->value = (u_int16_t) ( (R) * 50000/500 );
     return msg26_send(harkd,&msg,addr);
}
harkd_r msg_371x_send_I(harkd_t *harkd,u_int8_t addr,double I,double IM,double PM) {
     msg26_t msg; msg26_init(&msg,MSG_371X_CMD_SET);
     msg_371x_info_set_t *set = (msg_371x_info_set_t *) msg.info;
     set->IM    = IM * (30000/30 );
     set->PM    = PM * (2000/200 );
     set->mod = MSG_371X_SET_CURRENT;
     set->value = (u_int16_t) ( (I) * 30000/30 );
     return msg26_send(harkd,&msg,addr);
}
harkd_r msg_371x_send_P(harkd_t *harkd,u_int8_t addr,double P,double IM,double PM) {
     msg26_t msg; msg26_init(&msg,MSG_371X_CMD_SET);
     msg_371x_info_set_t *set = (msg_371x_info_set_t *) msg.info;
     set->IM    = IM * (30000/30 );
     set->PM    = PM * (2000/200 );
     set->mod = MSG_371X_SET_POWER;
     set->value = (u_int16_t) ( (P) * 2000/200 );
     return msg26_send(harkd,&msg,addr);
}
harkd_r msg_371x_send_recv_get(harkd_t *harkd,u_int8_t addr,
			       double *I ,double *V ,double *P,
			       double *IM,double *PM,double *R) {
     harkd_r r;
     msg26_t msg; msg26_init(&msg,MSG_371X_CMD_GET);
     msg_371x_info_get_t *get = (msg_371x_info_get_t *) msg.info;
     r = msg26_send(harkd,&msg,addr);
     if(r!=HARKD_OK) return r;
     r = msg26_recv(harkd,&msg);
     if(r!=HARKD_OK) return r;
     if(I)  *I  = (double)get->I  * 30 /30000  ;
     if(V)  *V  = (double)get->V  * 360/360000 ;
     if(P)  *P  = (double)get->P  * 200 /2000  ;
     if(IM) *IM = (double)get->IM * 30  /30000 ;
     if(PM) *PM = (double)get->PM * 200 /2000  ;
     if(R)  *R  = (double)get->R  * 500/50000  ;
     return HARKD_OK;
}















/* ---------------------------- Device ------------------------------------ */
typedef struct {
     u_int8_t addr;
     double I,V,P,IM,PM,R;
} harkd_371x_t;
harkd_r harkd_array_init(harkd_t *harkd,const char *port,char *args[]) {
     harkd_371x_t *d = harkd_udata(harkd);
     d->addr = 0x0;
     d->IM   = 30;
     d->PM   = 200;
     if(args[0]) {
	  if(args[1]) {
	       d->addr = atoi(args[1]);
	  }
     }
     if(harkd_serial_open(harkd,port)==NULL)                             goto io_error;
     if(msg_371x_send_ON(harkd,d->addr)!=HARKD_OK)                       goto io_error;
     if(msg_371x_send_I(harkd,d->addr,0,d->IM,d->PM)!=HARKD_OK)          goto io_error;
     if(msg_371x_send_recv_get
	(harkd,d->addr,
	 &d->I,&d->V,&d->P,&d->IM,&d->PM,&d->R)!=HARKD_OK) goto another_device;
     
     return HARKD_OK;
io_error:
     return HARKD_ERR;
another_device:
     return HARKD_ERR;
}
harkd_r harkd_array_command(harkd_t *harkd,char *args[]) {
     return HARKD_OK;
}
harkd_r harkd_array_get (harkd_t *harkd,const char *var,double *val) {
     harkd_371x_t *d = harkd_udata(harkd);
     harkd_r r = msg_371x_send_recv_get(harkd,d->addr,&d->I,&d->V,&d->P,&d->IM,&d->PM,&d->R);
     if(r!=HARKD_OK) return r;
     if(!strcasecmp(var,"I"))         { *val = d->I;
     } else if(!strcasecmp(var,"V"))  { *val = d->V;
     } else if(!strcasecmp(var,"P"))  { *val = d->P;
     } else if(!strcasecmp(var,"IM")) { *val = d->IM;
     } else if(!strcasecmp(var,"PM")) { *val = d->PM;
     } else if(!strcasecmp(var,"R"))  { *val = d->R;
     } else {
	  harkd_errorf_invalid_variable(harkd,var);
	  return HARKD_ERR;
     }
     return r;
}
harkd_r harkd_array_set (harkd_t *harkd,const char *var,double *val) {
     harkd_r r; harkd_371x_t *d = harkd_udata(harkd);
     if(!strcasecmp(var,"R")) {
	  r = msg_371x_send_R(harkd,d->addr,*val,d->IM,d->PM);
     } else if(!strcasecmp(var,"I")) {
	  r = msg_371x_send_I(harkd,d->addr,*val,d->IM,d->PM);
     } else if(!strcasecmp(var,"P")) {
	  r = msg_371x_send_P(harkd,d->addr,*val,d->IM,d->PM);
     } else if(!strcasecmp(var,"IM")) {
	  d->IM = *val;
     } else if(!strcasecmp(var,"PM")) {
	  d->PM = *val;
     } else {
	  harkd_errorf_invalid_variable(harkd,var);
	  return HARKD_ERR;
     }
     return r;
}


harkd_r harkd_array_clear(harkd_t *harkd) {
     return msg_371x_send_I(harkd,0x0,0,30,200);
}




const harkd_def_t HARKD_DEVICE_ARRAY_371X = {
     "ARRAY-371X",
     "Single input programmable DC electronic load.",
     's',
     harkd_help_inputs("R,I,P,IM,IP,V")
     harkd_help_outputs("R,I,P,IM,IP")
     ,
     {
	  sizeof(harkd_371x_t),
	  harkd_array_init,
	  harkd_array_command,
	  harkd_array_clear,
	  harkd_array_set,
	  harkd_array_get
     }
};
