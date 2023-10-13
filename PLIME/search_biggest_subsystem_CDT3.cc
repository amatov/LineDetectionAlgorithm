/****************************************************************************/
/**                                                                        **/
/** This function implements the thermal relaxation algorithm              **/
/** which is applied to equation systems. The goal is to detect the        **/
/** dominant subsystem having one particular solution.                     **/
/**                                                                        **/
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "leon.h"
#include "math_functions.h"
#include "struct.h"
#include "memory_alloc.h"
#include "data_output.h"
#include "update_equations.h"
#include "utils.h"

#include "conic_detect_tk.h"

#include "inline_functions.cc"

#define EQ ALGO->equations

/* temp */
#include "utils.h"

/****************************************************************************/
/* Conic_3 detection (ellipse)  					    */
/****************************************************************************/

/****************************************************************************/
/* Initialisation of solution for ellipse detection	       		    */
/****************************************************************************/
void solution_initialisation_CDT3(struct_cdetect *CDT, struct_algo *ALGO,
				  float *x)
{
  /* initialisation of solutions */
  if (ALGO->retry_flag != 0)
//     if (CDT->nb_retry[ALGO->step]==1)
//       printf("Retry #%3d", CDT->nb_retry[ALGO->step]);
//     else
//       printf("\b\b\b%3d", CDT->nb_retry[ALGO->step]);
//   fflush(stdout);
  
  x[0] = RAND * CDT->image.dim.x;
  x[1] = RAND * CDT->image.dim.y;
  if (CDT->initial_r == 0) {
    /* random ellipse */
      if (RAND<0.5) {
	x[2] = x[0] + (RAND+0.1) * CDT->image.dim.x/3;
	x[3] = x[1] + (RAND+0.1) * CDT->image.dim.y/3;
      }
      else {
	x[2] = x[0] - (RAND+0.1) * CDT->image.dim.x/3;
	x[3] = x[1] - (RAND+0.1) * CDT->image.dim.y/3;
      }
      /* excentricity randomly taken in [0,1[ */
      x[4] = sqrt((x[2]-x[0])*(x[2]-x[0]) + (x[3]-x[1])*(x[3]-x[1])) / RAND;

    }
  else {
    /* random circle with radius CDT.initial_r */
    x[2] = x[0]+1;
    x[3] = x[1]+1;
    x[4] = CDT->initial_r;
  }
  vector_copy(CDT->last_cycle_starting_point, x, CDT->SYST->nc);
}


/****************************************************************************/
/* Circle correction scheme for one equation                                */
/****************************************************************************/
void apply_circle_correction_to_current_eq (struct_cdetect *CDT,
					    struct_algo_param *P_ALGO,
					    struct_algo *ALGO, float *x,
					    int *nb_corr, int *nb_good,
					    float *tot_corr, float *tot_dist)
