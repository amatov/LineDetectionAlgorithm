#include <math.h>
#include <string.h>

#include <tk8.0.h>

#include "leon.h"
#include "leon_tcltk.h"
#include "utils.h"
#include "math_functions.h"
#include "search_biggest_subsystem_CDT2.h"

#define START_POINT_TAG "start"
#define CONIC_TAG "conic"
#define NO_TAG ""
#define LAST_ERR_CIRCLE_TAG "last_circle"
#define SIZE_TCL_COMMAND 256

#define QUICK_DRAW 1
#define NORMAL_DRAW 0

/***********************************************************************/
/* Create graphical window (TK canvas)				       */
/***********************************************************************/
void conic_detect_tk_init_window(struct_cdetect *CDT)
{
  int code;
  char str[20];
  /* create Tcl variables */
  sprintf(str,"set DIM_X %d",CDT->image.dim.x);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,"set DIM_Y %d",CDT->image.dim.y);
  Tcl_Eval(CDT->TK->interp, str);
  strcpy (CDT->TK->canvas_tag, ".cdt.canvas");
  TK_do_all_events();

  /* read & eval tcl script conic_detect.tcl */
  code = Tcl_EvalFile(CDT->TK->interp, "conic_detect.tcl");
  if (code != TCL_OK) exit_with_error("Can't read or eval conic_detect.tcl");
  TK_do_all_events();
}

/***********************************************************************/
/* Create conic in canvas with the 6 implicite eq parameters	       */
/* A*x^2 + B*xy + C*y^2 + D*x + E*y + F = 0			       */
/***********************************************************************/

#define A coeff[0]
#define B coeff[1]
#define C coeff[2]
#define D coeff[3]
#define E coeff[4]
#define F coeff[5]

void conic_detect_tk_create_conic_quick(struct_tk *TK, point_2d_int dim,
					float *coeff, char *tag, char *color)
{
   char str1[1096];
   char str2[1096];
   char str_aux[20];
   int y1,y2;
   float aux,aux1,aux2;
   int x_min,x_max;
   int inc;

   /* Calculate positions of vertical tangents */
   aux1 = sqrt(C*D*D-B*D*E+A*E*E+B*B*F-4*A*C*F);
   aux2 = B*B-4*A*C;
   x_min = (int)((2*C*D-B*E-2*sqrt(C)*aux1)/aux2);
   x_max = (int)((2*C*D-B*E+2*sqrt(C)*aux1)/aux2);
   sort_value(&x_min, &x_max);
   if (x_min < 0) x_min=0;
   if (x_max >= dim.x) x_max=dim.x-1;

   inc = 2 + (x_max - x_min)/20;
   strcpy(str1,".cdt.canvas create line");
   strcpy(str2,".cdt.canvas create line");
   for (int x=x_min; x<=x_max; x+=inc) {
     aux = (E + B*x)*(E + B*x) - 4*C*(F + x*(D + A*x));
     if (aux >= 0 && C!=0) {
       aux = sqrt(aux);
       y1 = (int)floor(-(E + B*x + aux)/2/C);
       y2 = (int)floor(-(E + B*x - aux)/2/C);
	 if (y1>=0 && y1<dim.y) {
	   sprintf(str_aux," %d %d", x, y1);
	   strcat(str1, str_aux);
	 }
	 if (y2>=0 && y2<dim.y) {
	   sprintf(str_aux," %d %d", x, y2);
	   strcat(str2, str_aux);
	 }
     }
   }

   sprintf(str1, "%s -fill %s -tag %s1", str1, color, tag);
   sprintf(str2, "%s -fill %s -tag %s2", str2, color, tag);
   Tcl_Eval(TK->interp, str1);
   Tcl_Eval(TK->interp, str2);
   TK_do_all_events();
}

