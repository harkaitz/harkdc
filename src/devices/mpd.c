/**
 * 
 * 
 * > minicom -b 9600 -D /dev/ttyUSB0 -8 
 */

#include "../harkd-device.h"
#include <stdio.h>
#include <string.h>
harkd_r harkd_mpd_init(harkd_t *harkd,const char *port,char *args[]) {
     char buffer[512];
     if(harkd_serial_open(harkd,port)==NULL)                  goto io_error;
     if(harkd_serial_puts(harkd,"\n*idn?\n")!=HARKD_OK)       goto io_error;
     if(harkd_serial_gets(harkd,buffer,sizeof(buffer))==NULL) goto another_device;
     if(harkd_serial_puts(harkd,"OUT1\n"   )!=HARKD_OK)       goto io_error;
     if(harkd_serial_puts(harkd,"TRACK0\n" )!=HARKD_OK)       goto io_error;
     
     if(strcasecmp("SN:V1.81",buffer))                        goto another_device;
     return HARKD_OK;
io_error:
     return HARKD_ERR;
another_device:
     return HARKD_ERR;
}
harkd_r harkd_mpd_command(harkd_t *harkd,char *args[]) {
     return HARKD_OK;
}
harkd_r harkd_mpd_get (harkd_t *harkd,const char *var,double *val) {
     char buffer[64];
     if(!(var[0]=='V'||var[0]=='I')) goto variable_not_available; 
     if(!(var[1]=='1'||var[1]=='2')) goto variable_not_available;
     sprintf(buffer,"%cOUT%c?\n",var[0],var[1]);
     if(harkd_serial_puts(harkd,buffer)!=HARKD_OK) goto io_error;
     if(harkd_serial_gets(harkd,buffer,sizeof(buffer))==NULL) goto io_error;
     *val = atof(buffer);
     return HARKD_OK;
variable_not_available:
     harkd_errorf_invalid_variable(harkd,var);
     return HARKD_ERR;
io_error:
     return HARKD_ERR;
}
harkd_r harkd_mpd_set (harkd_t *harkd,const char *var,double *val) {
     char buffer[64];
     if(!(var[0]=='V'||var[0]=='I')) goto variable_not_available; 
     if(!(var[1]=='1'||var[1]=='2')) goto variable_not_available; 
     sprintf(buffer,"%cSET%c:%.2f" "\n",var[0],var[1],*val);
     if(harkd_serial_puts(harkd,buffer)!=HARKD_OK) goto io_error;
     return HARKD_ERR;
variable_not_available:
     harkd_errorf_invalid_variable(harkd,var);
     return HARKD_ERR;
io_error:
     return HARKD_ERR;
}
harkd_r harkd_mpd_clear(harkd_t *harkd) {
     return harkd_serial_puts(harkd,
			      "VOUT1:0" "\n"
			      "VOUT2:0" "\n"
			      "IOUT1:0" "\n"
			      "IOUT2:0" "\n"
			      "OUT0"    "\n");
}
const harkd_def_t HARKD_DEVICE_MPD3305D = {
     "MPD-3305D","30V 5A USB Interface Programmable Power Supply",'s',
     harkd_help_outputs("I1,I2,V1,V2")
     harkd_help_inputs("I1,I2,V1,V2")
     ,
     {
	  0,
	  harkd_mpd_init,
	  harkd_mpd_command,
	  harkd_mpd_clear,
	  harkd_mpd_set,
	  harkd_mpd_get
     }
};
