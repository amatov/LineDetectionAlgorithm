/***********************************************************************/
/* This file includes the functions to update the equation list &      */
/* corresponding parameters considering the last found subsystem       */
/* (if accepted)						       */
/***********************************************************************/
#include <math.h>
#include <stdlib.h>
#include "leon.h"
#include "conic_detect_tk.h"
#include "line_detect_tk.h"
#include "math_functions.h"
#include "memory_alloc.h"

#include "inline_functions.cc"
#include "inline_functions_AR.cc"

#include <tk8.0.h>

/***********************************************************************/
/* AR modeling							       */
/***********************************************************************/
void AR_model_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
			struct_ARmodel *AR, float eps_threshold)
#define EQ ALGO->equations
#define A AR->SYST->A
#define b AR->SYST->b
{
  float aux,mean;
  AR->av_abs_err = 0;
  AR->av_quadr_err = 0;
  ALGO->last_subsys_size = 0;
  float *x1 = AR->solutions[ALGO->num_extr_subsys-1];

  if (AR->contiguous) { /* contiguous option enable */
    char *classified;
    classified = (char*) malloc(ALGO->nb_couples*sizeof(char));
    int index_begin, index_end;
    int last_begin = 0;
    int last_end = 0;

    /* compute classified vector */
    for (int i = 0; i < ALGO->nb_couples; i++)
#ifndef AR_RELATIVE_EPSILON
      classified[i] = (ABS(AR_dist(AR, ALGO, x1, i)) < P_ALGO->epsilon);
//      classified[i] = (ABS(AR_dist_sq(AR, ALGO, x1, i)) < 
//		       P_ALGO->epsilon * P_ALGO->epsilon);
#else
      classified[i] = (ABS(AR_dist(AR, ALGO, x1, i)) <
		       AR_RELATIVE_EPSILON*ABS(b[EQ[i]]));
#endif
    /* scan for the biggest contiguous solution system */
    for (int i = 0; i < ALGO->nb_couples-AR->contiguous; i++) {
      if (classified[i]) {
	index_begin = i++;
	do {
	  mean = 0;
	  for (int j = 0; j < AR->contiguous; j++) {
	    mean += classified[i+j];
//printf("%f\n",mean);
	  }
//printf("%1.2f ",mean/AR->contiguous);
	}
 	while (mean / AR->contiguous > 0.5
	       && i++ < ALGO->nb_couples-AR->contiguous);
	index_end = i;

	if (index_end-index_begin > last_end-last_begin) {
	  last_begin = index_begin;
	  last_end = index_end;
	}
      }
    }

    AR->partition_begin[ALGO->num_extr_subsys-1] = EQ[last_begin];
    AR->partition_end[ALGO->num_extr_subsys-1] = EQ[last_end];

    /* update equation system */
    ALGO->last_subsys_size = last_end - last_begin + 1;
    int k = 0;
    for (int i = 0; i<= ALGO->nb_couples; i++) {
      if (i >= last_begin
	  && i <= last_end) {
// 	/* calculate distance */
// 	float dist;
// 	if (AR->nb_pt_corr &&
// 	    i-AR->nb_pt_corr>=0 && i+AR->nb_pt_corr<ALGO->nb_couples) {
// 	  /* multiple points */
// 	  dist = 0;
// 	  for (int j=i-AR->nb_pt_corr; j<=i+AR->nb_pt_corr; j++)
// 	    dist += qprod_scal(A[j], x1, AR->SYST->nc) - b[j];
// 	  dist /= 2*AR->nb_pt_corr + 1;
// 	}
// 	else /* one point */
float dist;
 	  dist = AR_dist(AR, ALGO, x1,i);

// 	if (aux = ABS(dist) < P_ALGO->epsilon) {
aux = ABS(dist);
	  AR->av_abs_err += aux;
	  AR->av_quadr_err += aux * aux;
	  AR->partitions[EQ[i]] = ALGO->num_extr_subsys;
// 	}
// 	else EQ[k++] = EQ[i];
      }
      else EQ[k++] = EQ[i];
    }
  }
  else { /* contiguous option disable */
    int k = 0;
    ALGO->last_subsys_size = 0;
    for (int i = 0; i < ALGO->nb_couples; i++) {
      float aux = ABS(AR_dist(AR, ALGO, x1, i));
#ifndef AR_RELATIVE_EPSILON
      if (aux < P_ALGO->epsilon) {
#else
      if (aux < AR_RELATIVE_EPSILON*ABS(b[EQ[i]])) {
#endif
	AR->av_abs_err += aux;
	AR->av_quadr_err += aux * aux;
      	ALGO->last_subsys_size++;
	AR->partitions[EQ[i]] = ALGO->num_extr_subsys;
      }
      else EQ[k++] = EQ[i];    
    }

  }
  ALGO->nb_couples -= ALGO->last_subsys_size;
}
#undef EQ
#undef A
#undef b

/***********************************************************************/
/* line detection						       */
/***********************************************************************/
#define X_MIN LDT->cluster_edge[LDT->current_cluster][0]
#define X_MAX LDT->cluster_edge[LDT->current_cluster][1]
#define Y_MIN LDT->cluster_edge[LDT->current_cluster][2]
#define Y_MAX LDT->cluster_edge[LDT->current_cluster][3]

/* return the nb of point fitted by a segment of length 2*epsilon,     */
/* normal to u and with pt as center.				       */
/* (return -1 if the segment is out of image)			       */
inline int nb_pts_2epsilon(struct_ldetect *LDT, point_2d_int pt,
			   point_2d_float u, int epsilon)
{
  int nb_pts=0;
  int x=pt.x;
  int y=pt.y;
  int out=1;

  for (int i=-epsilon; i<=epsilon; i++) {
//    x = FL2INT(pt.x - i*u.y);
//    y = FL2INT(pt.y + i*u.x);
    if (u.x < u.y) x = pt.x +i;
    else y = pt.y+i;
    if (x>=X_MIN && x<X_MAX && y>=Y_MIN && y<Y_MAX) {
      out = 0;
      if (LDT->image.im[y][x]!=LDT->image.background_color) nb_pts++;
    }
  }
  if (out) nb_pts = -1;
  return nb_pts;
}

void line_detect_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
			   struct_ldetect *LDT, float eps_threshold)
{
  float pt_line_distance, pt_med_line_distance;
  float norm_ab;
  int new_nb_couples, k, j=0;
  float *x; /* current solution */
  float xx[3];
  point_2d_float current_pt;
  point_2d_int mid_segment;

  xx[0] = LDT->solutions[ALGO->step-1][0] / LDT->solutions[ALGO->step-1][2];
  xx[1] = LDT->solutions[ALGO->step-1][1] / LDT->solutions[ALGO->step-1][2];
  xx[2] = 1;

  x = (float*) &xx;

  norm_ab = norm_euclid(x[0], x[1]);
  new_nb_couples = ALGO->nb_couples;

  if (LDT->segment) { /* segment analysis */
    /* extract the biggest segment fitted by the last found subsystem	      */
    /* (LDT.segment length sliding window)				      */
    int *window = (int*)calloc(LDT->segment, sizeof(int));
    point_2d_float u; /* direction vector of last found line */
    float norm_u;
    point_2d_int start_pt, current_pt0;
    int i1 = 0;
    int i2 = 0;
    int tot_pts = 0;
    int flag = 0;
//    int threshold = (int)(eps_threshold*LDT->segment);
    int segment_start, segment_length=0;
    int i;

    float abs_x[2]; /* current solution in the absolute coord system */
    float aux = 1 + x[0]*(LDT->origin.x + 0.5 - LDT->image.dim.x/2) 
      + x[1]*(LDT->origin.y + 0.5 - LDT->image.dim.y/2);
    abs_x[0] = x[0] / aux;
    abs_x[1] = x[1] / aux;

    norm_u = sqrt(abs_x[0]*abs_x[0] + abs_x[1]*abs_x[1]);
    if (abs_x[1]<0) {
      u.x = -abs_x[1]/norm_u;
      u.y =  abs_x[0]/norm_u;
    }
    else {
      u.x =  abs_x[1]/norm_u;
      u.y = -abs_x[0]/norm_u;
    }

    /* start_pt computing */
    start_pt.y = (int)(-(1+abs_x[0]*X_MIN)/abs_x[1]);
    if (start_pt.y>=Y_MIN && start_pt.y<Y_MAX)
      /* left cluster edge */
      start_pt.x = X_MIN;
    else {
      start_pt.x = (int)(-(1+abs_x[1]*Y_MIN)/abs_x[0]);
      if (start_pt.x>=X_MIN && start_pt.x<=-(1+abs_x[1]*Y_MAX)/abs_x[0])
	/* upper cluster edge */
	start_pt.y = Y_MIN;
      else { 
	/* bottom cluster edge */
	start_pt.x = (int)(-(1+abs_x[1]*Y_MAX)/abs_x[0]);
	start_pt.y = Y_MAX;
      }
    }

    /* initialisation of sliding window array */
    for (i=0; i<=LDT->segment/2; i++) {
      int nb_pts;
      current_pt0.x = (int)(start_pt.x + i*u.x);
      current_pt0.y = (int)(start_pt.y + i*u.y);
      nb_pts = nb_pts_2epsilon(LDT, current_pt0, u, (int)LDT->seg_epsilon);
      window[i] = nb_pts;
      tot_pts += nb_pts;
    }
    if (tot_pts >= LDT->seg_win_threshold) flag = 1;

    current_pt.x++;
    current_pt.y++;
    int nb_pts = nb_pts_2epsilon(LDT, current_pt0, u, (int)LDT->seg_epsilon);

    /* begining of sliding window process */
    while (nb_pts>=0) {
      /* update segment */
      if (flag==0) {
	if (tot_pts >= LDT->seg_win_threshold) {
	  flag = 1;
	  i1=i;
	}
      }
      else {
/* ++++++++++ */
//	if (tot_pts < LDT->seg_win_threshold) {
	if (tot_pts < LDT->seg_win_threshold || nb_pts == 0) {
	  flag = 0;
	  i2 = i;
	  if (i2-i1 > segment_length) {
	    segment_length = i2-i1;
	    segment_start = i1;
	  }
	}
      }
      /* update sliding window */
      tot_pts -= window[i%LDT->segment];
      window[i%LDT->segment] = nb_pts;
      i++;
      tot_pts += nb_pts;
/* +++++++++ */
// printf("x:%d y:%d tot:%d nb_pt:%d i1:%d i2:%d length:%d\n"
//      ,current_pt0.x,current_pt0.y,tot_pts,nb_pts,i1,i2,segment_length);
      /* update nb_pts */
      current_pt0.x = (int)(start_pt.x + i*u.x);
      current_pt0.y = (int)(start_pt.y + i*u.y);
      nb_pts = nb_pts_2epsilon(LDT, current_pt0, u, (int)LDT->seg_epsilon);
    }
    /* end of sliding window process */

    if (flag==1) {
      i2 = i;
      if (i2-i1 > segment_length) {
	segment_length = i2-i1;
	segment_start = i1;
      }
    }

    /* add last found segment in segments matrix */
    segment_length = max_value(segment_length-LDT->segment/2,0);
    LDT->segments[ALGO->step-1][0] = (int)(start_pt.x + segment_start*u.x);
    LDT->segments[ALGO->step-1][1] = (int)(start_pt.y + segment_start*u.y);
    LDT->segments[ALGO->step-1][2] =
      (int)(start_pt.x +(segment_start+segment_length)*u.x);
    LDT->segments[ALGO->step-1][3] =
      (int)(start_pt.y + (segment_start+segment_length)*u.y);
    /* compute mid_segment coord for pt_med_line_distance calculation */
    mid_segment.x = (LDT->segments[ALGO->step-1][0] +
		     LDT->segments[ALGO->step-1][2])/2;
    mid_segment.y = (LDT->segments[ALGO->step-1][1] +
		     LDT->segments[ALGO->step-1][3])/2;
    mid_segment.x -= (int)((float)LDT->image.dim.x/2 - 0.5 - LDT->origin.x);
    mid_segment.y -= (int)((float)LDT->image.dim.y/2 - 0.5 - LDT->origin.y);
    aux = -(u.x*mid_segment.x + u.y*mid_segment.y);

    /* Compute the number of points fitted by the segment */
    int nb_point_fitted = 0;
    for (i = 0; i < ALGO->nb_couples; i++) {
      k=ALGO->equations[i];

      /* pt to current solution distance */
      pt_line_distance = qprod_scal(x, LDT->SYST->A[k], LDT->SYST->nc)/norm_ab;
      /* pt to median line of segment corresponding to the current solution */
      pt_med_line_distance = u.x*LDT->SYST->A[k][0]+u.y*LDT->SYST->A[k][1]+aux;

      if (ABS(pt_line_distance) <= eps_threshold
	  && ABS(pt_med_line_distance) <= segment_length/2+eps_threshold)
	nb_point_fitted++;
    }

    if (nb_point_fitted >= P_ALGO->stop_cycles) {
      /* update equations vector */
      for (i = 0; i < ALGO->nb_couples; i++) {
	k=ALGO->equations[i];

	/* pt to current solution */
	pt_line_distance = qprod_scal(x, LDT->SYST->A[k], LDT->SYST->nc)/norm_ab;
	/* pt to median line of segment corresponding to the current solution */
	pt_med_line_distance = u.x*LDT->SYST->A[k][0]+u.y*LDT->SYST->A[k][1]+aux;

	if (ABS(pt_line_distance) <= eps_threshold
	    && ABS(pt_med_line_distance) <= segment_length/2+eps_threshold) {
	  new_nb_couples--;
	  /* update canvas */
	  if (P_ALGO->graph && P_ALGO->mon_well_classified) {
	    current_pt.x = LDT->SYST->A[k][0] - LDT->origin.x
	      + LDT->image.dim.x/2;
	    current_pt.y = LDT->SYST->A[k][1] - LDT->origin.y
	      + LDT->image.dim.y/2;
	    line_detect_tk_flag_classified(LDT->TK,
					   current_pt, ALGO->step);
	  }
	  /* update sq error sum & residual mean */
	  LDT->sq_error[ALGO->step-1] += pt_line_distance * pt_line_distance;
	  LDT->mean[ALGO->step-1] += pt_line_distance;
	}
	else ALGO->equations[j++] = ALGO->equations[i];
      }
    }
/* ++++++++++ */
//    printf("nb_point_fitted:%d\n",nb_point_fitted);
  }

  else { /* no segment analysis */
    for (int i = 0; i < ALGO->nb_couples; i++) {
      k=ALGO->equations[i];
      pt_line_distance = qprod_scal(x, LDT->SYST->A[k], LDT->SYST->nc)/norm_ab;
      if (ABS(pt_line_distance) <= eps_threshold) {
	new_nb_couples--;
	/* update canvas */
	if (P_ALGO->graph && P_ALGO->mon_well_classified) {
	  current_pt.x = LDT->SYST->A[k][0] - LDT->origin.x
	    + LDT->image.dim.x/2;
	  current_pt.y = LDT->SYST->A[k][1] - LDT->origin.y
	    + LDT->image.dim.y/2;
	  line_detect_tk_flag_classified(LDT->TK,
					 current_pt, ALGO->step);
	}
	/* update sq error sum & residual mean */
	LDT->sq_error[ALGO->step-1] += pt_line_distance * pt_line_distance;
	LDT->mean[ALGO->step-1] += pt_line_distance;
      }
      else ALGO->equations[j++] = ALGO->equations[i];
    }
  }
  /* update the nb of couples left considering the current subsystem */
  ALGO->last_subsys_size = ALGO->nb_couples - new_nb_couples;
  ALGO->nb_couples = new_nb_couples;

//  free(x);
}
#undef X_MIN;
#undef X_MAX;
#undef Y_MIN;
#undef Y_MAX;

