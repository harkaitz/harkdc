/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
/* List of `const harkd_dev_itf_t` typed variables.*/
extern const harkd_dev_itf_t HARKD_DEVICE_ARRAY_371X;
extern const harkd_dev_itf_t HARKD_DEVICE_EXAMPLE;
extern const harkd_dev_itf_t HARKD_DEVICE_MPD3305D;
const harkd_dev_itf_t *harkd_device_list[] = {
    &HARKD_DEVICE_ARRAY_371X,
    &HARKD_DEVICE_EXAMPLE,
    &HARKD_DEVICE_MPD3305D,
    NULL
};
