#include "../harkd-device.h"
#include <string.h>
typedef struct {
     double var1;
} harkd_example_device_t ;
harkd_r harkd_example_init(harkd_t *harkd,const char *port,char *args[]) {
     harkd_example_device_t *udata = harkd_udata(harkd);
     if(args[0]) { udata->var1 = atof(args[0]); }
     harkd_printf(harkd,"Starting example device...\n");
     return HARKD_OK;
}
harkd_r harkd_example_clear(harkd_t *harkd) {
     harkd_printf(harkd,"Deleting example device...\n");
     return HARKD_OK;
}
harkd_r harkd_example_command(harkd_t *harkd,char *args[]) {
     if(!strcasecmp(args[0],"hello")) {
	  for(int i=1;args[i];i++) harkd_printf(harkd," %s",args[i]);
	  harkd_printf(harkd,"\n");
	  return HARKD_OK;
     } else {
	  harkd_errorf_invalid_command(harkd,args[0]);
	  return HARKD_ERR;
     }
}
harkd_r harkd_example_set (harkd_t *harkd,const char *var,double *val) {
     if(!strcasecmp(var,"var1")) {
	  harkd_example_device_t *d = harkd_udata(harkd);
	  d->var1 = *val;
	  return HARKD_OK;
     } else {
	  harkd_errorf_invalid_variable(harkd,var);
	  return HARKD_ERR;
     }
}
harkd_r harkd_example_get (harkd_t *harkd,const char *var,double *val) {
     if(!strcasecmp(var,"var1")) {
	  harkd_example_device_t *d = harkd_udata(harkd);
	  *val = d->var1;
	  return HARKD_OK;
     } else {
	  harkd_errorf_invalid_variable(harkd,var);
	  return HARKD_ERR;
     }
}




const harkd_def_t HARKD_DEVICE_EXAMPLE = {
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
