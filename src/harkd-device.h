/**
 * @file harkd-device.h
 * @description Device models must specify `long_help` using this
 * template. Also should raise error with this templates.
 */
#ifndef _HARKD_DEVICE_H_
#define _HARKD_DEVICE_H_
#include "harkd.h"
#include <uterm/uterm.h>



/* ---------------------------------------------------------------------------- */
harkd_r    harkd_serial_ports    (uterm_t *uterm,char *o_ports,int o_len);
sp_port_t *harkd_serial_open     (harkd_t *harkd,const char *portname);
harkd_r    harkd_serial_write    (harkd_t *harkd,void *buffer,size_t len);
harkd_r    harkd_serial_read     (harkd_t *harkd,void *buffer,size_t len);
char      *harkd_serial_gets     (harkd_t *harkd,char *buffer,size_t len);
harkd_r    harkd_serial_puts     (harkd_t *harkd,char *buffer);
void       harkd_set_serial_error(harkd_t *harkd);




/* ---------------------------------------------------------------------------- */
#define harkd_errorf(H,FMT,...) \
     uterm_errorf(uterm(H),"%s: " FMT,harkd_name(H),##__VA_ARGS__)
#define harkd_printf(H,FMT,...) \
     uterm_printf(uterm(H),FMT,##__VA_ARGS__)


/* ---------------------------------------------------------------------------- */
#define harkd_errorf_invalid_variable(H,VAR) \
     harkd_errorf(H,"Variable `%s` is not supported.",VAR)
#define harkd_errorf_invalid_command(H,CMD) \
     harkd_errorf(H,"Command `%s` is not supported.",CMD)
#define harkd_errorf_invalid_driver(H,PORT) \
     harkd_errorf(H,"The machine `%s` is not attached to `%s`.",(H)->def->name,PORT)


/* ---------------------------------------------------------------------------- */
#define harkd_help_paragraph(TXT) TXT "\n\n"
#define harkd_help_inputs(TXT)      "INPUTS  : " TXT "\n"
#define harkd_help_outputs(TXT)     "OUTPUTS : " TXT "\n"
#define harkd_help_commands() "\n### Commands\n\n"
#define harkd_help_command(NAME,ARGS,HELP) \
     "```\n" NAME " " ARGS ": " HELP "\n```\n"

#endif
