#ifndef _HARKD_TEST_H_
#define _HARKD_TEST_H_
#include "harkd.h"
#include <uterm/uterm.h>
typedef struct harkd_test_var_s harkd_test_var_t;
typedef struct harkd_test_s     harkd_test_t;
struct harkd_test_var_s {
     const char *name;
     const char *value;
     const char *help;
     const char *opt_choises[10];
};
struct harkd_test_s {
     const char *name;
     const char *description;
     const char *long_description;
     const harkd_test_var_t opts[20];
     harkd_r (*fun) (uterm_t *u,harkd_test_var_t *opts);
};
#define FOREACH_TEST_VARIABLES(V,VARS) for(harkd_test_var_t *V = VARS;V->name;V++)
const harkd_test_t  *harkd_test_list(int start,const harkd_test_t ***r);
harkd_r              harkd_test_run(uterm_t *u,const harkd_test_t *t,char *opts[]);
#define FOREACH_TEST(T) \
     for(const harkd_test_t **r,*T=harkd_test_list(1,&r);T;T=harkd_test_list(0,&r))

#ifndef NL
#  define NL "\n"
#endif

#endif
