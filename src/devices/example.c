/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
#include "../harkd.h"
#include <string.h>
#include <stdlib.h>
#include <hitz/hitz.h>
typedef struct {
     double var1;
} harkd_example_device_t ;
harkd_r harkd_example_init(harkd_dev_obj_t *harkd,const char *port,const char *args[]) {
     harkd_example_device_t *udata = harkd_itf_data(harkd,harkd_example_device_t);
     if(args[0]) { udata->var1 = atof(args[0]); }
     harkd_printf(1,"Starting example device...\n");
     return HARKD_OK;
}
harkd_r harkd_example_clear(harkd_dev_obj_t *harkd) {
     harkd_printf(1,"Deleting example device...\n");
     return HARKD_OK;
}
harkd_r harkd_example_command(harkd_dev_obj_t *harkd,const char *args[]) {
     if(!strcasecmp(args[0],"hello")) {
	  for(int i=1;args[i];i++) harkd_printf(1," %s",args[i]);
	  harkd_printf(1,"\n");
	  return HARKD_OK;
     } else {
	  harkd_error_invalid_command(harkd,args[0]);
	  return HARKD_ERR;
     }
}
harkd_r harkd_example_set (harkd_dev_obj_t *harkd,const char *var,double *val) {
     if(!strcasecmp(var,"var1")) {
	  harkd_example_device_t *d = harkd_itf_data(harkd,harkd_example_device_t);
	  d->var1 = *val;
	  return HARKD_OK;
     } else {
	  harkd_error_invalid_variable(harkd,var);
	  return HARKD_ERR;
     }
}
harkd_r harkd_example_get (harkd_dev_obj_t *harkd,const char *var,double *val) {
     if(!strcasecmp(var,"var1")) {
	  harkd_example_device_t *d = harkd_itf_data(harkd,harkd_example_device_t);
	  *val = d->var1;
	  return HARKD_OK;
     } else {
	  harkd_error_invalid_variable(harkd,var);
	  return HARKD_ERR;
     }
}




const harkd_dev_itf_t HARKD_DEVICE_EXAMPLE = {
     "example","An example device.",'n',
     harkd_help_paragraph(
	  "This is an example virtual device. Driver developers"
	  "can copy paste it's code as a template for new drivers.")
     harkd_help_inputs("VAR1")
     harkd_help_outputs("VAR1")
     harkd_help_commands()
     harkd_help_command("hello","Some text","This command simply echos the input")
     ,
     {
	  
	  sizeof(harkd_example_device_t),
	  harkd_example_init,
	  harkd_example_command,
	  harkd_example_clear,
	  harkd_example_set,
	  harkd_example_get
     }
};
