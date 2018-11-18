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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#ifndef HARKD_COPYRIGHT
#  define HARKD_COPYRIGHT "Copyright (c) 2018, Harkaitz Agirre ezama, All rights reserved."
#endif
static const harkd_test_t *test      = NULL;
static const char        **test_opts = NULL;
void *main_client (hitz_t *hitz) {
     
     if(test) {
	  harkd_test_run(test,test_opts);
     } else {
	  hitz_loop_and_clean(hitz);
     }
     
     return NULL;
}
int main_func(int argc,const char *argv[]) {
     const char *help =
	  "Usage: harkdc [TEST ARGS...]"                           NL
	  ""                                                       NL
	  "HARKDC is a tool to manage some electronic instruments" NL
	  "programatically with a PC."                             NL
	  ""                                                       NL
	  HARKD_COPYRIGHT                                          NL
	  ""                                                       NL
	  ;
     const char *wellcome =
	  "  ___________                                                  " NL
	  " | .^.^.^.^. | HARKD Terminal Interface                        " NL
	  " |-----.-----|                                                 " NL
	  " |HARKD: .oo | Type `help` to see a list of commands.          " NL
	  " |-----' .oo |                                                 " NL
	  " |___________| Run the program with `-h` argument for runtime  " NL
	  "               options.                                        " NL
	  "                                                               " NL
	  HARKD_COPYRIGHT NL
	  ""                                                                NL
	  ;
     hitz_t hitz; int ret = 1;
     hitz_init(&hitz,NULL,NULL);
     harkd_library_init(&hitz);
     if(argc>1) {
	  if(!strcasecmp(argv[1],"-h")) {
	       printf("%s",help);
	  } else {
	       test_opts = argv+2;
	       HARKD_ARRAY_FOREACH(const harkd_test_t,t,harkd_test_array) {
		    if(!strcasecmp(t->name,argv[1])) { test = t; }
	       }
	       if(!test) {
		    HARKD_ARRAY_FOREACH(const harkd_test_t,t,harkd_test_array) {
			 hitz_fprintf(&hitz,1,"%-15s %s\n",t->name,t->description);
		    }
		    hitz_fprintf(&hitz,1,"Please select a valid test.");
		    goto err;
	       }
	       main_client (&hitz);
	  }
     } else {
	  hitz_fprintf(&hitz,HITZ_STDTTY,"%s" NL,wellcome);
	  main_client (&hitz);
	  
     }
     ret = 0;
err:
     harkd_library_clean();
     fflush(stdout);
     fflush(stderr);
     return ret;
}

int main(int argc,const char *argv[]) { return main_func(argc,argv); }

