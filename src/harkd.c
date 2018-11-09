#include <stdio.h>
#include <hitz/hitz.h>
#include <libserialport.h>
#ifndef DISABLE_XLSX
#  include <xlsxwriter.h>
#endif

#include <string.h>
#include <stdarg.h>

#include "harkd.h"







/* -------------------------------------------------------------- */
void harkd_list_ports(const harkd_dev_itf_t *device,char buffer[],int len) {
     buffer[0] = '\0';
     if(device->port_type=='s') {
	  harkd_serial_ports(buffer,len);
     } else {
	  strncat(buffer,"open\n",len);
     }
}




/* -------------------------------------------------------------- */
harkd_dev_obj_t *harkd_new(const char            *name,
			   const harkd_dev_itf_t *itf,
			   const char            *portname,
			   const char *args[]) {
     
     harkd_dev_obj_t *harkd = NULL;
     /* 1.- Check arguments. */
     if(!name)   { harkd_error(NULL,"Please specify a name."); return NULL; }
     if(!itf)    { harkd_error(NULL,"Invalid interface.");     return NULL; }

     
     /* 2- If the device exists, close. */
     HARKD_LIST_FOREACH(harkd_dev_obj_t,d,harkd_dev_obj_list) {
	  if(!strcasecmp(name,d->name)) {
	       *_nxt = d->next;
	       harkd_free(d);
	  }
     }

     /* 3.- Create object. */
     harkd = malloc(sizeof(*harkd)+itf->fn.len);
     memset(harkd,0,sizeof(*harkd)+itf->fn.len);
     strncpy(harkd->name,name,sizeof(harkd->name)-1);
     harkd->itf   = itf;
     harkd->portname[0] = '\0';
     
     harkd->port  = NULL;
     harkd->next  = NULL;
     
     /* 4.- Search a port. */
     char buffer[HARKD_MAX_TEXT] = {0};
     if(portname) {
	  strncpy(buffer,portname,sizeof(buffer)-1);
     } else {
	  harkd_list_ports(harkd->itf,buffer,sizeof(buffer)-1);
     }
     char *found   = NULL;
     const char *args2[] = {NULL};
     for(char *r,*s = strtok_r(buffer,"\n,",&r);s;s = strtok_r(NULL,"\n,",&r)) {
	  harkd_log(1,"Trying to open `%s` ...",s);
	  if(!harkd->itf->fn.init) {
	       found = s;
	       break;
	  } else if(harkd->itf->fn.init(harkd,s,(args)?args:args2)>=0) {
	       found = s;
	       break;
	  }
     }

     /* 5.- Can't search a valid port. */
     if(!found) {
	  harkd_error(NULL,"Can't open any port.");
	  goto error;
     }

     /* 6.- Add to list. */
     harkd_log(1,"Add [harkd_dev_obj_t *%p] to device objects list.",harkd);
     harkd->next = harkd_dev_obj_list;
     harkd_dev_obj_list = harkd;
     
     return harkd;
error:
     if(harkd) harkd_free(harkd);
     return NULL;
}
harkd_dev_obj_t *harkd_get (const char *name) {
     HARKD_LIST_FOREACH(harkd_dev_obj_t,d,harkd_dev_obj_list) {
	  if(!strcasecmp(name,d->name)) {
	       return d;
	  }
     }
     return NULL;
}
void     harkd_free(harkd_dev_obj_t *harkd) {
     if(!harkd) return;
     if(harkd->itf->fn.clear)
	  harkd->itf->fn.clear(harkd);
     harkd_close(harkd);
}
harkd_r harkd_run      (harkd_dev_obj_t *harkd,const char *args[]) {
     const char *d_args[] = {"info",NULL};
     if(harkd->itf->fn.command) {
	  return harkd->itf->fn.command(harkd,(args)?args:d_args);
     } else {
	  return HARKD_OK;
     }
}
harkd_r harkd_var_set  (harkd_dev_obj_t *harkd,const char *var,double *val) {
     if(harkd->itf->fn.set) {
	  return harkd->itf->fn.set(harkd,var,val);
     } else {
	  harkd_error(harkd,"Setting values is not supported for this device.");
	  return HARKD_ERR;
     }
}
harkd_r harkd_var_get  (harkd_dev_obj_t *harkd,const char *var,double *val) {
     if(harkd->itf->fn.get) {
	  return harkd->itf->fn.get(harkd,var,val);
     } else {
	  harkd_error(harkd,"Getting values is not supported for this device.");
	  return HARKD_ERR;
     }
}
void     harkd_close    (harkd_dev_obj_t *harkd) {
     if(harkd->port) {
	  sp_close(harkd->port);
	  sp_free_port(harkd->port);
	  harkd->port = NULL;
     }
     harkd->portname[0] = '\0';
}
int      harkd_is_open  (harkd_dev_obj_t *harkd) {
     return (harkd->port)?1:0;
}
int      harkd_port_is_openned(const char *portname) {
     HARKD_LIST_FOREACH(harkd_dev_obj_t,d,harkd_dev_obj_list) {
	  if(!strcasecmp(d->portname,portname)) return 1;
     }
     return 0;
}








/* ------------------------------------------------------------------------------------- */
harkd_r harkd_test_run(const harkd_test_t *t,const char *i_opts[]) {
     harkd_test_var_t opts[30]; int o;
     for(o=0;t->opts[o].name && o<29;o++) {
	  opts[o].name  = t->opts[o].name;
	  opts[o].value = (t->opts[o].value)?t->opts[o].value:"";
     }
     if(!i_opts) {} else if(!i_opts[0]) {
     } else if (!strcasecmp(i_opts[0],"help")) {
	  harkd_printf(1,"## %s\n\n%s\n\n",t->name,t->long_description);
	  harkd_printf(1,"+ Variables;\n\n");
	  for(int j=0;t->opts[j].name;j++) {
	       harkd_printf(1,"  + %-10s = %s\n",t->opts[j].name,t->opts[j].value);
	  }
	  harkd_printf(1,"\n");
	  return HARKD_OK;
     } else {
	  for(int k=0,j;i_opts[k];k++) {
	       char *var = (char*)i_opts[k],*val="",*valn;
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
     return t->fun(opts);
}