/***********************************************************************/
/* circle detection						       */
/***********************************************************************/
#define X CDT->SYST->A[k][0]
#define Y CDT->SYST->A[k][1]
#define XC x[0]
#define YC x[1]
#define R  x[2]

void c2detect_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
			struct_cdetect *CDT, float eps_threshold)
{
  int enhanced_size_estim,new_nb_couples,k;
  float *x;
  int j=0;

  new_nb_couples = ALGO->nb_couples;
  x = CDT->solutions[ALGO->step-1];

  if (CDT->postprocess_mode==0) { /* normal postprocessing */
    float d1_x,d1_y;
    for (int i = 0; i < ALGO->nb_couples; i++) {
      k=ALGO->equations[i];
      d1_x = X - XC;
      d1_y = Y - YC;
      if (ABS(sqrt(d1_x*d1_x + d1_y*d1_y) - R) < eps_threshold) {
	new_nb_couples--;
	/* update canvas */
	if (P_ALGO->graph && P_ALGO->mon_well_classified)
	  conic_detect_tk_flag_classified(CDT->TK,
					  CDT->SYST->A[k], ALGO->step);
      }
      else
	ALGO->equations[j++] = ALGO->equations[i];
    }
  }
  else /* enhanced postprocessing */
    {
      float delta_r,d1,cos,aux;
      float angular_dist,pf1_x,pf1_y;
      int *temp_flag;
      int nb_flag=0;

      int *temp_eq;
      temp_eq = (int*)malloc(ALGO->nb_couples*sizeof(int));
      /* array of flagged pts (in graph canvas) to erase rejected subsys */
      if (P_ALGO->graph && P_ALGO->mon_well_classified)
	temp_flag = (int*)malloc(ALGO->last_subsys_size*sizeof(int));

      for (int i = 0; i < ALGO->nb_couples; i++) {
	k=ALGO->equations[i];
	if (find_tangente_of_edge(CDT->image, CDT->SYST->A[k],
				  CDT->tan_estimation, CDT->remove_bad_pts,
				  &(CDT->u))) {
	
	  aux = sqrt(CDT->u.x*CDT->u.x + CDT->u.y*CDT->u.y);
	  CDT->u.x /= aux; /* norm(u)=1 */
	  CDT->u.y /= aux;

	  /* d1 : distances between current pt and center of current circle */
	  pf1_x = X-XC;
	  pf1_y = Y-YC;
	  d1 = sqrt(pf1_x*pf1_x + pf1_y*pf1_y);

	  delta_r = d1-R;
	  if (ABS(delta_r) < eps_threshold) {
	    /* angle calculation ((-u.y;u.x) is the direction vect. of normal) */
	    cos = ABS(-pf1_x*CDT->u.y + pf1_y*CDT->u.x)/d1;
	    if (cos > ANGULAR_THRESHOLD_CIRCLE
		|| cos < (1-ANGULAR_THRESHOLD_CIRCLE)) {
	      new_nb_couples--;
	      /* update canvas */
	      if (P_ALGO->graph && P_ALGO->mon_well_classified) {
		conic_detect_tk_flag_classified(CDT->TK, CDT->SYST->A[k],
						ALGO->step);
		temp_flag[nb_flag++] = k;
	      }
	    }
	    else /* the pt is close but not fitted by this ellipse */
	      temp_eq[j++] = ALGO->equations[i];
	  }
	  else /* delta_r > eps_threshold => pt rejected */
	    temp_eq[j++] = ALGO->equations[i];
	}
	else /* tan calculation impossible => pt rejected */
	  temp_eq[j++] = ALGO->equations[i];
      }

      enhanced_size_estim =  ALGO->nb_couples-new_nb_couples;
//printf("\nenh size: %d / %d\n",enhanced_size_estim,ALGO->last_subsys_size);
      if (enhanced_size_estim > ALGO->last_subsys_size*ENHANCED_THRESHOLD_CIRCLE) {
	/* subsystem accepted */
	free(ALGO->equations);
	ALGO->equations = temp_eq;
      }
      else { /* subsystem rejected */
	new_nb_couples = ALGO->nb_couples;
	free(temp_eq);
	ALGO->last_subsys_size = 0;
	/* erase subsystem in graphical display */
        if (P_ALGO->graph && P_ALGO->mon_well_classified)
	  for (int i=0; i<nb_flag; i++) {
	    k = temp_flag[i];
	    conic_detect_tk_flag_classified(CDT->TK, CDT->SYST->A[k],-1);
	  }
      }
    }

  /* update the nb of couples left considering the subsystem size */
  ALGO->nb_couples = new_nb_couples;
}