void conic_detect_tk_create_conic(struct_tk *TK, point_2d_int dim,
				  float *coeff, char *tag, char *color)
{
   char str1[1096];
   char str2[1096];
   char str_aux[20];
   int x1,x2,y1,y2;
   float aux,aux1,aux2;
   int x_min,x_max,y_min,y_max;
   int inc;

   aux1 = sqrt(C*D*D-B*D*E+A*E*E+B*B*F-4*A*C*F);
   aux2 = B*B-4*A*C;
   /* Calculate positions of vertical tangents */
   x_min = (int)((2*C*D-B*E-2*sqrt(C)*aux1)/aux2);
   x_max = (int)((2*C*D-B*E+2*sqrt(C)*aux1)/aux2);
   sort_value(&x_min, &x_max);
   if (x_min < 0) x_min=0;
   if (x_max >= dim.x) x_max=dim.x-1;
   /* Calculate positions of horizontal tangents */
   y_min = (int)((-B*D+2*A*E+2*sqrt(A)*aux1)/aux2);
   y_max = (int)((-B*D+2*A*E-2*sqrt(A)*aux1)/aux2);
   sort_value(&y_min, &y_max);
   if (y_min < 0) y_min=0;
   if (y_max >= dim.y) y_max=dim.y-1;

   /* x scanning */
   inc = 2 + (x_max - x_min)/50;
   strcpy(str1,".cdt.canvas create line");
   strcpy(str2,".cdt.canvas create line");
   for (int x=x_min; x<=x_max; x+=inc) {
     aux = (E + B*x)*(E + B*x) - 4*C*(F + x*(D + A*x));
     if (aux >= 0 && C!=0) {
       aux = sqrt(aux);
       y1 = (int)floor(-(E + B*x + aux)/2/C);
       y2 = (int)floor(-(E + B*x - aux)/2/C);
	 if (y1>=0 && y1<dim.y) {
	   sprintf(str_aux," %d %d", x, y1);
	   strcat(str1, str_aux);
	 }
	 if (y2>=0 && y2<dim.y) {
	   sprintf(str_aux," %d %d", x, y2);
	   strcat(str2, str_aux);
	 }
     }
   }

   sprintf(str1, "%s -fill %s -tag %s1", str1, color, tag);
   sprintf(str2, "%s -fill %s -tag %s2", str2, color, tag);
   Tcl_Eval(TK->interp, str1);
   Tcl_Eval(TK->interp, str2);

   /* y scanning */
   inc = 2 + (y_max - y_min)/50;
   strcpy(str1,".cdt.canvas create line");
   strcpy(str2,".cdt.canvas create line");
   for (int y=y_min; y<=y_max; y+=inc) {
     aux = (D + B*y)*(D + B*y) - 4*A*(F + y*(E + C*y));
     if (aux >= 0 && C!=0) {
       aux = sqrt(aux);
       x1 = (int)floor(-(D + B*y + aux)/2/A);
       x2 = (int)floor(-(D + B*y - aux)/2/A);
	 if (x1>=0 && x1<dim.x) {
	   sprintf(str_aux," %d %d", x1, y);
	   strcat(str1, str_aux);
	 }
	 if (x2>=0 && x2<dim.x) {
	   sprintf(str_aux," %d %d", x2, y);
	   strcat(str2, str_aux);
	 }
     }
   }

   sprintf(str1, "%s -fill %s -tag %s1", str1, color, tag);
   sprintf(str2, "%s -fill %s -tag %s2", str2, color, tag);
   Tcl_Eval(TK->interp, str1);
   Tcl_Eval(TK->interp, str2);
   TK_do_all_events();
}

#undef A
#undef B
#undef C
#undef D
#undef E
#undef F

/***********************************************************************/
/* Create ellipse in canvas with the 2 foyers & a		       */
/***********************************************************************/
void conic_detect_tk_create_ellipse(struct_tk *TK, point_2d_int dim,
				    float *coeff, char *tag,
				    char *color, int mode)
