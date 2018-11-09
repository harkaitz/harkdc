#ifndef _HARKD_H_
#define _HARKD_H_
#include <stddef.h>
#define HARKD_MAX_NAME 20

typedef struct hitz_s        hitz_t;
typedef struct sp_port       sp_port_t;
typedef struct harkd_test_s  harkd_test_t;
typedef struct harkd_table_s harkd_table_t;
typedef struct hitz_module_s hitz_module_t;
typedef int harkd_r;
#define HARKD_OK   1
#define HARKD_ERR -1


typedef struct harkd_dev_obj_s       harkd_dev_obj_t;



void harkd_library_init  (hitz_t *opt_hitz);
void harkd_library_clean (void);


/**
 * Some usefull macros.
 */
#define HARKD_LIST_FOREACH(TYPE,CURR,LIST)	  \
     for (TYPE **_nxt = &(LIST),*CURR = (LIST);   \
	  CURR;					  \
	  _nxt=&((*_nxt)->next),CURR=*(_nxt))
#define HARKD_ARRAY_FOREACH(TYPE,CURR,LIST) \
     for(TYPE **_nxt=(LIST),*CURR=*_nxt;CURR;_nxt++,CURR=*_nxt)

#ifndef NL
#  define NL "\n"
#endif





/**
 * A structure to represent a device's interface.
 */
typedef struct harkd_dev_itf_s   harkd_dev_itf_t;
typedef harkd_r (*harkd_init_f) (harkd_dev_obj_t *harkd,const char *port,const char *args[]);
typedef harkd_r (*harkd_cmd_f)  (harkd_dev_obj_t *harkd,const char *args[]);
typedef harkd_r (*harkd_opr_f)  (harkd_dev_obj_t *harkd);
typedef harkd_r (*harkd_set_f)  (harkd_dev_obj_t *harkd,const char *var,double *val);
struct harkd_dev_itf_s {
     const char *name;  /**< The name for `harkd_get_def` */
     const char *help;  /**< A descriptive help. */
#    define harkd_help_paragraph(TXT)          TXT "\n\n"
#    define harkd_help_inputs(TXT)             "INPUTS  : " TXT "\n"
#    define harkd_help_outputs(TXT)            "OUTPUTS : " TXT "\n"
#    define harkd_help_commands()              "\n### Commands\n\n"
#    define harkd_help_command(NAME,ARGS,HELP) "```\n" NAME " " ARGS ": " HELP "\n```\n"
     char        port_type;
     const char *long_help;
     struct {
	  int          len;
	  harkd_init_f init;
	  harkd_cmd_f  command;
	  harkd_opr_f  clear;
	  harkd_set_f  set,get;
     } fn;
};
extern const harkd_dev_itf_t **harkd_dev_itf_array;
const harkd_dev_itf_t *harkd_get_itf(const char *name);







void harkd_list_ports     (const harkd_dev_itf_t *device,char buffer[],int len);
int  harkd_port_is_openned(const char *portname);
void harkd_wait           (int ms);






#define HARKD_MAX_STRING  64
#define HARKD_MAX_TEXT    1024



/**
 * A structure to represent a device's. (harkd.c)
 */
struct harkd_dev_obj_s {
     hitz_t                *hitz;
     char                   name[HARKD_MAX_STRING];
     const harkd_dev_itf_t *itf;
     
     sp_port_t             *port;
     char                   portname[HARKD_MAX_STRING];
     harkd_dev_obj_t       *next;
     char                  *itf_data[1];
};
extern harkd_dev_obj_t *harkd_dev_obj_list;


harkd_dev_obj_t *harkd_new        (const char *name,const harkd_dev_itf_t *device,
				   const char *port,const char *options[]);
harkd_dev_obj_t *harkd_get        (const char *name);
void             harkd_free       (harkd_dev_obj_t *harkd);
harkd_r          harkd_run        (harkd_dev_obj_t *harkd,const char *args[]);
harkd_r          harkd_var_set    (harkd_dev_obj_t *harkd,const char *var,double *val);
harkd_r          harkd_var_get    (harkd_dev_obj_t *harkd,const char *var,double *val);
void             harkd_close      (harkd_dev_obj_t *harkd);
int              harkd_is_open    (harkd_dev_obj_t *harkd);
int              harkd_port_is_openned(const char *portname);
#define          harkd_itf_data(H,T) ((T*) (H)->itf_data)










/**
 * (harkd-table.c)
 */
harkd_table_t *harkd_table_new        (const char *filename);
void           harkd_table_free       (harkd_table_t *t);
void           harkd_table_add        (harkd_table_t *t,double d);
void           harkd_table_add_string (harkd_table_t *t,const char *s);
void           harkd_table_add_formula(harkd_table_t *t,const char *s,...);
void           harkd_table_next  (harkd_table_t *t);
void           harkd_table_next_column(harkd_table_t *t);
int            harkd_table_column(harkd_table_t *t);
int            harkd_table_row   (harkd_table_t *t);
void           harkd_table_add_chart (harkd_table_t *t,const char *names,const char *yx,...);






/* ----------------- SERIAL PORT -------------------------------- */
harkd_r    harkd_serial_ports    (char *o_ports,int o_len);
sp_port_t *harkd_serial_open     (harkd_dev_obj_t *harkd,const char *portname);
harkd_r    harkd_serial_write    (harkd_dev_obj_t *harkd,void *buffer,size_t len);
harkd_r    harkd_serial_read     (harkd_dev_obj_t *harkd,void *buffer,size_t len);
char      *harkd_serial_gets     (harkd_dev_obj_t *harkd,char *buffer,size_t len);
harkd_r    harkd_serial_puts     (harkd_dev_obj_t *harkd,char *buffer);
void       harkd_set_serial_error(harkd_dev_obj_t *harkd,const char *whence);







/* ------------------- TESTS ------------------------------------- */
typedef struct harkd_test_var_s harkd_test_var_t;
struct harkd_test_var_s {
     const char *name;
     const char *value;
     const char *help;
     const char *opt_choises[10];
};






typedef struct harkd_test_s     harkd_test_t;
struct harkd_test_s {
     const char *name;
     const char *description;
     const char *long_description;
     const harkd_test_var_t opts[20];
     harkd_r (*fun) (harkd_test_var_t *opts);
};
extern const harkd_test_t  **harkd_test_array;
harkd_r              harkd_test_run(const harkd_test_t *t,const char *opts[]);
#define FOREACH_TEST(T) FOREACH_LIST(harkd_test_t,T,harkd_test_list)
#define FOREACH_TEST_VARIABLES(V,VARS) for(harkd_test_var_t *V = VARS;V->name;V++)





extern hitz_module_t *HARKD_HITZ_MODULE;





/* ------------------ ERROR REPORTING ----------------------------------------------- */
const char *harkd_strerror(void);
harkd_r     harkd_log(harkd_r type,const char *format,...);
hitz_t     *harkd_hitz(void);
#define harkd_error_invalid_variable(H,VAR) harkd_log(HARKD_ERR,"Invalid variable `%s`.",VAR)
#define harkd_error_invalid_command(H,VAR) harkd_log(HARKD_ERR,"Invalid variable `%s`.",VAR)
#define harkd_error(H,...) harkd_log(HARKD_ERR,##__VA_ARGS__)
#define harkd_printf(...)  hitz_fprintf(harkd_hitz(),##__VA_ARGS__)

#endif
