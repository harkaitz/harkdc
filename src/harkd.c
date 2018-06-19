#include <stdio.h>
#include <uterm/uterm.h>
#include <libserialport.h>
#include <string.h>

#include "harkd.h"
#include "harkd-device.h"
#include "harkd-test.h"

#include "devices/devices.h"
#include "tests/tests.h"

#define HARKD_MAX_DEVICES     15
#define HARKD_MAX_DEVICE_DEFS 20
#define HARKD_MAX_TESTS       30


static struct {
     int                 initialized;
     harkd_t             devices    [HARKD_MAX_DEVICES];
     harkd_def_t         device_defs[HARKD_MAX_DEVICE_DEFS];
     const harkd_test_t *tests      [HARKD_MAX_TESTS];
} g = { 0 };




/* -------------------------------------------------------------- */
void     harkd_init(void) {
     if(!g.initialized) {
	  int i;
	  memset(&g,0,sizeof(g));
	  g.initialized = 1;
	  for(int i=0;harkd_device_list[i];i++) {
	       const harkd_def_t *d = harkd_device_list[i];
	       memcpy(&g.device_defs[i],d,sizeof(g.device_defs[i]));
	  }
	  i=0; for(const harkd_test_t **t=harkd_tests;*t;t++) {
	       g.tests[i++] = *t;
	  }
	  g.tests[i] = NULL;
     }
}
void     harkd_clean(void) {
     if(g.initialized) {
	  g.initialized = 0;
	  FOREACH_DEVICE(d) { harkd_free(d); }
     }
}
const harkd_def_t *harkd_get_def  (const char *name) {
     FOREACH_DEVICE_DEFS(df) {
	  if(!strncasecmp(name,df->name,strlen(name))) {
	       return df;
	  }
     }
     return NULL;
}
void harkd_list_ports(const harkd_def_t *device,char buffer[],int len) {
     buffer[0] = '\0';
     if(device->port_type=='s') {
	  harkd_serial_ports(NULL,buffer,len);
     } else {
	  strncat(buffer,"open\n",len);
     }
}
harkd_t           *harkd_list_devices(int start,harkd_t     **r) {
     if(start) *r = &g.devices[0];
     if((*r)==&g.devices[HARKD_MAX_DEVICES]) {
	  return NULL;
     } else if((*r)->name[0]) {
	  return ((*r)++);
     } else {
	  (*r)++;
	  return harkd_list_devices(0,r);
     }
}
harkd_def_t       *harkd_list_drivers(int start,harkd_def_t **r) {
     if(start) *r = &g.device_defs[0];
     if((*r)==&g.device_defs[HARKD_MAX_DEVICE_DEFS]) {
	  return NULL;
     } else if((*r)->name) {
	  return ((*r)++);
     } else {
	  (*r)++;
	  return harkd_list_drivers(0,r);
     }
}
const harkd_test_t  *harkd_test_list(int start,const harkd_test_t ***r) {
     if(start) *r = g.tests;
     if(!(*r)) {
	  return NULL;
     } else {
	  return *((*r)++);
     }
}