#define X1 coeff[0]
#define Y1 coeff[1]
#define X2 coeff[2]
#define Y2 coeff[3]
#define AA coeff[4]
{
  float impl_coeff[6];
  float xo,yo,a2,b2,c2,inc;
  double SIN,COS,SIN2,COS2,SINCOS;

  xo = (X1+X2)/2;
  yo = (Y1+Y2)/2;
  c2 = ((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1))/4;
  a2 = AA*AA/4;
  b2 = a2-c2;
  COS = (X2-X1) / (2*sqrt(c2));
  SIN = -(Y1-Y2) / (2*sqrt(c2)); /* indirect base */
  SINCOS = SIN*COS;
  COS2 = COS*COS;
  SIN2 = SIN*SIN;

  if (b2>0) {
    impl_coeff[0] = b2*COS2+a2*SIN2;		/* x^2 */
    impl_coeff[1] = 2*SINCOS*(b2-a2);		/* xy  */
    impl_coeff[2] = a2*COS2+b2*SIN2;		/* y^2 */
    impl_coeff[3] = -2*(SINCOS*yo*(b2-a2) + b2*xo*COS2 + a2*xo*SIN2); /* x */
    impl_coeff[4] = -2*(SINCOS*xo*(b2-a2) + a2*yo*COS2 + b2*yo*SIN2); /* y */
    impl_coeff[5] = COS2*(b2*xo*xo + a2*yo*yo) + SIN2*(a2*xo*xo + b2*yo*yo) 
      + 2*xo*yo*SINCOS*(b2-a2) - a2*b2;		/* cst */

//     if ((inc=sqrt(4*c2)) > 30) inc /= 20;
//     else inc = 2; /* increment scaled with foyers distance */

    if (mode==QUICK_DRAW)
      conic_detect_tk_create_conic_quick(TK, dim, impl_coeff, tag, color);
    else
      conic_detect_tk_create_conic(TK, dim, impl_coeff, tag, color);
  }
}
#undef X1
#undef Y1
#undef X2
#undef Y2
#undef AA

/***********************************************************************/
/* Put starting point in cdetect canvas                                */
/***********************************************************************/
void conic_detect_tk_put_start(struct_cdetect *CDT)
{
  conic_detect_tk_create_conic(CDT->TK, CDT->image.dim,
			       CDT->last_cycle_starting_point,
			       START_POINT_TAG, START_POINT_LINE_COLOR);
}

void conic2_detect_tk_put_start(struct_cdetect *CDT)
#define X CDT->last_cycle_starting_point[0]
#define Y CDT->last_cycle_starting_point[1]
#define R CDT->last_cycle_starting_point[2]
{
  char str[SIZE_TCL_COMMAND];
  sprintf(str,
	  ".cdt.canvas create oval %d %d %d %d -tag %s -outline %s",
	  (int)(X-R), (int)(Y-R), (int)(X+R), (int)(Y+R),
	  START_POINT_TAG, START_POINT_LINE_COLOR);
  
  Tcl_Eval(CDT->TK->interp, str);
  TK_do_all_events();
}
#undef X
#undef Y
#undef R

void conic3_detect_tk_put_start(struct_cdetect *CDT)
{
//  char str[SIZE_TCL_COMMAND];
//  printf("{%f %f %f %f %f}",
//	 CDT->last_cycle_starting_point[0],
//	 CDT->last_cycle_starting_point[1],
//	 CDT->last_cycle_starting_point[2],
//	 CDT->last_cycle_starting_point[3],
//	 CDT->last_cycle_starting_point[4]);
//int x;
//scanf("%d",&x);
  conic_detect_tk_create_ellipse(CDT->TK, CDT->image.dim,
				 CDT->last_cycle_starting_point,
				 START_POINT_TAG,
				 START_POINT_LINE_COLOR, NORMAL_DRAW);
}

