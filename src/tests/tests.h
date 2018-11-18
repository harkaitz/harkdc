/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
/* List of `const harkd_test_t` typed variables.*/
extern const harkd_test_t HARKD_DCDC_TEST;
extern const harkd_test_t HARKD_EXAMPLE_TEST;
const harkd_test_t *harkd_tests[] = {
    &HARKD_DCDC_TEST,
    &HARKD_EXAMPLE_TEST,
    NULL
};