#undef X
#undef Y
#undef XC
#undef YC
#undef R

/***********************************************************************/
/* ellipse detection						       */
/***********************************************************************/
#define X CDT->SYST->A[k][0]
#define Y CDT->SYST->A[k][1]
#define XF1 x[0]
#define YF1 x[1]
#define XF2 x[2]
#define YF2 x[3]
#define R x[4]

void c3detect_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
			struct_cdetect *CDT, float eps_threshold)
{
  int enhanced_size_estim,new_nb_couples,k;
  float *x;
  int j=0;

  new_nb_couples = ALGO->nb_couples;
  x = CDT->solutions[ALGO->step-1];

  if (CDT->postprocess_mode==0) { /* normal postprocessing */
    float d1_x,d1_y,d2_x,d2_y;
    for (int i = 0; i < ALGO->nb_couples; i++) {
      k=ALGO->equations[i];
      d1_x = X - XF1;
      d1_y = Y - YF1;
      d2_x = X - XF2;
      d2_y = Y - YF2;
      if (ABS(sqrt(d1_x*d1_x + d1_y*d1_y)
	      + sqrt(d2_x*d2_x + d2_y*d2_y) - R) < eps_threshold) {
	new_nb_couples--;
	/* update canvas */
	if (P_ALGO->graph && P_ALGO->mon_well_classified)
	  conic_detect_tk_flag_classified(CDT->TK,
					  CDT->SYST->A[k], ALGO->step);
      }
      else
	ALGO->equations[j++] = ALGO->equations[i];
    }
  }
  else /* enhanced postprocessing */
    {
      float aux,delta_r,d1,d2,cos1,cos2,cos_alpha,sin_alpha;
      float angular_dist,pf1_x,pf1_y,pf2_x,pf2_y;
      point_2d_float u1,u2;
      int *temp_flag;
      int nb_flag=0;

      int *temp_eq;
      temp_eq = (int*)malloc(ALGO->nb_couples*sizeof(int));

      /* array of flagged pts (in graph canvas) to erase rejected subsys */
      if (P_ALGO->graph && P_ALGO->mon_well_classified)
	temp_flag = (int*)malloc(ALGO->last_subsys_size*sizeof(int));

      for (int i = 0; i < ALGO->nb_couples; i++) {
	k=ALGO->equations[i];
	if (find_tangente_of_edge(CDT->image, CDT->SYST->A[k],
				  CDT->tan_estimation, CDT->remove_bad_pts,
				  &(CDT->u))) {
	
	  aux = sqrt(CDT->u.x*CDT->u.x + CDT->u.y*CDT->u.y);
	  CDT->u.x /= aux; /* norm(u)=1 */
	  CDT->u.y /= aux;

	  /* d1 & d2 : distances between current pt and foyers */
	  pf1_x = X-XF1;
	  pf1_y = Y-YF1;
	  pf2_x = X-XF2;
	  pf2_y = Y-YF2;
	  d1 = sqrt(pf1_x*pf1_x + pf1_y*pf1_y);
	  d2 = sqrt(pf2_x*pf2_x + pf2_y*pf2_y);

	  delta_r = (d1+d2)-R;
	  if (ABS(delta_r) < eps_threshold) {
	    /* bisect angle calculation */
	    aux = (pf1_x*pf2_x + pf1_y*pf2_y)/(d1*d2);
	    /* aux=-cos(alpha1+alpha2) */
	    if (aux>1) aux=1;
	    else if (aux<-1) aux=-1;
	    cos_alpha = sqrt(0.5*(1-aux));
	    sin_alpha = sqrt(0.5*(1+aux));

	    /* lines' equations calculation */
	    u1.x =  cos_alpha*CDT->u.x + sin_alpha*CDT->u.y;/* 1st line */
	    u1.y = -sin_alpha*CDT->u.x + cos_alpha*CDT->u.y;
	    u2.x = -cos_alpha*CDT->u.x + sin_alpha*CDT->u.y;/* 2nd line */
	    u2.y = -sin_alpha*CDT->u.x - cos_alpha*CDT->u.y;

	    /* */
	    cos1 = ABS(u1.x*pf1_x + u1.y*pf1_y)/d1;
	    cos2 = ABS(u1.x*pf2_x + u1.y*pf2_y)/d2;

	    if (cos1 >= cos2)
	      /* 1st case: F1 with 1st line & F2 with 2nd line */
	      cos2 = (u2.x*pf2_x + u2.y*pf2_y)/d2;
	    else 
	      /* 2nd case: F1 with 2nd line & F2 with 1st line */
	      cos1 = (u2.x*pf1_x + u2.y*pf1_y)/d1;

	    angular_dist = ABS(cos1)+ABS(cos2);
	    if (angular_dist > ANGULAR_THRESHOLD_ELLIPSE
		|| angular_dist < (1-ANGULAR_THRESHOLD_ELLIPSE)) {
	      new_nb_couples--;
	      /* update canvas */
	      if (P_ALGO->graph && P_ALGO->mon_well_classified) {
		conic_detect_tk_flag_classified(CDT->TK, CDT->SYST->A[k],
						ALGO->step);
		temp_flag[nb_flag++] = k;
	      }
	    }
	    else /* the pt is close but not fitted by this ellipse */
	      temp_eq[j++] = ALGO->equations[i];
	  }
	  else /* delta_r > eps_threshold => pt rejected */
	    temp_eq[j++] = ALGO->equations[i];
	}
	else /* tan calculation impossible => pt rejected */
	  temp_eq[j++] = ALGO->equations[i];
      }

      enhanced_size_estim =  ALGO->nb_couples-new_nb_couples;
      if (enhanced_size_estim > ALGO->last_subsys_size*ENHANCED_THRESHOLD_ELLIPSE)
	{
	  /* subsystem accepted */
	  free(ALGO->equations);
	  ALGO->equations = temp_eq;
	}
      else { /* subsystem rejected */
	new_nb_couples = ALGO->nb_couples;
	free(temp_eq);
	ALGO->last_subsys_size = 0;
	/* erase subsystem in graphical display */
        if (P_ALGO->graph && P_ALGO->mon_well_classified)
	  for (int i=0; i<nb_flag; i++) {
	    k = temp_flag[i];
	    conic_detect_tk_flag_classified(CDT->TK, CDT->SYST->A[k],CDT_UNFLAG);
	  }
      }
    }

  /* update the nb of couples left considering the subsystem size */
  ALGO->nb_couples = new_nb_couples;
}

#undef X
#undef Y
#undef XF1
#undef YF1
#undef XF2
#undef YF2
