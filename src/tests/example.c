#include "../harkd.h"
#include <hitz/hitz.h>
harkd_r harkd_example_test (harkd_test_var_t *vars) {
     FOREACH_TEST_VARIABLES(v,vars) {
	  harkd_printf(1,"%s=%s\n",v->name,v->value);
     }
     return HARKD_OK;
}
const harkd_test_t HARKD_EXAMPLE_TEST = {
     "example",
     "Simply prints the variables specified.",
     "Simply prints the variables specified.",
     {{"VAR1","VAL1"},
      {NULL  ,NULL  }},
     harkd_example_test
};