#define X CDT->SYST->A[i1][0]
#define Y CDT->SYST->A[i1][1]
{ 
  x[4] /= 2;
  /* mean sq tan estimation */
  int i1 = EQ[ALGO->equation_index]; /* indirect access to equations */
  float dist_AB, geo_dist, delta_r, correction_coeff;
  if (find_tangente_of_edge(CDT->image, CDT->SYST->A[i1],
			    CDT->tan_estimation, CDT->remove_bad_pts,
			    &(CDT->u))
      > CORRELATION_THRESHOLD) {
    delta_r =  sqrt((x[0]-X)*(x[0]-X)
		    +(x[1]-Y)*(x[1]-Y))-x[4];
    /* no correction if delta_r too big */
    if (ABS(ALPHA_DIST_CIRC*delta_r/ALGO->ti) < P_ALGO->stop_temp) {
      dist_AB = sqrt(CDT->u.x*CDT->u.x + CDT->u.y*CDT->u.y);
      CDT->u.x /= dist_AB; /* |u|=1 */
      CDT->u.y /= dist_AB;
      /* calculate distance */
      geo_dist = CDT->u.x*(x[0]-X) + CDT->u.y*(x[1]-Y);
      /* calculate correction coeff */
      correction_coeff = -(ABS(geo_dist)+ALPHA_DIST_CIRC*ABS(delta_r))/ALGO->ti;
      if (correction_coeff > -MAX_EXP_NEG)
	correction_coeff = EXP_TAB(correction_coeff);
      else correction_coeff = 0;
//            correction_coeff = EXP_TAB(-(ABS(geo_dist)
//                                         +ALPHA_DIST_CIRC*ABS(delta_r))/ti);
      /* apply correction to current solutions */
      x[0] -= CDT->u.x*geo_dist*(ALGO->ti/ALGO->t0) * correction_coeff;
      x[1] -= CDT->u.y*geo_dist*(ALGO->ti/ALGO->t0) * correction_coeff;
      x[4] += delta_r * (ALGO->ti/ALGO->t0) * correction_coeff;
      
      /* update canvas if -graph enable */
      if (P_ALGO->graph && ALGO->iteration%MONITORING_STEP==0
	  && P_ALGO->mon==-1) {
	x[2] = x[4];
	conic2_detect_tk_update_mon_canvas(CDT, x, i1, i1, ALGO->ti);
      }
      
      nb_corr++;
    }
  }
  else {
    /* the selected point is isolate and tan. can't be estimated or the */
    /* correlation coefficient is |r|<CORRELATION_THRESHOLD & the       */
    /* option -remove_bad_pts is enable. In both cases the pt is remove */
    /* from eq[]                                                        */
    
    /* flag rejected pt in canvas */
    if (P_ALGO->graph && P_ALGO->mon_well_classified)
      conic_detect_tk_flag_rejected_point(CDT,i1);
    /* remove point in eq list */
    EQ[ALGO->equation_index] = EQ[--(ALGO->nb_couples)];
    ALGO->iteration--;
  }
  
  x[4] *= 2;
  x[2] = x[0]+x[4]/10;
  x[3] = x[1];
  
}
#undef X
#undef y

/****************************************************************************/
/* Ellipse correction scheme for one equation                               */
/****************************************************************************/
#define X CDT->SYST->A[i1][0]
#define Y CDT->SYST->A[i1][1]
#define XF1 x[0]
#define YF1 x[1]
#define XF2 x[2]
#define YF2 x[3]

