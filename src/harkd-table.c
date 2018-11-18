/************************************************************************
 * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
 * for commercial use, visit http://sugaar-solutions.com for more information.
 *
 * Copyright 2018 Sugaar Solutions, all rights reserved
 *
 * Authors: Harkaitz Agirre Ezama
 ************************************************************************/
 
#include "harkd.h"
#include <xlsxwriter.h>
#include <hitz/hitz.h>
#include <stdarg.h>
/* -------------------------------- TABLE ------------------------------------ */
struct harkd_table_s {
#    ifndef DISABLE_XLSX
     lxw_workbook  *workbook;
     lxw_worksheet *worksheet;
     lxw_chart     *chart;
#    endif
     int row,col,max_col;
};
harkd_table_t *harkd_table_new(const char *filename) {
     harkd_table_t *t = malloc(sizeof(*t));
#    ifndef DISABLE_XLSX
     t->workbook  = workbook_new(filename);
     t->worksheet = workbook_add_worksheet(t->workbook,NULL);
     t->chart     = NULL;
#    endif
     t->row = 0;
     t->col = 0;
     t->max_col = 0;
     return t;
}
void harkd_table_free(harkd_table_t *t) {
     if(t) {
#         ifndef DISABLE_XLSX
	  if(t->chart) {
	       worksheet_insert_chart(t->worksheet,0,t->max_col, t->chart);
	  }
	  workbook_close(t->workbook);
#         endif
	  free(t);
     }
}
void harkd_table_add(harkd_table_t *t,double d) {
     harkd_printf(1,"%-15f",d);
#    ifndef DISABLE_XLSX
     worksheet_write_number(t->worksheet, t->row, t->col, d, NULL);
#    endif
     harkd_table_next_column(t);
}
void harkd_table_add_string(harkd_table_t *t,const char *s) {
     harkd_printf(1,"%-15s",s);
#    ifndef DISABLE_XLSX
     worksheet_write_string(t->worksheet, t->row, t->col, s, NULL);
#    endif
     harkd_table_next_column(t);
}
void harkd_table_add_formula(harkd_table_t *t,const char *format,...) {
#    ifndef DISABLE_XLSX
     char s[64];
     va_list args;
     va_start(args, format);
     vsprintf(s,format,args);
     va_end(args);
     harkd_printf(1,"%-15s",s);
     worksheet_write_formula(t->worksheet, t->row, t->col, s, NULL);
#    endif
     harkd_table_next_column(t);
}
void harkd_table_next(harkd_table_t *t) {
     t->row++;
     t->col = 0;
     harkd_printf(1,"\n");
     fflush(stdout);
}
void harkd_table_next_column(harkd_table_t *t) {
     harkd_printf(1,",");
     fflush(stdout);
     if((++t->col)>(t->max_col))
	  t->max_col = t->col;
}

int  harkd_table_column(harkd_table_t *t) { return t->col; }
int  harkd_table_row   (harkd_table_t *t) { return t->row; }
void harkd_table_add_chart (harkd_table_t *t,const char *names,const char *yx,...) {
     char s[128];
     va_list args;
     va_start(args, yx);
     vsprintf(s,yx,args);
     va_end(args);
     const char *y = NULL,*x = NULL;
     y = strtok(s,";");
     if(y) x = strtok(NULL,";");
     if(!t->chart) {
	  t->chart = workbook_add_chart(t->workbook, LXW_CHART_LINE);
     }
     /* Configure the chart. */
     chart_add_series(t->chart, x , y);
     if(names) {
	  strcpy(s,names);
	  if((y = strtok(s,";")))
	       x = strtok(NULL,";");
	  else
	       x = NULL;
	  if(y) chart_axis_set_name(t->chart->y_axis,y);
	  if(x) chart_axis_set_name(t->chart->x_axis,x);
     }
}
