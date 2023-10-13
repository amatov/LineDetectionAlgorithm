#include <math.h>
#include <tk8.0.h>
#include <tcl8.0.h>

#include "leon.h"
#include "leon_tcltk.h"
#include "utils.h"
#include "math_functions.h"

#define LINE_TAG "line"
#define NO_TAG "good"
#define START_POINT "start"

/***********************************************************************/
/* Create graphical window (TK canvas)                                 */
/***********************************************************************/
void line_detect_tk_init_window(struct_ldetect *LDT) {
  int code;
  char str[20];
  /* create Tcl variables */
  sprintf(str,"set DIM_X %d",LDT->image.dim.x);
  Tcl_Eval(LDT->TK->interp, str);
  sprintf(str,"set DIM_Y %d",LDT->image.dim.y);
  Tcl_Eval(LDT->TK->interp, str);
  Tcl_Eval(LDT->TK->interp, "set LDT_F .ldt");
  Tcl_Eval(LDT->TK->interp, "set LDT_C .canvas");
  strcpy (LDT->TK->canvas_tag, ".ldt.canvas");

  TK_do_all_events();
  /* read & eval tcl script ldetect.tcl */
  code = Tcl_EvalFile(LDT->TK->interp, "line_detect.tcl");
  if (code != TCL_OK) exit_with_error("Can't read or eval line_detect.tcl");
  TK_do_all_events();

  /* draw clusters' edges */
  if (LDT->cluster) {
    int cl = LDT->cluster;
    point_2d_int begin,end;
    for (int i=1; i<cl; i++) {
       begin.x = end.x = i*LDT->image.dim.x/cl;
       begin.y = 0;
       end.y = LDT->image.dim.y-1;
      TK_create_line(LDT->TK, begin, end, CLUSTER_EDGES_COLOR);
       begin.x = 0;
       begin.y = end.y = i*LDT->image.dim.y/cl;
       end.x = LDT->image.dim.x-1;
      TK_create_line(LDT->TK, begin, end, CLUSTER_EDGES_COLOR);
    }
  }
}

