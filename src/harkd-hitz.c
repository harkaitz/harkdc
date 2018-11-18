/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
#include "harkd.h"
#include <hitz/hitz.h>
#include <string.h>
#include <ctype.h>
#ifdef WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

static hitz_module_t harkd_hitz_module;
hitz_module_t *HARKD_HITZ_MODULE = &harkd_hitz_module;
/* ---------------------------------------------------------------------------------------- */
int harkd_cmd_ports(hitz_t *h,hitz_module_t **module,const char *argv[]) {
     char buffer[512] = {0};
     if(harkd_serial_ports(buffer,sizeof(buffer)-1)==HARKD_OK) {
	  hitz_fprintf(h,1,"Serial ports:\n%s",buffer);
     }
     return 1;
}
int harkd_cmd_interfaces(hitz_t *h,hitz_module_t **module,const char *argv[]) {
     HARKD_ARRAY_FOREACH(const harkd_dev_itf_t,itf,harkd_dev_itf_array) {
	  if(argv[1]) {
	       if(!strcasecmp(itf->name,argv[1])) {
		    harkd_printf(1,"## %s\n\n%s\n\n%s\n\n",itf->name,itf->help,itf->long_help);
	       }
	  } else {
	       harkd_printf(1,"%-20s %s\n",itf->name,itf->help);
	  }
     }
     return 1;
}
int harkd_cmd_new(hitz_t *h,hitz_module_t **module,const char *argv[]) {
     harkd_dev_obj_t *harkd = NULL; const harkd_dev_itf_t *def;
     if(!argv[1]) {
	  harkd_printf(1,"new [NAME|list] [DRIVER|COMMAND|delete] [search] [OPTIONS...]\n");
	  return 0;
     }
     if(!argv[2]) {
	  if(!strcasecmp(argv[1],"list")) {
	       HARKD_LIST_FOREACH(harkd_dev_obj_t,d,harkd_dev_obj_list) {
		    hitz_fprintf(h,1,"%-20s %-20s %s\n",d->name,d->itf->name,d->portname);
	       }
	  } else if((harkd = harkd_get(argv[1]))) {
	       const char *help[] = {"help",NULL};
	       harkd_run(harkd,help);
	  } else {
	       harkd_error(NULL,"Please select an interface.");
	       harkd_cmd_interfaces(h,module,argv+1);
	  }
	  return 0;
     }
     def = harkd_get_itf(argv[2]);
     if(def) {
	  if(!argv[3]) {
	       char buffer[512];
	       harkd_list_ports(def,buffer,sizeof(buffer)-1);
	       if(!buffer[0]) {
		    harkd_error(h,"No available ports.");
	       } else {
		    hitz_fprintf(h,3,"Please select a port:\n");
		    hitz_fprintf(h,1,"%s",buffer);
	       }
	  } else if(!strcasecmp(argv[3],"search")) {
	       harkd = harkd_new(argv[1],def,NULL,argv+4);
	  } else {
	       harkd = harkd_new(argv[1],def,argv[3],argv+4);
	  }
     } else if(!(harkd = harkd_get(argv[1]))) {
	  harkd_error(h,"Device `%s` not found.",argv[1]);
     } else if(!strcasecmp(argv[2],"delete")) {
	  harkd_free(harkd);
     } else {
	  harkd_run(harkd,argv+2);
     }
     return 1;
}
int harkd_cmd_table(hitz_t *h,hitz_module_t **module,const char *argv[]) {
     if(!argv[1]) {
	  hitz_fprintf(h,1,"%s DEV/VAR[=VAL] ...\n",argv[0]);
	  return 1;
     }
     int pl = 0;
     for(char **argp=(char**)argv+1;*argp;argp++) {
	  char            *dev   = strtok(*argp,"/");
	  harkd_dev_obj_t *harkd = harkd_get(dev);
	  if(!harkd) {
	       harkd_error("Device `%s` not found.",dev);
	       continue;
	  }
	  double dval;
	  char  *svar = strtok(NULL,"=");
	  if(!svar) {
	       harkd_error(h,"Specify a variable.");
	       return 1;
	  }
	  for(char *p=svar;*p;p++) *p = toupper(*p);
	  char  *sval = strtok(NULL,"\n");
	  if(sval) {
	       dval = atof(sval);
	       if(harkd_var_set(harkd,svar,&dval)!=HARKD_OK) return 1;
	  } else {
	       if(harkd_var_get(harkd,svar,&dval)!=HARKD_OK) return 1;
	       hitz_fprintf(h,1,"%10.4f,",dval); pl = 1;
	  }
     }
     if(pl) hitz_fprintf(h,1,"\n");
     return 1;
}
int harkd_cmd_test(hitz_t *h,hitz_module_t **module,const char *argv[]) {
     HARKD_ARRAY_FOREACH(const harkd_test_t,t,harkd_test_array) {
	  if(!argv[1]) {
	       hitz_fprintf(h,1,"%-15s %s\n",t->name,t->description);
	  } else if((strcasecmp(t->name,argv[1]))) {

	  } else {
	       harkd_test_run(t,argv+2);
	  }
     }
     return 1;
}
static hitz_module_t harkd_hitz_module = {
     "harkd",
     NULL,
     {
	  {"ports"      ,"List available ports."            ,harkd_cmd_ports},
	  {"new"        ,"Open device."                     ,harkd_cmd_new},
	  {"table"      ,"Get/set values from/to the device",harkd_cmd_table},
	  {"interfaces" ,"Show available device drivers."   ,harkd_cmd_interfaces},
	  {"test"       ,"Run a predefined test."           ,harkd_cmd_test},
	  {NULL}
     }
};


void harkd_wait(int ms) {
#    ifdef _WIN32
     Sleep(ms);
#    else
     usleep(ms*1000);
#    endif
}

