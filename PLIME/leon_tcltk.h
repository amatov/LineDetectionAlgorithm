#ifndef LEON_TCLTK_H
#define LEON_TCLTK_H

#include "tcl.h"
#include "struct.h"

Tcl_Interp* init_TclTk(char *argv_0);
void close_TclTk(Tcl_Interp *interp);
void TK_do_all_events();
void TK_create_line (struct_tk *TK, point_2d_int start, point_2d_int end,
		     char *color);
void TK_create_cross (struct_tk *TK, point_2d_int point, char *color);

char *TK_get_subsystem_color(int color, int brightness);

#endif