/* -------------------------------------------------------------- */
harkd_t *harkd_new(const char *name,const harkd_def_t *device,
		   const char *port,char *args[],uterm_t *opt_uterm) {
     
     uterm_t *u = (opt_uterm)?opt_uterm:uterm_global(0);

     /* Check arguments. */
     if(!name[0])   { uterm_errorf(u,"Please specify a name.");         goto error; }
     /* --- */
     harkd_t *harkd = NULL;
     for(harkd_t *d=g.devices;d<g.devices+HARKD_MAX_DEVICES;d++) {
	  if(!d->name[0]) {
	       harkd = d;
	  } else if(!strcasecmp(name,d->name)) {
	       harkd_free(d);
	       harkd = d;
	  }
     }
     if(!harkd) {
	  uterm_errorf(u,"Maximun number of devices reached."); goto error;
     }
     strncpy(harkd->name,name,sizeof(harkd->name)-1);
     harkd->uterm = opt_uterm;
     harkd->def   = device;
     if(!harkd->def) {
	  uterm_errorf(u,"Device model `%s` not supported.",device); goto error;
     }
     /* --- */
     harkd_udata(harkd);
     char buffer[128] = {0};
     if(port) {
	  strncpy(buffer,port,sizeof(buffer)-1);
     } else {
	  harkd_list_ports(harkd->def,buffer,sizeof(buffer)-1);
     }
     /* Search a port. */
     char *found   = NULL;
     char *args2[] = {NULL};
     for(char *r,*s = strtok_r(buffer,"\n,",&r);
	 s;
	 s = strtok_r(NULL,"\n,",&r)) {
	  if(!harkd->def->fn.init) {
	       found = s;
	       break;
	  } else if(harkd->def->fn.init(harkd,s,(args)?args:args2)==HARKD_OK) {
	       found = s;
	       break;
	  }
     }
     if(!found) {
	  harkd_errorf_invalid_driver(harkd,port);
	  goto error;
     }
     // uterm_printf(u,"New device `%s` on `%s`.\n",name,found);
     if(!uterm_getvar(u,name,1)) {
	  uterm_setvar(u,name,1,harkd,(void*)harkd_free);
     }
     return harkd;
error:
     if(harkd) harkd_free(harkd);
     return NULL;
}
harkd_t *harkd_get (const char *name,uterm_t *opt_uterm) {
     FOREACH_DEVICE(d) {
	  if(opt_uterm) if(opt_uterm!=uterm(d)) continue;
	  if(!strcasecmp(name,d->name)) {
	       return d;
	  }
     }
     return NULL;
}
void     harkd_free(harkd_t *harkd) {
     if(harkd) {
	  //debugf("Cleaning %p ...",harkd);
	  if(harkd->def->fn.clear)
	       harkd->def->fn.clear(harkd);
	  if(harkd->udata && harkd->def->fn.len)
	       free(harkd->udata);
	  harkd_close(harkd);
	  memset(harkd,0,sizeof(*harkd));
     }
}
void    *harkd_udata(harkd_t *harkd) {
     if((!harkd->udata) && harkd->def->fn.len) {
	  harkd->udata = malloc(harkd->def->fn.len);
	  memset(harkd->udata,0,harkd->def->fn.len);
     }
     return harkd->udata;
}
void     harkd_set_uterm(harkd_t *harkd,uterm_t *uterm) {
     harkd->uterm = uterm;
}
const char *harkd_name(harkd_t *harkd) {
     return harkd->name;
}







harkd_r harkd_run      (harkd_t *harkd,char *args[]) {
     char *d_args[] = {"info",NULL};
     if(harkd->def->fn.command) {
	  return harkd->def->fn.command(harkd,(args)?args:d_args);
     } else {
	  return HARKD_OK;
     }
}
harkd_r harkd_var_set  (harkd_t *harkd,const char *var,double *val) {
     if(harkd->def->fn.set) {
	  return harkd->def->fn.set(harkd,var,val);
     } else {
	  harkd_errorf(harkd,"Setting values is not supported for this device.");
	  return HARKD_ERR;
     }
}
harkd_r harkd_var_get  (harkd_t *harkd,const char *var,double *val) {
     if(harkd->def->fn.get) {
	  return harkd->def->fn.get(harkd,var,val);
     } else {
	  harkd_errorf(harkd,"Getting values is not supported for this device.");
	  return HARKD_ERR;
     }
}




void     harkd_close    (harkd_t *harkd) {
     if(harkd->port) {
	  sp_close(harkd->port);
	  sp_free_port(harkd->port);
	  harkd->port = NULL;
     }
     harkd->portname[0] = '\0';
}
int      harkd_is_open  (harkd_t *harkd) {
     return (harkd->port)?1:0;
}

/* -------------------------------------------------------------- */

int harkd_port_is_openned(uterm_t *uterm,const char *portname) {
     FOREACH_DEVICE(d) if(!strcasecmp(d->portname,portname)) return 1;
     return 0;
}


