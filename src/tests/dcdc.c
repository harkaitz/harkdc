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
#ifndef NL
#  define NL "\n"
#endif
harkd_r harkd_dcdc_test (harkd_test_var_t *vars) {
     harkd_dev_obj_t *l=NULL,*s=NULL; harkd_table_t *t = NULL;
     double Vin=0,Imax=0; int N=1,Tstep=0;
     double Istep=0,Isupply=0,Vsupply=0,Iload=0,Vload=0,Imaxin=5;
     const char
	  *Iout   = "I1",
	  *Vout   = "V1",
	  *Output = "dcdc.xlsx";
     FOREACH_TEST_VARIABLES(v,vars) {
	  if(!strcasecmp(v->name,"Vin")) {
	       Vin = atof(v->value);
	  } else if(!strcasecmp(v->name,"Imax")) {
	       Imax = atof(v->value);
	  } else if(!strcasecmp(v->name,"Imaxin")) {
	       Imaxin = atof(v->value);
	  } else if(!strcasecmp(v->name,"N")) {
	       N = atoi(v->value);
	  } else if(!strcasecmp(v->name,"Tstep")) {
	       Tstep = atoi(v->value);
	  } else if(!strcasecmp(v->name,"output")) {
	       Output = v->value;     
	  } else if(!strcasecmp(v->name,"CH")) {
	       int num = atoi(v->value);
	       if(num==1) {
		    Iout = "I1";
		    Vout = "V1";
	       } else if(num==2) {
		    Iout = "I2";
		    Vout = "V2";
	       } else {
		    goto error;
	       }
	  } else {
	       harkd_error(NULL,"Invalid variable `%s`.",v->name);
	       goto error;
	  }
     }
     l = harkd_new("load"  ,harkd_get_itf("ARRAY-371X"),NULL,NULL);
     s = harkd_new("supply",harkd_get_itf("MPD-3305D") ,NULL,NULL);
     if(!l) { harkd_error(NULL,"Can't find a compatible load."  ); goto error; }
     if(!s) { harkd_error(NULL,"Can't find a compatible supply."); goto error; }
     t = harkd_table_new(Output);
     harkd_table_add_string(t,"I(in)");
     harkd_table_add_string(t,"V(in)");
     harkd_table_add_string(t,"I(out)");
     harkd_table_add_string(t,"V(out)");
     harkd_table_add_string(t,"P(in)");
     harkd_table_add_string(t,"P(out)");
     
     harkd_table_add_chart(t,"V(out);I(out)","Sheet1!$D$2:$D$%i;Sheet1!$C$2:$C$%i",N+1,N+1);
     harkd_table_next(t);
     Istep = Imax/N*1;
     harkd_var_set(s,Iout,&Imaxin);
     harkd_var_set(s,Vout,&Vin);
     for(double I=0;I<Imax;I+=Istep) {
	  harkd_var_set(l,"I",&I);
	  harkd_wait(Tstep);
	  harkd_var_get(l,"I",&Iload);
	  harkd_var_get(l,"V",&Vload);
	  harkd_var_get(s,Iout,&Isupply);
	  harkd_var_get(s,Vout,&Vsupply);
	  harkd_table_add(t,Isupply);
	  harkd_table_add(t,Vsupply);
	  harkd_table_add(t,Iload);
	  harkd_table_add(t,Vload);
	  /*
	  int r = harkd_table_row(t)+1;
	  harkd_table_add_formula(t,"=A%i*B%i",r,r);
	  harkd_table_add_formula(t,"=C%i*D%i",r,r);
	  */
	  harkd_table_add(t,Isupply* Vsupply);
	  harkd_table_add(t,Iload  * Vload);
	  harkd_table_next(t);
     }
     if(s) harkd_free(s);
     if(l) harkd_free(l);
     if(t) harkd_table_free(t);
     return HARKD_OK;
error:
     if(l) harkd_free(l);
     if(s) harkd_free(s);
     return HARKD_ERR;
}
const harkd_test_t HARKD_DCDC_TEST = {
     "DC/DC",
     "Measure the efficiency of a DC/DC converter.",
     
     "This test measures the efficiency of a DC/DC converter" NL
     "using an `electronic load` and a `DC power supply`."    NL
     ,
     {
	  {"Vin"    ,"12"   ,"V (List separated by commas)"},
	  {"Imax"   ,"2"    ,"A"},
	  {"N"      ,"10"   ,"",
	   {"10","20","40","80",NULL}},
	  {"Tstep"  ,"2000"     ,"ms (Time between measurements)"},
	  {"CH"     ,"1"        ,"Channel to use."},
	  {"Output" ,"dcdc.xlsx","Output file."},
	  {NULL}
     },
     harkd_dcdc_test
};