/***********************************************************************/
/* Create line in Tk canvas with eq. coeff. (ax+by+c=0)	               */
/***********************************************************************/
void line_detect_tk_create_line (struct_tk *tk, float *eq_coeff,
				 point_2d_int dim, char *color, char *tag)
{
  char str[150];
  int x0,y0,x1,y1;
  float X = (float)--dim.x/2;
  float Y = (float)--dim.y/2;
  float a = eq_coeff[0];
  float b = eq_coeff[1];
  float c = eq_coeff[2];

  if (a==0 && b==0) exit_with_error("Can create line (TK_create_line with a=b=0)");
  if (c==0) c=sqrt(a*a+b*b)/10000;

  a /= c;
  b /= c;
  c = 1;

  if (a==0 && b!=0) { /* horizontal line */
    x0=0;
    x1=dim.x;
    y1=y0=round_to_int(-1/b + Y);
  }
  if (b==0 && a!=0) { /* vertical line */
    y0=0;
    y1=dim.y;
    x1=x0=round_to_int(-1/a + X);
  }
  if (a!=0 && b!=0) {
    if (ABS((-1+a*X)/b) <= Y) {
      x0=0;
      y0=round_to_int((-1+a*X)/b + Y);
    }
    else if (ABS((-1+b*Y)/a) <= X) {
      x0=round_to_int((-1+b*Y)/a + X);
      y0=0;
    }
    else if (ABS(-(1+a*X)/b) <= Y) {
      x0=dim.x;
      y0=round_to_int(-(1+a*X)/b + Y);
    }

    if (ABS(-(1+b*Y)/a) <= X) {
      x1=round_to_int(-(1+b*Y)/a + X);
      y1=dim.y;
    }
    else if (ABS(-(1+a*X)/b) <= Y) {
      x1=dim.x;
      y1=round_to_int(-(1+a*X)/b + Y);
    }
    else if (ABS((-1+b*Y)/a) <= X) {
      x1=round_to_int((-1+b*Y)/a + X);
      y1=0;
    }
    else if (ABS((-1+a*X)/b) <= Y) {
      x1=0;
      y1=round_to_int((-1+a*X)/b + Y);
    }
  }

  sprintf(str, "%s create line %d %d %d %d -fill %s -tag %s", tk->canvas_tag,
	  x0, y0, x1, y1, color, tag);
  Tcl_Eval(tk->interp, str);
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

/***********************************************************************/
/* Create segment in Tk canvas					       */
/***********************************************************************/
void line_detect_tk_create_segment (struct_tk *tk, int *coord,
				    char *color, char *tag)
{
  char str[150];
  sprintf(str, "%s create line %d %d %d %d -fill %s -tag %s", tk->canvas_tag,
	  coord[0], coord[1], coord[2], coord[3], color, tag);
  Tcl_Eval(tk->interp, str);
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

/***********************************************************************/
/* Put starting point in ldetect canvas                                */
/***********************************************************************/
void line_detect_tk_put_start(struct_ldetect *LDT)
{
  line_detect_tk_create_line (LDT->TK, LDT->last_cycle_starting_point,
			      LDT->image.dim, START_POINT_LINE_COLOR,
			      START_POINT);
}

/***********************************************************************/
/* Remove starting point in ldetect canvas                             */
/***********************************************************************/
void line_detect_tk_remove_start(struct_ldetect *LDT)
{
  char str[200];
  sprintf(str, ".ldt.canvas delete %s", START_POINT);
  Tcl_Eval(LDT->TK->interp, str);
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

/***********************************************************************/
/* Update ldetect canvas (create last found subsystem)		       */
/***********************************************************************/
void line_detect_tk_update_canvas(struct_ldetect *LDT,
				  struct_algo *ALGO,
				  struct_algo_param *P_ALGO)
{
  /* create line corresponding to the last found subsystem */
  if (ALGO->last_subsys_size >= P_ALGO->stop_cycles) {
    if (LDT->segment) { /* segment detection */
      line_detect_tk_create_segment (LDT->TK, LDT->segments[ALGO->step-1],
				     GOOD_LINE_COLOR, NO_TAG);
    }
    else { /* no segment detection */
      line_detect_tk_create_line (LDT->TK, LDT->solutions[ALGO->step-1],
				  LDT->image.dim, GOOD_LINE_COLOR, NO_TAG);
    }
  }
  else {
    if (P_ALGO->draw_bad_subsystem)
      line_detect_tk_create_line (LDT->TK, LDT->solutions[ALGO->step-1],
				  LDT->image.dim, ERR_LINE_COLOR, NO_TAG);
  }


  /* put cross corresponding to the current position of origin */
//#ifdef DISPLAY_ORIGIN
  point_2d_int cross;
  cross.x = LDT->image.dim.x/2;
  cross.y = LDT->image.dim.y/2;
  TK_create_cross (LDT->TK, cross, CROSS_COLOR);
//#endif
}

/***********************************************************************/
/* Update ldetect canvas with points fitted by the last found subsys   */
/* (command line option -mon_well_classified)			       */
/***********************************************************************/
void line_detect_tk_flag_classified(struct_tk *TK, point_2d_float point,
				    int index_subsystem)
{ 
  char str[200];
  char *color;
  int s = WELL_CLASSIFIED_PT_SIZE/2;

  if (index_subsystem>=0)
    color = TK_get_subsystem_color(index_subsystem, 0);
  else
    /* the classified points were flagged but the subsystem is rejected */
    color = REJECTED_AFTER_POSTPROCESSING_COLOR;

  sprintf(str,".ldt.canvas create rectangle %d %d %d %d -fill %s -outline %s",
	  (int)point.x-s, (int)point.y-s, (int)point.x+s, (int)point.y+s,
	  color, color);

  Tcl_Eval(TK->interp, str); 
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

void line_detect_tk_flag_non_classified(struct_ldetect *LDT, struct_algo *ALGO)
{
  point_2d_int pt;
  for (int i=0; i<ALGO->nb_couples; i++) {
    pt.x = (int)(LDT->SYST->A[ALGO->equations[i]][0] + LDT->image.dim.x/2);
    pt.y = (int)(LDT->SYST->A[ALGO->equations[i]][1] + LDT->image.dim.y/2);
    TK_create_cross(LDT->TK, pt, NON_CLASSIFIED_POINTS_COLOR);
  }
}

/***********************************************************************/
/* Update ldetect canvas with the line corresponding to the current    */
/* subsystem (real time graphical monitoring)			       */
/***********************************************************************/
void line_detect_tk_update_mon_canvas(struct_ldetect *LDT, float *x)
{
  char str[100];

  /* delete last */
  sprintf(str, ".ldt.canvas delete %s", LINE_TAG);
  Tcl_Eval(LDT->TK->interp, str);
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);

  /* draw new one */
  line_detect_tk_create_line
    (LDT->TK, x,LDT->image.dim, GOOD_LINE_COLOR, LINE_TAG);
}

#undef NO_TAG
#undef LINE_TAG
