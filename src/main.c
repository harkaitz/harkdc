#include <uterm/uterm.h>
#include <uterm/uterm-socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "harkd.h"
#include "harkd-device.h"
#include "harkd-test.h"
#ifndef NL
#  define NL "\n"
#endif
#ifdef _WIN32
#  define fp_isatty(FP) _isatty(_fileno(FP))
#else
#  define fp_isatty(FP) isatty(fileno(FP))
#endif
#ifndef COPYRIGHT
#  define COPYRIGHT "Copyright (c) 2018, Harkaitz Agirre ezama, All rights reserved."
#endif
const harkd_test_t *test      = NULL;
char              **test_opts = NULL;


uterm_db_t HARKD_COMMANDS[];
void *main_client (uterm_t *uterm) {
     uterm_add_dbs(uterm,UTERM_BASIC_COMMANDS);
     uterm_add_dbs(uterm,HARKD_COMMANDS);
     if(test) {
	  harkd_test_run(uterm,test,test_opts);
     } else {
	  while(uterm_loop(uterm)) {}
     }
     return NULL;
}
int main_func(int argc,char *argv[]) {
     const char *help =
	  "Usage: harkdc -p PORT [-l HOST] [-C MAXCONNS]"          NL
	  "       harkdc"                                          NL
	  ""                                                       NL
	  "HARKDC is a tool to manage some electronic instruments" NL
	  "programatically with a PC."                             NL
	  ""                                                       NL
	  COPYRIGHT                                                NL
	  ""                                                       NL
	  ;
     const char *prompt =
	  "  ___________                                                  " NL
	  " | .^.^.^.^. | HARKD Terminal Interface                        " NL
	  " |-----.-----|                                                 " NL
	  " |HARKD: .oo | Type `help` to see a list of commands.          " NL
	  " |-----' .oo |                                                 " NL
	  " |___________| Run the program with `-h` argument for runtime  " NL
	  "               options.                                        " NL
	  "                                                               " NL
	  COPYRIGHT NL
	  ""                                                                NL
	  ;
     uterm_t *u = uterm_global(0);
     uterm_io_config_stdio(u,"harkd> ");
     char *port    = NULL;
     char *host    = "127.0.0.1";
     int   maxconn = 20;
     
     
     int c; while((c=getopt(argc,argv,"l:p:C:h"))!=-1) {
	  switch(c) {
	  case 'h': printf("%s",help);      return 0;
	  case 'p': port    = optarg;       break;
	  case 'l': host    = optarg;       break;
	  case 'C': maxconn = atoi(optarg); break;
	  default: return 1;
	  }
     }
     
     harkd_init();
     {
	  if(optind<argc) {
	       FOREACH_TEST(t) {
		    if(!strcasecmp(t->name,argv[optind])) { test = t; }
	       }
	       if(!test) {
		    FOREACH_TEST(t) {
			 uterm_printf(u,"%-15s %s\n",t->name,t->description);
		    }
		    uterm_errorf(u,"Please select a valid test.");
		    harkd_clean();
		    return 1;
	       }
	       test_opts = argv+optind+1;
	  }
	  if(port) {
	       uterm_server(".\n","ERROR\n",
			    host,port,maxconn,
			    main_client);
	  } else {
	       if(fp_isatty(stdin)) {
		    fprintf(stderr,"%s",prompt);
	       }
	       main_client (u);
	  }
     }
     harkd_clean();
     
     fflush(stdout);
     fflush(stderr);
     return 0;
}
/* ---------------------------------------------------------------------------------------- */
void harkd_cmd_ports(uterm_t *uterm,void **udata,int argc,char *argv[]) {
     char buffer[512];
     /* Serial ports. */
     if(harkd_serial_ports(uterm,buffer,sizeof(buffer)-1)==HARKD_OK) {
	  buffer[sizeof(buffer)-1] = '\0';
	  uterm_printf(uterm,"Serial ports:\n%s",buffer);
     }
     
}
void harkd_cmd_driver(uterm_t *uterm,void **udata,int argc,char *argv[]) {
     FOREACH_DEVICE_DEFS(df) {
	  if(argv[1]) {
	       if(!strcasecmp(df->name,argv[1])) {
		    uterm_printf(uterm,"## %s\n\n%s\n\n%s\n\n",df->name,df->help,df->long_help);
	       }
	  } else {
	       uterm_printf(uterm,"%-20s %s\n",df->name,df->help);
	  }
     }
}
void harkd_cmd_device(uterm_t *u,void **udata,int argc,char *argv[]) {
     harkd_t *harkd = NULL; const harkd_def_t *def;
     switch (argc) {
     case 1:
	  uterm_printf
	       (u,"device [NAME|list] [DRIVER|COMMAND|delete] [search] [OPTIONS...]\n");
	  break;
     case 2:
	  if(!strcasecmp(argv[1],"list")) {
	       FOREACH_DEVICE(d) {
		    if(u!=uterm(d)) continue;
		    uterm_printf(u,"%-20s %-20s %s\n",d->name,d->def->name,d->portname);
	       }
	  } else if((harkd = harkd_get(argv[1],u))) {
	       char *help[] = {"help",NULL};
	       harkd_run(harkd,help);
	  } else {
	       harkd_cmd_driver(u,udata,argc-1,argv+1);
	  }
	  break;
     default:
	  def = harkd_get_def(argv[2]);
	  if(def) {
	       if(!argv[3]) {
		    char buffer[512];
		    harkd_list_ports(def,buffer,sizeof(buffer)-1);
		    if(!buffer[0]) {
			 uterm_printf(u,"No available ports.\n");
		    } else {
			 uterm_printf(u,"%s",buffer);
		    }
	       } else if(!strcasecmp(argv[3],"search")) {
		    harkd = harkd_new(argv[1],def,NULL,argv+4,u);
	       } else {
		    harkd = harkd_new(argv[1],def,argv[3],argv+4,u);
	       }
	  } else if(!(harkd = harkd_get(argv[1],u))) {
	       uterm_errorf(u,"Device `%s` not found.",argv[1]);
	  } else if(!strcasecmp(argv[2],"delete")) {
	       harkd_free(harkd);
	  } else {
	       harkd_run(harkd,argv+2);
	  }
	  break;
     }
}
void harkd_cmd_table(uterm_t *u,void **udata,int argc,char *argv[]) {
     if(argc==1) {
	  uterm_printf(u,"%s DEV/VAR[=VAL] ...\n",argv[0]);
	  return;
     }
     int pl = 0;
     for(char **argp=argv+1;*argp;argp++) {

	  
	  char    *dev   = strtok(*argp,"/");
	  harkd_t *harkd = harkd_get(dev,u);
	  if(!harkd) {
	       uterm_errorf(u,"Device `%s` not found.",dev);
	       continue;
	  }
	  double dval;
	  char  *svar = strtok(NULL,"=");
	  if(!svar) {
	       harkd_errorf(harkd,"Specify a variable.");
	       return;
	  }
	  for(char *p=svar;*p;p++) *p = toupper(*p);
	  char  *sval = strtok(NULL,"\n");
	  if(sval) {
	       dval = atof(sval);
	       if(harkd_var_set(harkd,svar,&dval)!=HARKD_OK) return;
	  } else {
	       if(harkd_var_get(harkd,svar,&dval)!=HARKD_OK) return;
	       uterm_printf(u,"%10.4f,",dval); pl = 1;
	  }
     }
     if(pl) uterm_printf(u,"\n");
}
void harkd_cmd_test(uterm_t *u,void **udata,int argc,char *argv[]) {
     FOREACH_TEST(t) {
	  if(argc==1) {
	       uterm_printf(u,"%-15s %s\n",t->name,t->description);
	  } else if((strcasecmp(t->name,argv[1]))) {

	  } else {
	       harkd_test_run(u,t,argv+2);
	  }
     }
}


uterm_db_t HARKD_COMMANDS[] = {
     {0,"ports",harkd_cmd_ports,"List available ports."             ,NULL,NULL},
     {0,"device" ,harkd_cmd_device ,"Open device."                      ,NULL,NULL},
     {0,"table"  ,harkd_cmd_table  ,"Get/set values from/to the device" ,NULL,NULL},
     {0,"driver" ,harkd_cmd_driver ,"Show available device drivers."    ,NULL,NULL},
     {0,"test"   ,harkd_cmd_test   ,"Run a predefined test."            ,NULL,NULL},
     {0,""}
};

int main(int argc,char *argv[]) { return main_func(argc,argv); }