/* ----------------------------------------------------------------------- */
harkd_r harkd_serial_ports(uterm_t *uterm,char *o_ports,int o_len) {
     struct sp_port **port_list = NULL;
     o_ports[0] = '\0';
     if(sp_list_ports (&port_list)==SP_OK) {
	  for(struct sp_port **port=port_list;*port;port++) {
	       const char *p = sp_get_port_name (*port);
	       FOREACH_DEVICE(d) {
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
	  uterm_errorf((uterm)?uterm:uterm_global(0),"%s",err);
	  sp_free_error_message(err);
	  return HARKD_ERR;
     }
}
sp_port_t *harkd_serial_open(harkd_t *harkd,const char *i_portname) {
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
     harkd_set_serial_error(harkd);
     harkd_close(harkd);
     return NULL;
}
harkd_r    harkd_serial_write    (harkd_t *harkd,void *buffer,size_t len) {
     sp_port_t *port = harkd->port;
     if(!port) return HARKD_ERR;
     if(sp_blocking_write (port,buffer,len,1000)!=len) {
	  harkd_set_serial_error(harkd);
	  return HARKD_ERR;
     }
     if(sp_drain(port)!=SP_OK) {
	  harkd_set_serial_error(harkd);
	  return HARKD_ERR;
     }
     return HARKD_OK;
     
}
harkd_r    harkd_serial_read     (harkd_t *harkd,void *buffer,size_t len) {
     sp_port_t *port = harkd->port; int r;
     if(!port) return HARKD_ERR;
     r = sp_blocking_read (port,buffer,len,1000);
     if(r==len) {
	  return HARKD_OK;
     } else {
	  if(r<0) harkd_set_serial_error(harkd);
	  return HARKD_ERR;
     }
}
char      *harkd_serial_gets(harkd_t *harkd,char *buffer,size_t len) {
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
     harkd_errorf(harkd,"Buffer overflow.");
     return NULL;
}


harkd_r   harkd_serial_puts(harkd_t *harkd,char *buffer) {
     uterm_debugf("serial","%s",buffer);
     return harkd_serial_write(harkd,buffer,strlen(buffer));
}
void       harkd_set_serial_error(harkd_t *harkd) {
     char *err = sp_last_error_message();
     harkd_errorf(harkd,"%s: %s",harkd->portname,err);
     sp_free_error_message(err);
}

/* ------------------------------------------------------------------------------------- */
harkd_r harkd_test_run(uterm_t *u,const harkd_test_t *t,char *i_opts[]) {
     harkd_test_var_t opts[30]; int o;
     for(o=0;t->opts[o].name && o<29;o++) {
	  opts[o].name  = t->opts[o].name;
	  opts[o].value = (t->opts[o].value)?t->opts[o].value:"";
     }
     if(!i_opts) {} else if(!i_opts[0]) {
     } else if (!strcasecmp(i_opts[0],"help")) {
	  uterm_printf(u,"## %s\n\n%s\n\n",t->name,t->long_description);
	  uterm_printf(u,"+ Variables;\n\n");
	  for(int j=0;t->opts[j].name;j++) {
	       uterm_printf(u,"  + %-10s = %s\n",t->opts[j].name,t->opts[j].value);
	  }
	  uterm_printf(u,"\n");
	  return HARKD_OK;
     } else {
	  for(int k=0,j;i_opts[k];k++) {
	       char *var = i_opts[k],*val="",*valn;
	       if(!var) continue;
	       if((valn = strchr(var,'='))) {
		    *valn='\0'; valn++; val = valn;
	       }
	       for(j=0;j<o;j++) {
		    if(!strcasecmp(var,opts[j].name)) {
			 opts[j].name  = var;
			 opts[j].value = val;
			 break;
		    }
	       }
	       if(j==o && o<29) {
		    opts[o].name  = var;
		    opts[o].value = val;
		    o++;
	       }
	  }
     }
     opts[o].name = NULL;
     return t->fun(u,opts);
}

#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif


void harkd_wait(int ms) {
#    ifdef _WIN32
     Sleep(ms);
#    else
     usleep(ms*1000);
#    endif
}