/***********************************************************************/
/* Remove starting point in cdetect canvas                             */
/***********************************************************************/
void conic_detect_tk_remove_start(struct_cdetect *CDT)
{
  char str[SIZE_TCL_COMMAND];
  sprintf(str, ".cdt.canvas delete %s", START_POINT_TAG);
  Tcl_Eval(CDT->TK->interp, str);
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

void conic2_detect_tk_remove_start(struct_cdetect *CDT)
{
  conic_detect_tk_remove_start(CDT);
}

void conic3_detect_tk_remove_start(struct_cdetect *CDT)
{
  char str[SIZE_TCL_COMMAND];
  sprintf(str, ".cdt.canvas delete %s1", START_POINT_TAG);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str, ".cdt.canvas delete %s2", START_POINT_TAG);
  Tcl_Eval(CDT->TK->interp, str);
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

/***********************************************************************/
/* Update cdetect canvas with the conic corresponding to the last      */
/* found subsys							       */
/***********************************************************************/
void conic_detect_tk_update_canvas(struct_cdetect *CDT, struct_algo *ALGO,
				   struct_algo_param *P_ALGO)
{
  /* delete last conic */
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete conic1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete conic2");
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
  /* draw new one */
  conic_detect_tk_create_conic(CDT->TK, CDT->image.dim, 
			       CDT->solutions[ALGO->step-1],
			       CONIC_TAG, GOOD_ELLIPSE_COLOR);
}

void conic2_detect_tk_update_canvas(struct_cdetect *CDT, struct_algo *ALGO,
				    struct_algo_param *P_ALGO)
#define X (int)CDT->solutions[ALGO->step-1][0]
#define Y (int)CDT->solutions[ALGO->step-1][1]
#define R (int)CDT->solutions[ALGO->step-1][2]
  /* draw last found circle */
{
  char str[SIZE_TCL_COMMAND];
  static int X0 = 0;
  static int Y0 = 0;
  static int R0 = 0;

  float visible_circle;
  visible_circle = prop_circle_in_image(CDT, CDT->solutions[ALGO->step-1]);

  if (ALGO->last_subsystem_accepted) {
    /* subsystem is accepted */
    sprintf(str,
	    ".cdt.canvas create oval %d %d %d %d -outline snow -outline %s",
	    X-R,Y-R,X+R,Y+R,GOOD_CIRCLE_COLOR);
  }
  else {
    /* subsystem is rejected */
    if (P_ALGO->draw_bad_subsystem) {
      /* remove the last erroneous solution in canvas */
      sprintf(str,".cdt.canvas delete %s",LAST_ERR_CIRCLE_TAG);
      Tcl_Eval(CDT->TK->interp, str);
      /* draw the last erroneous solution in dark color */
      sprintf(str,
	      ".cdt.canvas create oval %d %d %d %d -outline %s",
	      X0-R0,Y0-R0,X0+R0,Y0+R0,ERR_CIRCLE_DARK_COLOR);
      Tcl_Eval(CDT->TK->interp, str);
      X0 = X;
      Y0 = Y;
      R0 = R;
      /* draw the current erroneous solution */
      sprintf(str,
	      ".cdt.canvas create oval %d %d %d %d -outline %s -tag %s",
	      X-R,Y-R,X+R,Y+R,ERR_CIRCLE_LIGHT_COLOR,
	      LAST_ERR_CIRCLE_TAG);
      /* monitoring */
//      printf(" Rejected (size=%4.1f%% of typical) [%d %d %d]\n",
//	     ALGO->last_subsys_size/(float)CDT->last_subsystem_typical_size*100,
//	     X,Y,R);
    }
  }
    Tcl_Eval(CDT->TK->interp, str);
    TK_do_all_events();
/* ********** */
//int i;
//scanf("%d",&i);

}
#undef X
#undef Y
#undef R

void conic3_detect_tk_update_canvas(struct_cdetect *CDT, 
				    struct_algo *ALGO, struct_algo_param *P_ALGO)
  /* draw last_subsystem_typical_size found ellipse */
{
  static int color_nb = 0;

  float *x = CDT->solutions[ALGO->step-1];
  float visible_ellipse = 1;
  if (ALGO->last_subsys_size >= P_ALGO->stop_cycles 
      && ALGO->last_subsys_size >= ALGO->last_subsys_size_threshold)
    conic_detect_tk_create_ellipse(CDT->TK, CDT->image.dim,
				   CDT->solutions[ALGO->step-1],
				   NO_TAG, GOOD_ELLIPSE_COLOR, NORMAL_DRAW);
  else
    if (P_ALGO->draw_bad_subsystem)
      conic_detect_tk_create_ellipse(CDT->TK, CDT->image.dim,
				     CDT->solutions[ALGO->step-1],
				     NO_TAG, ERR_ELLIPSE_COLOR, NORMAL_DRAW);
}

/***********************************************************************/
/* Update cdetect canvas with points fitted by the last found subsys   */
/* (command line option -mon_well_classified)			       */
/***********************************************************************/
void conic_detect_tk_flag_classified(struct_tk *TK, float *point,
				     int index_subsystem)
#define CDT_UNFLAG -1
{
  char str[SIZE_TCL_COMMAND];
  char *color;

  if (index_subsystem != CDT_UNFLAG)
    color = TK_get_subsystem_color(index_subsystem, 0);
  else
    /* the classified points were flagged but the subsystem is rejected */
    color = REJECTED_AFTER_POSTPROCESSING_COLOR;

  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill %s -outline %s",
	  (int)point[0]-2, (int)point[1]-2, (int)point[0]+2, (int)point[1]+2,
	  color, color);
  Tcl_Eval(TK->interp, str); 
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
}

