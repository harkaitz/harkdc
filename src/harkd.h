#ifndef _HARKD_H_
#define _HARKD_H_
#include <stddef.h>
#define HARKD_MAX_NAME 20
typedef struct harkd_s      harkd_t;
typedef struct harkd_def_s  harkd_def_t;
typedef struct uterm_s      uterm_t;
typedef struct uterm_db_s   uterm_db_t;
typedef struct sp_port      sp_port_t;
typedef struct harkd_test_s harkd_test_t;
typedef enum {
     HARKD_OK  = 0,
     HARKD_ERR = -1
} harkd_r;
typedef harkd_r (*harkd_init_f) (harkd_t *harkd,const char *port,char *args[]);
typedef harkd_r (*harkd_cmd_f)  (harkd_t *harkd,char *args[]);
typedef harkd_r (*harkd_opr_f)  (harkd_t *harkd);
typedef harkd_r (*harkd_set_f)  (harkd_t *harkd,const char *var,double *val);
struct harkd_def_s {
     const char *name;
     const char *help;
     char        port_type;
     const char *long_help;
     struct {
	  int len;
	  harkd_init_f init;
	  harkd_cmd_f  command;
	  harkd_opr_f  clear;
	  harkd_set_f  set,get;
     } fn;
};
struct harkd_s {
     uterm_t           *uterm;
     char               name[HARKD_MAX_NAME];
     const harkd_def_t *def;
     void              *udata;
     sp_port_t         *port;
     char               portname[64];
};





void               harkd_init     (void);
void               harkd_clean    (void);
const harkd_def_t *harkd_get_def  (const char *name);
void               harkd_list_ports(const harkd_def_t *device,char buffer[],int len);

harkd_t           *harkd_list_devices(int start,harkd_t     **r);
harkd_def_t       *harkd_list_drivers(int start,harkd_def_t **r);

#define FOREACH_DEVICE_DEFS(DF) \
     for(harkd_def_t *r,*DF=harkd_list_drivers(1,&r);DF;DF=harkd_list_drivers(0,&r))
#define FOREACH_DEVICE(D) \
     for(harkd_t *r,*D=harkd_list_devices(1,&r);D;D=harkd_list_devices(0,&r))






/* -------------------------------------------------------------- */
harkd_t *harkd_new      (const char *name,const harkd_def_t *device,
			 const char *port,
			 char *options[],uterm_t *opt_uterm);
harkd_t *harkd_get      (const char *name,uterm_t *opt_uterm);
void     harkd_free     (harkd_t *harkd);
void    *harkd_udata    (harkd_t *harkd);
const char *harkd_name  (harkd_t *harkd);
void     harkd_set_uterm(harkd_t *harkd,uterm_t *uterm);
harkd_r  harkd_run      (harkd_t *harkd,char *args[]);
harkd_r  harkd_var_set  (harkd_t *harkd,const char *var,double *val);
harkd_r  harkd_var_get  (harkd_t *harkd,const char *var,double *val);



void     harkd_close    (harkd_t *harkd);
int      harkd_is_openned(harkd_t *harkd);
/* -------------------------------------------------------------- */

int      harkd_port_is_openned(uterm_t *uterm,const char *portname);
void     harkd_wait(int ms);





#endif