inline void apply_correction_to_current_eq(struct_cdetect *CDT,
					   struct_algo_param *P_ALGO,
					   struct_algo *ALGO, float *x,
					   int *nb_corr, int *nb_good,
					   float *tot_corr, float *tot_dist) {
  float geo_dist; /* geometric distance between current point and solution */
  int i1 = EQ[ALGO->equation_index]; /* indirect access to equations */

  point_2d_int f1,f2;
  if (P_ALGO->graph) { /* store old solution for graphical monitoring */
    f1.x = (int)x[0];
    f1.y = (int)x[1];
    f2.x = (int)x[2];
    f2.y = (int)x[3];
  }

  /* lms tan estimation */
  if (find_tangente_of_edge(CDT->image, CDT->SYST->A[i1],
			    CDT->tan_estimation, CDT->remove_bad_pts,
			    &(CDT->u))
      > CORRELATION_THRESHOLD) {

    float c_tan;
    float d1_tan,d2_tan;

    float norm = sqrt(CDT->u.x*CDT->u.x + CDT->u.y*CDT->u.y);
    CDT->u.x /= norm; /* norm(u)=1 */
    CDT->u.y /= norm;

    /* d1_tan & d2_tan : distances between tan at current pt and foyers */
    c_tan = -CDT->u.y*X + CDT->u.x*Y; /* tan eq: u.y*X-u.x*Y+c_tan = 0  */
    d1_tan = CDT->u.y*XF1 - CDT->u.x*YF1 + c_tan;
    d2_tan = CDT->u.y*XF2 - CDT->u.x*YF2 + c_tan;

    /* abs_d1 & abs_d2 : distances between current point and foyers */
    point_2d_float pf1,pf2;	/* PF1 & PF2 vectors		      */
    float abs_d1,abs_d2;		/* ||PF1|| & ||PF2||		      */
    float delta_r;		/* diff between current r & radius    */
    /* corresponding to current point     */
    pf1.x = X-XF1;
    pf1.y = Y-YF1;
    pf2.x = X-XF2;
    pf2.y = Y-YF2;
    abs_d1 = sqrt(pf1.x*pf1.x + pf1.y*pf1.y);
    abs_d2 = sqrt(pf2.x*pf2.x + pf2.y*pf2.y);
    delta_r = (abs_d1+abs_d2)-x[4];

    //	  if (iteration%2 || (ABS(delta_r)/x[4] > 0.2)) {
    /* first kind of correction in witch the distance between the two */
    /* current foyers is supposed to be correct			      */

    float c1,c2;		/* cst for line's equations	      */
    point_2d_float u1,u2;	/* direction vectors of line1 & line2 */
    float delta1,delta2;       	/* distances to 1st & 2nd line	      */
    float delta;		/* delta=min(delta1,delta2)	      */
    float cos_alpha,sin_alpha;	/* cos & sin of the error angle	      */
    float e2;			/* sq excentricity of current ellipse */
    float r;

	  /* no correction if delta_r too big or (d1_tan * d2_tan) < 0 */
    if (ABS(ALPHA_DIST_ELLIPSE*delta_r/ALGO->ti) < P_ALGO->stop_temp &&
	d1_tan*d2_tan>0) {
      /* no correction if conditioned correction mode is enable */
      if (!P_ALGO->no_corrections || ABS(delta_r)
	  > P_ALGO->epsilon) {
	/* bisect angle calculation */
	float aux = (pf1.x*pf2.x + pf1.y*pf2.y)/(abs_d1*abs_d2);
	/* aux=-cos(alpha1+alpha2) */
	if (aux>1) aux=1;
	else if (aux<-1) aux=-1;
	cos_alpha = sqrt(0.5*(1-aux));
	sin_alpha = sqrt(0.5*(1+aux));

		/* lines' eq calculation */
		/* |u1|=|u2|=1           */
			/*u1=R(a)(u) */
	u1.x = cos_alpha*CDT->u.x + sin_alpha*CDT->u.y;
	u1.y = -sin_alpha*CDT->u.x + cos_alpha*CDT->u.y;
	/* u2=R(pi-a)(u)*/
	u2.x = -cos_alpha*CDT->u.x + sin_alpha*CDT->u.y;
	u2.y = -sin_alpha*CDT->u.x - cos_alpha*CDT->u.y;
	c1 = -u1.y*X + u1.x*Y; /* 1st line: u1.y*X - u1.x*Y + c1 = 0 */
	c2 = -u2.y*X + u2.x*Y; /* 2nd line: u2.y*X - u2.x*Y + c2 = 0 */
	if (!(d1_tan>0 && d2_tan>0)) {
	  /* 1st case: F1 proj. on 1st line & F2 proj. on 2nd line */
	  delta1 = u1.y*XF1 - u1.x*YF1 + c1;
	  delta2 = u2.y*XF2 - u2.x*YF2 + c2;
	  delta = min_abs_value(delta1, delta2);
	  geo_dist = ((ABS(delta1)+ABS(delta2))/2
		      +ALPHA_DIST_ELLIPSE*ABS(delta_r));
	  if ((aux = geo_dist/ALGO->ti)<=P_ALGO->stop_temp) {
	    /* no correction if r<exp(-stop_temp) */
	    r = (ALGO->ti/ALGO->t0) * EXP_TAB(-aux);
	      
	    x[0] += -u1.y * delta * r;
	    x[1] +=  u1.x * delta * r;
	    x[2] += -u2.y * delta * r;
	    x[3] +=  u2.x * delta * r;
	    x[4] += delta_r * r;
	  }
	}
	else {
	  /* 2nd case: F1 proj. on 2nd line & F2 proj. on 1st line */
	  delta1 = u2.y*XF1 - u2.x*YF1 + c2;
	  delta2 = u1.y*XF2 - u1.x*YF2 + c1;
	  delta = min_abs_value(delta1, delta2);

	  geo_dist = ((ABS(delta1)+ABS(delta2))/2
		      +ALPHA_DIST_ELLIPSE*ABS(delta_r));
	  if ((aux = geo_dist/ALGO->ti)<=P_ALGO->stop_temp) {
	    /* no correction if r<exp(-stop_temp/ti) */
	    r = (ALGO->ti/ALGO->t0) * EXP_TAB(-aux);
	      
	    x[0] += -u2.y * delta * r;
	    x[1] +=  u2.x * delta * r;
	    x[2] += -u1.y * delta * r;
	    x[3] +=  u1.x * delta * r;
	    x[4] += delta_r * r;
	  }
	}
	/* additionnal correction for high excentricity ellipses      */

		/* excentricity of current ellipse */
	e2 = ((XF2-XF1)*(XF2-XF1)+(YF2-YF1)*(YF2-YF1))/x[4];
	//		if (e2>8/9) { /* a>3b */
	if (0) {
	  point_2d_float f1f2;		/* F1F2 vector		      */
	  point_2d_float u_f1f2;	/* direction vector of F1F2   */
	  float norm_f1f2;		/* |F1F2|		      */

	  /* u_f1f2 direction vector of (f1f2) */
	  f1f2.x = XF2 - XF1;
	  f1f2.y = YF2 - YF1;
	  norm_f1f2 = sqrt(f1f2.x*f1f2.x + f1f2.y*f1f2.y);
	  u_f1f2.x = f1f2.x / norm_f1f2; /* norm(u_f1f2)=1 */
	  u_f1f2.y = f1f2.y / norm_f1f2;
	  if (ABS(u_f1f2.x*CDT->u.x+u_f1f2.y*CDT->u.y)<0.707) {
	    pf1.x = X-XF1;
	    pf1.y = Y-YF1;
	    pf2.x = X-XF2;
	    pf2.y = Y-YF2;
	    abs_d1 = sqrt(pf1.x*pf1.x + pf1.y*pf1.y);
	    abs_d2 = sqrt(pf2.x*pf2.x + pf2.y*pf2.y);
	    delta_r = (abs_d1+abs_d2)-x[4];

	    if (delta_r>=0) r=-0.01*(ALGO->ti/ALGO->t0)
			      *EXP_TAB(-ABS(delta_r)/ALGO->ti);
	    //		    else r=0.25*(ti/t0)*EXP_TAB(-ABS(delta_r)/ti);
	    else r=0;
	    x[0] += r * f1f2.x;
	    x[1] += r * f1f2.y;
	    x[2] += -r * f1f2.x;
	    x[3] += -r * f1f2.y;
	  }
	}

	/* update avg_dist */
	(*nb_corr)++;
	if (ABS(delta_r) < P_ALGO->epsilon) (*nb_good)++;
	*tot_dist += geo_dist;
	*tot_corr += r * sqrt(delta*delta * (u2.y*u2.y + u2.x*u2.x +
					    u1.y*u1.y + u1.x*u1.x) +
			                    delta_r*delta_r);

	/* update canvas & stdout if -graph enable */
	if (P_ALGO->graph)
	  if (P_ALGO->mon==-1 && ALGO->iteration%MONITORING_STEP==0) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	    printf("%5d ti=%5.1f",ALGO->index_cycle,ALGO->ti);
	    fflush(stdout);
	    conic3_1_detect_tk_update_mon_canvas(CDT,x,i1,u1,u2,f1,f2,ALGO->ti);
	  }
      }
    }
    else {
      // 	      point_2d_float f1f2; /* F1F2 vector */

      // 	      if ((aux = ABS(delta_r)/ti)<=P_ALGO->stop_temp) {
      // //	      if (0) {
      // 		f1f2.x = XF2 - XF1;
      // 		f1f2.y = YF2 - YF1;
      // 		r = 0.25 * (ti/t0) * EXP_TAB(-aux);
      // 		x[0] += r * f1f2.x;
      // 		x[1] += r * f1f2.y;
      // 		x[2] += -r * f1f2.x;
      // 		x[3] += -r * f1f2.y;
      //	      }
    }
  }
  else {
    /* the selected point is isolate and tan. can't be estimated or the */
    /* correlation coefficient is |r|<CORRELATION_THRESHOLD & the	    */
    /* option -remove_bad_pts is enable. In both cases the pt is remove */
    /* from eq[]							    */

	  /* flag rejected pt in canvas */
    if (P_ALGO->graph && P_ALGO->mon_well_classified)
      conic_detect_tk_flag_rejected_point(CDT,i1);
    /* remove point in eq list */
    EQ[ALGO->equation_index] = EQ[--(ALGO->nb_couples)];
    ALGO->iteration--;
  }
}