void conic_detect_tk_flag_non_classified(struct_cdetect *CDT, struct_algo *ALGO)
{
  point_2d_int pt;
  for (int i=0; i<ALGO->nb_couples; i++) {
    pt.x = (int)CDT->SYST->A[ALGO->equations[i]][0];
    pt.y = (int)CDT->SYST->A[ALGO->equations[i]][1];
    TK_create_cross(CDT->TK, pt, NON_CLASSIFIED_POINTS_COLOR);
  }
}

void conic_detect_tk_flag_rejected_point(struct_cdetect *CDT, int index)
{
  point_2d_int pt;
  pt.x = (int)CDT->SYST->A[index][0];
  pt.y = (int)CDT->SYST->A[index][1];
  TK_create_cross(CDT->TK, pt, REJECTED_POINTS_COLOR);
}

/***********************************************************************/
/* Update cdetect canvas with the conic corresponding to the current   */
/* subsystem (real time graphical monitoring)			       */
/***********************************************************************/
void conic_detect_tk_update_mon_canvas(struct_cdetect *CDT, float *x)
{
  /* delete last conic */
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete conic1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete conic2");
  Tk_DoOneEvent(TK_ALL_EVENTS|TK_DONT_WAIT);
  /* draw new one */
  conic_detect_tk_create_conic_quick(CDT->TK, CDT->image.dim, x,
				     CONIC_TAG, GOOD_ELLIPSE_COLOR);
}

void conic2_detect_tk_update_mon_canvas(struct_cdetect *CDT, float *x,
					int i1, int i2, float current_temp)
