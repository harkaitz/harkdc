/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
#include "harkd.h"
#include <libserialport.h>
#include <string.h>
/* --------------------------------------------------------------------------------------- */
harkd_r harkd_serial_ports(char *o_ports,int o_len) {
     struct sp_port **port_list = NULL;
     o_ports[0] = '\0';
     if(sp_list_ports (&port_list)==SP_OK) {
	  for(struct sp_port **port=port_list;*port;port++) {
	       const char *p = sp_get_port_name (*port);
	       HARKD_LIST_FOREACH(harkd_dev_obj_t,d,harkd_dev_obj_list) {
		    if(!strcasecmp(p,d->portname)) {
			 p = NULL; break;
		    }
	       }
	       if(p) {
		    strncat(o_ports,p,o_len-1);
		    strncat(o_ports,"\n",o_len-1);
	       }
	  }
	  o_ports[o_len-1] = '\0';
	  sp_free_port_list (port_list);
	  return HARKD_OK;
     } else {
	  char *err = sp_last_error_message();
	  harkd_error(NULL,"%s",err);
	  sp_free_error_message(err);
	  return HARKD_ERR;
     }
}
sp_port_t *harkd_serial_open(harkd_dev_obj_t *harkd,const char *i_portname) {
     sp_port_t *port  = NULL;
     harkd_close(harkd);
     if(sp_get_port_by_name(i_portname,&port)!=SP_OK) {
	  goto error;
     }
     // debugf("Port %p: Opening %s ...",port,i_portname);
     if(sp_open(port,SP_MODE_READ | SP_MODE_WRITE)!=SP_OK) {
	  goto error;
     }
     if(sp_set_baudrate    (port, 9600                )!=SP_OK) goto error;
     if(sp_set_bits        (port, 8                   )!=SP_OK) goto error;
     if(sp_set_parity      (port, SP_PARITY_NONE      )!=SP_OK) goto error;
     if(sp_set_stopbits    (port, 1                   )!=SP_OK) goto error;
     
     if(sp_set_rts         (port, SP_RTS_OFF          )!=SP_OK) goto error;
     if(sp_set_cts         (port, SP_CTS_IGNORE       )!=SP_OK) goto error;
     if(sp_set_dtr         (port, SP_DTR_OFF          )!=SP_OK) goto error;
     if(sp_set_dsr         (port, SP_DSR_IGNORE       )!=SP_OK) goto error;
     if(sp_set_xon_xoff    (port, SP_XONXOFF_DISABLED )!=SP_OK) goto error;
     
     if(sp_set_flowcontrol (port, SP_FLOWCONTROL_NONE )!=SP_OK) goto error;
     if(sp_flush           (port, SP_BUF_BOTH         )!=SP_OK) goto error;
     harkd->port = port;
     strncpy(harkd->portname,i_portname,sizeof(harkd->portname)-1);
     return port;
error:
     harkd_set_serial_error(harkd,"Opening error.");
     harkd_close(harkd);
     return NULL;
}
harkd_r    harkd_serial_write    (harkd_dev_obj_t *harkd,void *buffer,size_t len) {
     sp_port_t *port = harkd->port;
     if(!port) return HARKD_ERR;
     if(sp_blocking_write (port,buffer,len,1000)!=len) {
	  harkd_set_serial_error(harkd,"Writting error.");
	  return HARKD_ERR;
     }
     if(sp_drain(port)!=SP_OK) {
	  harkd_set_serial_error(harkd,"Writting error.");
	  return HARKD_ERR;
     }
     return HARKD_OK;
     
}
harkd_r    harkd_serial_read     (harkd_dev_obj_t *harkd,void *buffer,size_t len) {
     sp_port_t *port = harkd->port; int r;
     if(!port) return HARKD_ERR;
     r = sp_blocking_read (port,buffer,len,1000);
     if(r==len) {
	  return HARKD_OK;
     } else {
	  if(r<0) harkd_set_serial_error(harkd,"Reading error");
	  return HARKD_ERR;
     }
}
char      *harkd_serial_gets(harkd_dev_obj_t *harkd,char *buffer,size_t len) {
     for(size_t i=0;i<(len-1);i++) {
	  if(harkd_serial_read(harkd,buffer+i,1)!=HARKD_OK) {
	       return NULL;
	  }
	  if(buffer[i]=='\r'||buffer[i]=='\n') {
	       if(i--) {
		    buffer[i] = '\0';
		    return buffer;
	       }
	  }
     }
     harkd_error(harkd,"Buffer overflow.");
     return NULL;
}


harkd_r   harkd_serial_puts(harkd_dev_obj_t *harkd,char *buffer) {
     harkd_log(1,"serial %s",buffer);
     return harkd_serial_write(harkd,buffer,strlen(buffer));
}
void       harkd_set_serial_error(harkd_dev_obj_t *harkd,const char *whence) {
     char *err = sp_last_error_message();
     harkd_error(harkd,"%s: Serial port `%s`: %s",whence,harkd->portname,err);
     sp_free_error_message(err);
}
