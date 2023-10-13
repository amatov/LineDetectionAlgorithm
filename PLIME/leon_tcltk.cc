#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <tcl8.0.h>
#include <tk8.0.h>
#include <stdlib.h>
#include <math.h>
#include "leon.h"
#include "math_functions.h"
#include "utils.h"
#include "struct.h"

Tcl_Interp* init_TclTk(char *argv_0)
{
  Tk_Window main;
  Tcl_Interp *interp;

  fprintf(stderr,"Init TclTk ...\n");
  Tcl_FindExecutable(argv_0);
  interp = Tcl_CreateInterp();

#ifdef TCL_MEM_DEBUG
  Tcl_InitMemory(interp);
#endif
  /*
    #ifdef __WIN32__
    tty = 1;
    #else
    tty = isatty(0);
    #endif
    Tcl_SetVar(interp, "tcl_interactive",
    ((fileName == NULL) && tty) ? "1" : "0", TCL_GLOBAL_ONLY);
    */
  main = Tk_MainWindow(interp);

  if (Tcl_Init(interp) == TCL_ERROR) exit_with_error("Can't init Tcl");
  if (Tk_Init(interp) == TCL_ERROR) exit_with_error("Can't init Tk");

  return interp;
}

void close_TclTk(Tcl_Interp *interp)
{
  Tk_MainLoop();
  Tcl_DeleteInterp(interp);
  Tcl_Exit(0);
}

void TK_do_all_events()
{
  while (Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT));
}

/***********************************************************************/
/* Create line in Tk canvas with 2 pts coord.			       */
/***********************************************************************/
void TK_create_line (struct_tk *TK, point_2d_int start, point_2d_int end,
		     char *color)
{
  char str[100];
  sprintf(str, "%s create line %d %d %d %d -fill %s", TK->canvas_tag,
	  start.x, start.y, end.x, end.y, color);
  Tcl_Eval(TK->interp, str);
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

/***********************************************************************/
/* Create cross in Tk canvas					       */
/***********************************************************************/
void TK_create_cross(struct_tk *TK, point_2d_int p, char *color)
{
  char str[256];
  sprintf(str, "%s create line %d %d %d %d -fill %s", TK->canvas_tag,
	  p.x-2, p.y, p.x+2, p.y, color);
  Tcl_Eval(TK->interp, str);
  sprintf(str, "%s create line %d %d %d %d -fill %s", TK->canvas_tag,
	  p.x, p.y-2, p.x, p.y+2, color);
  Tcl_Eval(TK->interp, str);
  TK_do_all_events();
}

/***********************************************************************/
/* Get subsystem colors						       */
/***********************************************************************/
char *TK_get_subsystem_color(int color_nb, int brightness)
{
  static char *dark_color[] = SUBSYS_DARK_COLORS;
  static char *light_color[] = SUBSYS_LIGHT_COLORS;
  static int begining_color = rand()%NB_SUBSYSTEM_COLOR;

  if (brightness==0)
    return dark_color[(begining_color+color_nb)%NB_SUBSYSTEM_COLOR];
  else
    return light_color[(begining_color+color_nb)%NB_SUBSYSTEM_COLOR];
}
