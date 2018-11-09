#include "harkd.h"
#include "devices/devices.h" /* harkd_device_list  */
#include "tests/tests.h"     /* harkd_tests        */
#include <hitz/hitz.h>
#include <hitz/hitz-commands.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

hitz_t                 *hitz = NULL;
const harkd_dev_itf_t **harkd_dev_itf_array = harkd_device_list;
const harkd_test_t    **harkd_test_array    = harkd_tests;
harkd_dev_obj_t        *harkd_dev_obj_list  = NULL;
extern hitz_module_t   *HARKD_HITZ_MODULE;

/* ------------------------------------------------------------------------------------------------
 * ---- LIBRARY MANAGEMENT ------------------------------------------------------------------------
 * ------------------------------------------------------------------------------------------------ */
static int harkd_library_inited = 0;
static hitz_t s_hitz;
void harkd_library_init(hitz_t *h) {
     if(harkd_library_inited++) return;
     hitz_init(&s_hitz,NULL,NULL);
     hitz_add (&s_hitz,HARKD_HITZ_MODULE);
     hitz_add (&s_hitz,&HITZ_MODULE);
     if(h) {
	  hitz = h;
	  hitz_add (h,HARKD_HITZ_MODULE);
	  hitz_add (h,&HITZ_MODULE);
     }
     return;
}
void harkd_library_clean(void) {
     if(--harkd_library_inited) return;
}
const harkd_dev_itf_t *harkd_get_itf_search(const char *name) {
     for(int i=0;harkd_dev_itf_array[i];i++) {
	  return harkd_dev_itf_array[i];
     }
     return NULL;
}
hitz_t *harkd_hitz(void) {
     return (hitz)?hitz:&s_hitz;
}



const harkd_dev_itf_t *harkd_get_itf(const char *name) {
     HARKD_ARRAY_FOREACH(const harkd_dev_itf_t,i,harkd_dev_itf_array) {
	  if(!strcasecmp(i->name,name)) {
	       return i;
	  }
     }
     return NULL;
}







static __thread char  harkd_errstr[512] = {0};
const char *harkd_strerror(void) { return  harkd_errstr; }
harkd_r harkd_log(harkd_r type,const char *format,...) {
     va_list args;
     va_start(args,format);
     if(type > 0) {
	  if(getenv("harkd_DEBUG")) {
	       fprintf(stderr,"harkd: [D] ");
	       vfprintf(stderr,format, args);
	       fprintf(stderr,"\n");
	  }
     } else if(type == 0) {
	  fprintf(stderr,"harkd: [I] ");
	  vfprintf(stderr,format, args);
	  fprintf(stderr,"\n");
     } else if(type < 0) {
	  vsnprintf (harkd_errstr,sizeof(harkd_errstr)-1,format, args);
	  if(getenv("harkd_DEBUG")) {
	       fprintf(stderr,"harkd: [E] %s\n",harkd_errstr);
	  }
	  hitz_fprintf(harkd_hitz(),2,"harkd: error: %s\n",harkd_errstr);
     }
     va_end(args);
     return type;
}