#define X0 (int)(x[0])
#define Y0 (int)(x[1])
#define R  (int)(x[2])
#define X1 (int)(CDT->SYST->A[i1][0])
#define Y1 (int)(CDT->SYST->A[i1][1])
#define X2 (int)(CDT->SYST->A[i2][0])
#define Y2 (int)(CDT->SYST->A[i2][1])
{
  char str[SIZE_TCL_COMMAND];

  /* update temperature */
  sprintf(str,"set temp %6.2f",current_temp);
  Tcl_Eval(CDT->TK->interp, str);

  /* delete last */
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete circle");
  /* draw new one */
  sprintf(str,".cdt.canvas create oval %d %d %d %d -outline green -tags circle",
	  X0-R,Y0-R,X0+R,Y0+R);
  Tcl_Eval(CDT->TK->interp, str);

  if (CDT->tan_estimation) {
    /* delete last */
    Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete sq_0");
//    Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete tan");
    /* draw new one */
    sprintf(str,".cdt.canvas create oval %d %d %d %d -fill red -tags sq_0",
	    X1-5,Y1-5,X1+5,Y1+5);
    Tcl_Eval(CDT->TK->interp, str);
//    sprintf(str,".cdt.canvas create line %d %d %d %d -fill red -tags tan",
//	    (int)(X1-15*CDT->u.x),(int)(Y1-15*CDT->u.y),
//	    (int)(X1+15*CDT->u.x),(int)(Y1+15*CDT->u.y));
//    Tcl_Eval(CDT->TK->interp, str);
  }
  else {
    /* delete last */
    Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete sq_0");
    Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete sq_1");
    Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete tan");
    /* draw new one */
    sprintf(str,".cdt.canvas create oval %d %d %d %d -fill red -tags sq_0",
	    X1-5,Y1-5,X1+5,Y1+5);
    Tcl_Eval(CDT->TK->interp, str);
    sprintf(str,".cdt.canvas create oval %d %d %d %d -fill red -tags sq_0",
	    X2-5,Y2-5,X2+5,Y2+5);
    Tcl_Eval(CDT->TK->interp, str);
    sprintf(str,".cdt.canvas create line %d %d %d %d -fill red -tags tan",
	    (int)(X1),(int)(Y1),(int)(X2),(int)(Y2));
    Tcl_Eval(CDT->TK->interp, str);
  }
  TK_do_all_events();
}
#undef X0
#undef Y0
#undef R
#undef X1
#undef X1
#undef X2
#undef X2

void conic3_1_detect_tk_update_mon_canvas(struct_cdetect *CDT, float *x, int i1, 
					  point_2d_float u1, point_2d_float u2,
					  point_2d_int f1, point_2d_int f2,
					  float current_temp)
#define F1X (int)(x[0])
#define F1Y (int)(x[1])
#define F2X (int)(x[2])
#define F2Y (int)(x[3])
#define X1 (int)(CDT->SYST->A[i1][0])
#define Y1 (int)(CDT->SYST->A[i1][1])

//#define FULL_MONITORING

{
  char str[SIZE_TCL_COMMAND];

  /* update temperature */
  sprintf(str,"set temp %6.2f",current_temp);
  Tcl_Eval(CDT->TK->interp, str);

  /* delete last */
#ifdef FULL_MONITORING
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete tan");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete line1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete line2");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete old_f1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete old_f2");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete sq_0");
#endif
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete conic1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete conic2");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete foyer1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete foyer2");

  /* draw new one */
#ifdef FULL_MONITORING
  sprintf(str,
      ".cdt.canvas create line %d %d %d %d -fill green -tags line1 -arrow last",
	  (int)(X1-100*u1.x),(int)(Y1-100*u1.y),
	  (int)(X1+100*u1.x),(int)(Y1+100*u1.y));
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,
      ".cdt.canvas create line %d %d %d %d -fill pink -tags line2 -arrow last",
	  (int)(X1-100*u2.x),(int)(Y1-100*u2.y),
	  (int)(X1+100*u2.x),(int)(Y1+100*u2.y));
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill green -tags old_f1",
	  f1.x-2,f1.y-2,f1.x+2,f1.y+2);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill pink -tags old_f2",
	  f2.x-2,f2.y-2,f2.x+2,f2.y+2);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,
	  ".cdt.canvas create line %d %d %d %d -fill red -tags tan -arrow last",
	  (int)(X1-25*CDT->u.x),(int)(Y1-25*CDT->u.y),
	  (int)(X1+25*CDT->u.x),(int)(Y1+25*CDT->u.y));
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create oval %d %d %d %d -fill red -tags sq_0",
	  X1-5,Y1-5,X1+5,Y1+5);
  Tcl_Eval(CDT->TK->interp, str);