#undef X
#undef Y
#undef XF1
#undef YF1
#undef XF2
#undef YF2
#undef ALPHA_DIST

/****************************************************************************/
/* calculate subsystem size                                                 */
/****************************************************************************/
void calculate_subsystem_size_cdt3(struct_algo_param *P_ALGO, struct_algo *ALGO,
				   struct_cdetect *CDT, float *x) {
  float eps_threshold;
  float d1_x,d1_y,d2_x,d2_y;
  ALGO->last_subsys_size = 0;
  ALGO->last_subsys_size_data = 0;
  
  if (P_ALGO->relative_eps) /* relative threshold */
    eps_threshold = max_value(P_ALGO->relative_eps*x[4], P_ALGO->epsilon);
  else eps_threshold = P_ALGO->epsilon; /* absolute threshold */
  
  for (int i = 0; i < ALGO->nb_couples; i++) {
    int k=EQ[i];
    d1_x = CDT->SYST->A[k][0]-x[0];
    d1_y = CDT->SYST->A[k][1]-x[1];
    d2_x = CDT->SYST->A[k][0]-x[2];
    d2_y = CDT->SYST->A[k][1]-x[3];
    if (ABS(sqrt(d1_x*d1_x + d1_y*d1_y)
	    + sqrt(d2_x*d2_x + d2_y*d2_y) - x[4]) < eps_threshold)
      ALGO->last_subsys_size++;
  }

/*
  float c_x = (x[2]+x[0])/2;
  float c_y = (x[3]+x[1])/2;
  float alpha0 = 1/(SQR(x[2]-x[0])+SQR(x[3]-x[1]))*acos(x[2]-x[0]);
  if ((x[3]-x[1]) > 0) alpha0 = -alpha0;
  for (int R=(int)(x[4]-P_ALGO->epsilon); R<=(int)(x[4]+P_ALGO->epsilon); R++) {
    for (float i=0; i<=TWO_PI; i+=1/(float)R) {
  
	if (c>=0 && c<CDT->image.dim.x && l>=0 && l<CDT->image.dim.y)
	  if (CDT->image.im[l][c]!=CDT->image.background_color)
	    ALGO->last_subsys_size_data++;
*/

  for (int i = 0; i < ALGO->nb0_couples; i++) {
    d1_x = CDT->SYST->A[i][0]-x[0];
    d1_y = CDT->SYST->A[i][1]-x[1];
    d2_x = CDT->SYST->A[i][0]-x[2];
    d2_y = CDT->SYST->A[i][1]-x[3];
    if (ABS(sqrt(d1_x*d1_x + d1_y*d1_y)
	    + sqrt(d2_x*d2_x + d2_y*d2_y) - x[4]) < eps_threshold)
      ALGO->last_subsys_size_data++;
  }
}

