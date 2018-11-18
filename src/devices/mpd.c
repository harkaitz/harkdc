/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 

#include "../harkd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
harkd_r harkd_mpd_init(harkd_dev_obj_t *harkd,const char *port,const char *args[]) {
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
harkd_r harkd_mpd_command(harkd_dev_obj_t *harkd,const char *args[]) {
     return HARKD_OK;
}
harkd_r harkd_mpd_get (harkd_dev_obj_t *harkd,const char *var,double *val) {
     char buffer[64];
     if(!(var[0]=='V'||var[0]=='I')) goto variable_not_available; 
     if(!(var[1]=='1'||var[1]=='2')) goto variable_not_available;
     sprintf(buffer,"%cOUT%c?\n",var[0],var[1]);
     if(harkd_serial_puts(harkd,buffer)!=HARKD_OK) goto io_error;
     if(harkd_serial_gets(harkd,buffer,sizeof(buffer))==NULL) goto io_error;
     *val = atof(buffer);
     return HARKD_OK;
variable_not_available:
     harkd_error_invalid_variable(harkd,var);
     return HARKD_ERR;
io_error:
     return HARKD_ERR;
}
harkd_r harkd_mpd_set (harkd_dev_obj_t *harkd,const char *var,double *val) {
     char buffer[64];
     if(!(var[0]=='V'||var[0]=='I')) goto variable_not_available; 
     if(!(var[1]=='1'||var[1]=='2')) goto variable_not_available; 
     sprintf(buffer,"%cSET%c:%.3f" "\n",var[0],var[1],*val);
     if(harkd_serial_puts(harkd,buffer)!=HARKD_OK) goto io_error;
     //harkd_wait(200);
     return HARKD_ERR;
variable_not_available:
     harkd_error_invalid_variable(harkd,var);
     return HARKD_ERR;
io_error:
     return HARKD_ERR;
}
harkd_r harkd_mpd_clear(harkd_dev_obj_t *harkd) {
     return harkd_serial_puts(harkd,
			      "OUT0" "\n");
}
const harkd_dev_itf_t HARKD_DEVICE_MPD3305D = {
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
