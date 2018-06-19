#include "../harkd-test.h"
#include <string.h>
harkd_r harkd_dcdc_test (uterm_t *u,harkd_test_var_t *vars) {
     harkd_t *l=NULL,*s=NULL;
     double Vin=0,Imax=0; int N=1,Tstep=0;
     double Istep=0,Isupply=0,Vsupply=0,Iload=0,Vload=0;
     const char
	  *fmt1 = "%-15s,%-15s,%-15s,%-15s\n",
	  *fmt2 = "%-15f,%-15f,%-15f,%-15f\n",
	  *Iout = "I1",
	  *Vout = "V1";
     FOREACH_TEST_VARIABLES(v,vars) {
	  if(!strcasecmp(v->name,"Vin")) {
	       Vin = atof(v->value);
	  } else if(!strcasecmp(v->name,"Imax")) {
	       Imax = atof(v->value);
	  } else if(!strcasecmp(v->name,"N")) {
	       N = atoi(v->value);
	  } else if(!strcasecmp(v->name,"Tstep")) {
	       Tstep = atoi(v->value);
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
	       uterm_errorf(u,"Invalid variable `%s`.",v->name);
	       goto error;
	  }
     }
     l = harkd_new("load"  ,harkd_get_def("ARRAY-371X"),NULL,NULL,u);
     s = harkd_new("supply",harkd_get_def("MPD-3305D") ,NULL,NULL,u);
     if(!l) { uterm_errorf(u,"Can't find a compatible load."  ); goto error; }
     if(!s) { uterm_errorf(u,"Can't find a compatible supply."); goto error; }
     uterm_printf(u,fmt1,"I(in)","V(in)","I(out)","V(out)");
     Istep = Imax/N*1;
     harkd_var_set(s,Iout,&Imax);
     harkd_var_set(s,Vout,&Vin);
     for(double I=0;I<Imax;I+=Istep) {
	  harkd_var_set(l,"I",&I);
	  harkd_wait(Tstep);
	  harkd_var_get(s,Iout,&Isupply);
	  harkd_var_get(s,Vout,&Vsupply);
	  harkd_var_get(l,"I",&Iload);
	  harkd_var_get(l,"V",&Vload);
	  uterm_printf(u,fmt2,Isupply,Vsupply,Iload,Vload);
	  
     }
     if(s) harkd_free(s);
     if(l) harkd_free(l);
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
	  {"Imax"   ,"1"    ,"A"},
	  {"N"      ,"10"   ,"",
	   {"10","20","40","80",NULL}},
	  {"Tstep"  ,"1500" ,"ms (Time between measurements)"},
	  {"CH"     ,"1"    ,"Channel to use."},
	  {NULL}
     },
     harkd_dcdc_test
};