/****************************************************************************/
/* Core for ellipse detection                                               */
/****************************************************************************/
void search_biggest_subsystem_CDT3(struct_cdetect *CDT,
				   struct_algo_param *P_ALGO,
				   struct_algo *ALGO){
    float avg;
    int non_choisies = 0;
    static float x[5]; /* Vectors of current subsystem solution */
    int *ligne;
    float visible_ellipse;
    float xx,yy;
    float aux;

    int nb_corr = 1;
    int nb_good = 0;
    float tot_dist = 1;
    float tot_corr = 1;

#ifdef AVG_MONITOR
FILE *avg_monitor;
char avg_monitor_filename[100];
sprintf(avg_monitor_filename,"avg_monitor_%d_%d.txt",ALGO->step,CDT->nb_retry[ALGO->step]);
avg_monitor=fopen_w(avg_monitor_filename);
#endif

    ALGO->last_subsystem_nb_cycles = ALGO->nb_cycles;

    /* the following variables are defined to simplify expressions */
    int nc = CDT->SYST->nc;
    /* ****** */

    /* solution monitoring in reciprocal space (-mon_ab option) */
    FILE *mon_ab;
    if (P_ALGO->mon_ab) {
      char mon_ab_name[100];
      sprintf(mon_ab_name,"MONITOR/mon_ab_%d",ALGO->step+1);
      mon_ab = fopen(mon_ab_name,"w");
    }

    ALGO->ti = P_ALGO->T;

    /* initialisation of solutions */
    solution_initialisation_CDT3(CDT, ALGO, x);
    if (P_ALGO->graph) conic3_detect_tk_put_start(CDT);

    ALGO->step++;

    /* In the beginning every index points to the equation itself */
    ligne= (int *) vector_alloc(ALGO->nb_couples, sizeof(int));
    for (int j = 0; j < ALGO->nb_couples; j++) ligne[j] = j;

    /* set initial temp if auto_temp_set is enable */
    if (P_ALGO->auto_temp_set) {
//      calculate_avg_circle(&avg, x, CDT->SYST, ALGO);
//      t0 = P_ALGO->tzero * avg;
//      printf("initial:%f\n",t0);
      exit_with_error
	("Auto setting of initial temp is no yet supported (c3detect)");
    }
    else ALGO->t0=P_ALGO->T;

    /* Here begins the outer loop, counting the number of cycles, i.e. the   */
    /* number of times, all equations of the equation system have been chosen*/
    /* to adapt the solution.                                                */
    ALGO->nb_cycles = P_ALGO->nb0_cycles;
    for (ALGO->index_cycle = 0;
	 ALGO->index_cycle < P_ALGO->nb0_cycles;
	 ALGO->index_cycle++) {

      /* calculate current temperature */
      aux = (float)ALGO->index_cycle / (float)ALGO->nb_cycles;
      ALGO->ti = P_ALGO->T * ((1-P_ALGO->T_param)*exp(-40*aux/(2-aux))
			+ P_ALGO->T_param*exp(-2.5*aux));

      /* temp monitoring */
      if (P_ALGO->mon_t) fprintf(P_ALGO->mon_t_file,"%g %g\n",avg,ALGO->ti);

      /* stop algorithm if avg/ti is upper than stop_temp */
       if (tot_dist/nb_corr/ALGO->ti > P_ALGO->stop_temp
 	  || (ABS(sqrt((x[0]-xx)*(x[0]-xx)+(x[1]-yy)*(x[1]-yy)))<0.5)) {
//      if (tot_dist/nb_corr/ti > P_ALGO->stop_temp
//	  || ((int)x[0]==(int)xx)&&((int)x[1]==(int)yy)) {
 	ALGO->last_subsystem_nb_cycles = ALGO->index_cycle;
 	ALGO->index_cycle = ALGO->nb_cycles;
       }

      /* */
      if (tot_dist/nb_corr/ALGO->ti > P_ALGO->no_corrections)
	P_ALGO->no_corrections=0;

#ifdef AVG_MONITOR
/* temp (avg_dist monitoring) */
fprintf(avg_monitor,"%f %f %d\n", tot_dist/nb_corr, tot_corr/nb_corr, nb_good);
#endif

      /* avg_dist initialisation */
      tot_dist = 0;
      tot_corr = 0;
      nb_corr = 1;
      nb_good = 0;

      /* */
      if (tot_dist/nb_corr/ALGO->ti < P_ALGO->no_corrections)
	P_ALGO->no_corrections=0;

      xx = x[0];
      yy = x[1];

/************************************************************************/
/* Here begins the inner loop, each couple of equations of the equation */
/* system is chosen in random order.                                    */
      for (ALGO->iteration = 0;
	   ALGO->iteration < (int)(ALGO->nb_couples*P_ALGO->partial);
	   ALGO->iteration++){
	/* choose one equation */
	ALGO->equation_index = choose_line(ligne, &non_choisies, ALGO->nb_couples);
	if (ALGO->ti > P_ALGO->T*0.8)
	  apply_circle_correction_to_current_eq(CDT, P_ALGO, ALGO, x,
						&nb_corr, &nb_good, &tot_corr,
						&tot_dist);
	else
	  apply_correction_to_current_eq(CDT, P_ALGO, ALGO, x,
					 &nb_corr, &nb_good, &tot_corr, &tot_dist);
      }
/* End of inner loop					                */
/************************************************************************/

      /* erase last line in stdout if monitoring enable */
      if (P_ALGO->graph && P_ALGO->mon==-1)
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");

      /* print partial results every 5 cycles */
      if (P_ALGO->mon > 1)
	if ((ALGO->index_cycle % (int)(ALGO->nb_cycles/5)) == 0) {
	  printf("ti %f ",ALGO->ti);
	  vector_to_screen(x,6);
	}
    }

    /* calculate subsystem size */
    calculate_subsystem_size_cdt3(P_ALGO, ALGO, CDT, x);

    /* add current solution to the matrix of solutions */
    for (int i=0; i < nc; i++)
      CDT->solutions[ALGO->step-1][i] = x[i];

    /* update the equation list eq[] */
    visible_ellipse = 1; /* partial ellipse length not yet implemented */
    ALGO->last_subsystem_typical_size = (int)(ellipse_length(x)*visible_ellipse);
    ALGO->last_subsys_size_threshold =
      (int)(P_ALGO->stop_cycles_prop/100*ALGO->last_subsystem_typical_size);

    if (ALGO->last_subsystem_accepted =
	(ALGO->last_subsys_size_data >= P_ALGO->stop_cycles
	 && ALGO->last_subsys_size_data >= ALGO->last_subsys_size_threshold
	 && ALGO->last_subsys_size > ALGO->last_subsys_size_data/10)) {
      /* last found subsystem is accepted (by first postprocessing) */
      c3detect_update_eq(P_ALGO, ALGO, CDT,
			 max_value(P_ALGO->relative_eps*x[4], P_ALGO->epsilon));
    }
    else ALGO->last_subsystem_enhanced_size = 0;
    
    /* remove starting solution in canvas */
    if (P_ALGO->graph) conic3_detect_tk_remove_start(CDT);

    free(ligne);

    /* close monitoring files */
    if (P_ALGO->mon_ab) fclose(mon_ab);

#ifdef AVG_MONITOR
    /* temp avg_monitor */
    fclose(avg_monitor);
#endif

}

#undef XA
#undef XB
#undef YA
#undef YB
#undef MIN_AB
#undef MAX_AB

#undef EQ