#endif
#ifndef FULL_MONITORING
//   sprintf(str,
// 	  ".cdt.canvas create line %d %d %d %d -fill red -tags tan",
// 	  (int)(X1-10*CDT->u.x),(int)(Y1-10*CDT->u.y),
// 	  (int)(X1+10*CDT->u.x),(int)(Y1+10*CDT->u.y));
//   Tcl_Eval(CDT->TK->interp, str);
#endif
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill green -tags foyer1",
	  F1X-3,F1Y-3,F1X+3,F1Y+3);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill pink -tags foyer2",
	  F2X-3,F2Y-3,F2X+3,F2Y+3);
  Tcl_Eval(CDT->TK->interp, str);
  TK_do_all_events();

  conic_detect_tk_create_ellipse(CDT->TK, CDT->image.dim, x,
				 CONIC_TAG, GOOD_ELLIPSE_COLOR, QUICK_DRAW);

#ifdef FULL_MONITORING
  while (!getc(stdin));
#endif
}

void conic3_2_detect_tk_update_mon_canvas(struct_cdetect *CDT, float *x, int i1,
					  point_2d_float f1_est,
					  point_2d_float f2_est,
					  point_2d_int f1,
					  point_2d_int f2)
{
  char str[SIZE_TCL_COMMAND];

#ifdef FULL_MONITORING
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete tan");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete line1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete line2");
#endif
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete old_f1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete old_f2");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete tan");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete sq_0");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete conic1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete conic2");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete foyer1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete foyer2");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete est1");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete est2");
  Tcl_Eval(CDT->TK->interp, ".cdt.canvas delete line_f1f2");

  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill green -tags est1",
	  (int)f1_est.x-5,(int)f1_est.y-5,(int)f1_est.x+5,(int)f1_est.y+5);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill pink -tags est2",
	  (int)f2_est.x-5,(int)f2_est.y-5,(int)f2_est.x+5,(int)f2_est.y+5);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill green -tags foyer1",
	  F1X-3,F1Y-3,F1X+3,F1Y+3);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill pink -tags foyer2",
	  F2X-3,F2Y-3,F2X+3,F2Y+3);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create line %d %d %d %d -fill red -tags line_f1f2",
	  F1X+3*(F2X-F1X),F1Y+3*(F2Y-F1Y),
	  F1X-3*(F2X-F1X),F1Y-3*(F2Y-F1Y));
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,
	  ".cdt.canvas create line %d %d %d %d -fill red -tags tan -arrow last",
	  (int)(X1-25*CDT->u.x),(int)(Y1-25*CDT->u.y),
	  (int)(X1+25*CDT->u.x),(int)(Y1+25*CDT->u.y));
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create oval %d %d %d %d -fill red -tags sq_0",
	  X1-5,Y1-5,X1+5,Y1+5);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill green -tags old_f1",
	  f1.x-2,f1.y-2,f1.x+2,f1.y+2);
  Tcl_Eval(CDT->TK->interp, str);
  sprintf(str,".cdt.canvas create rectangle %d %d %d %d -fill pink -tags old_f2",
	  f2.x-2,f2.y-2,f2.x+2,f2.y+2);
  Tcl_Eval(CDT->TK->interp, str);
  TK_do_all_events();

  conic_detect_tk_create_ellipse(CDT->TK, CDT->image.dim, x,
				 CONIC_TAG, GOOD_ELLIPSE_COLOR, QUICK_DRAW);
//   float xx[5];
// xx[0]=f1_est.x;
// xx[1]=f1_est.y;
// xx[2]=f2_est.x;
// xx[3]=f2_est.y;
// xx[4]=x[4];
//   conic_detect_tk_create_ellipse(CDT->TK, CDT->dim, xx,
// 				 CONIC_TAG, ESTIMATED_ELLIPSE_COLOR, QUICK_DRAW);

#ifdef FULL_MONITORING
  while (!getc(stdin));
#endif
}

#undef F1X
#undef F1Y
#undef F2X
#undef F2Y
#undef X1
#undef Y1

#undef START_POINT_TAG
#undef CONIC_TAG
#undef NO_TAG

#undef SIZE_TCL_COMMAND
#undef QUICK_DRAW
#undef NORMAL_DRAW
