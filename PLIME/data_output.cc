#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "struct.h"
#include "math_functions.h"
#include "utils.h"
#include "bmp_8b.h"
#include "memory_alloc.h"

#include "inline_functions_AR.cc"

#define AR_MAPPING_MATLAB

/***********************************************************************/
/* display last subsystem info                                         */
/***********************************************************************/
void display_last_subsyst_size (struct_algo *ALGO)
{
  printf("\nNb:%-3d  Size:%4d / %-4d Cycles:%4d / %-4d Eq.left:%d\n",
	 ALGO->num_extr_subsys,
	 ALGO->last_subsys_size, ALGO->last_subsys_size_threshold,
	 ALGO->last_subsystem_nb_cycles, ALGO->nb_cycles, ALGO->nb_couples);
}

void display_last_subsystem_info_cdt (struct_algo *ALGO, struct_cdetect *CDT)
{
  if (ALGO->last_subsystem_accepted)
    /* accepted */
    printf("%-3d* ",ALGO->num_extr_subsys);
  else /* rejected */
    printf("%-3d  ",ALGO->num_extr_subsys);    
	  
    /* Nb   cycle     size       %typ   Enhanc left */
    printf("%-4d %-4d %-4d %-4d %-5.1f%% %-4.1f%% %-4d",
	   ALGO->last_subsystem_nb_cycles,
	   ALGO->last_subsys_size, ALGO->last_subsys_size_data,
	   ALGO->last_subsys_size_threshold,
	   ALGO->last_subsys_size/(float)ALGO->last_subsystem_typical_size*100,
	   ALGO->last_subsystem_enhanced_size/(float)ALGO->last_subsys_size*100,
	   ALGO->nb_couples);

    /* display current solution */
    if (CDT->conic_type == CIRCLE)
      printf(" [%3d %3d %3d]\n",
	     (int)CDT->solutions[ALGO->step-1][0],
	     (int)CDT->solutions[ALGO->step-1][1],
	     (int)CDT->solutions[ALGO->step-1][2]);
    else
      printf(" [%3d %3d %3d %3d %3d]\n",
	     (int)CDT->solutions[ALGO->step-1][0],
	     (int)CDT->solutions[ALGO->step-1][1],
	     (int)CDT->solutions[ALGO->step-1][2],
	     (int)CDT->solutions[ALGO->step-1][3],
	     (int)CDT->solutions[ALGO->step-1][4]);
}

/***********************************************************************/
/* Display time informations                                           */
/***********************************************************************/
void display_time_informations(struct_algo_param *P_ALGO) {
  printf("Total time  : %4.2fs\n",P_ALGO->BENCH->time);
  printf("  Relaxation    : %4.2fs\n",P_ALGO->BENCH->computation_time);
  printf("  Postprocessing: %4.2fs\n",P_ALGO->BENCH->postprocessing_time);
}

/***********************************************************************/
/* creation of spreadsheet files of the equation system		       */
/***********************************************************************/
void create_eq_syst_files(struct_eq_system *SYST, struct_algo_param *P_ALGO)
{
  if (P_ALGO->syst_out_file_name[0] != '\0') {
    FILE *a_file, *b_file;
    char aux[100];

    sprintf(aux,"%s_a.txt",P_ALGO->syst_out_file_name);
    a_file = fopen_w(aux);
    sprintf(aux,"%s_b.txt",P_ALGO->syst_out_file_name);
    b_file = fopen_w(aux);

    for (int i=0; i<SYST->nr; i++) {
      vector_to_file(a_file, SYST->A[i], SYST->nc, " ");
      fprintf(a_file,"\n");
      fprintf(b_file,"%f\n", SYST->b[i]);
    }

    fclose (a_file);
    fclose (b_file);

    printf("\nEq system files created (%s_a.txt & %s_b.txt)\n",
	   P_ALGO->syst_out_file_name ,P_ALGO->syst_out_file_name);
    exit(1);
  }
}

/***********************************************************************/
/* AR model		       					       */
/***********************************************************************/

/* print AR parameters on screen */
void print_AR_param(struct_algo_param *PAR, struct_algo *ALGO,struct_ARmodel *AR)
{
  printf("\n\n----------- AR modeling of time series ---------\n\n");

  printf ("AR model order             : %d",AR->order);
  if (PAR->winnow) printf(" (winnow)\n");
  else printf(" (perceptron)\n");
  if (AR->index.min)
    printf ("Input file for AR data     : %s (%d:%d)\n",
	    AR->inname,AR->index.min,AR->index.max);
  else
    printf ("Input file for AR data     : %s \n",AR->inname);
  printf ("Input time samples         : %d\n",AR->num_data);
  printf ("Maximum absolute error     : %-8.4f \n",PAR->epsilon);

  if (PAR->auto_temp_set){
    printf ("Automatic setting of initial temperature \n");
    printf ("Gain factor tzero  : %-10.6f \n",PAR->tzero);
    printf ("Gain update tzero  : %-10.6f \n",PAR->update_tz);
    printf ("Fraction old tzero : %-10.6f \n",PAR->alpha_tz);
  }
  else {
    printf ("Initial temperature        : %-8.3f \n",PAR->T);
  }
  printf ("Number of iterations       : %d \n",PAR->nb0_cycles);
  printf ("Prop. of eq. in inner loop : %-10.2f \n",PAR->partial);

  if (!PAR->exponential) printf("Temperature decrease       : linear \n");
  else printf("Temperature decrease       : exponential\n");

  printf("Distance with %d point(s)\n",2*AR->nb_pt_corr+1);

  if (!PAR->auto_temp_set) printf("\nGain factor %10.6f \n",PAR->alpha);

  if (PAR->no_corrections)
    printf("Correction mode : conditioned if ti > %f\n",
	   PAR->no_corrections);
  else printf("Correction mode : non-conditioned \n");

  printf("Stop if avg_d/ti > %d\n", PAR->stop_temp);

  printf ("\nInitial number of equations: %d\n",ALGO->nb_couples);
  printf ("------------------------------------------------\n\n");
}

/* print AR subsyst results on screen */
void print_AR_subsyst_results(struct_ARmodel *AR,
			      struct_algo *ALGO,
			      struct_eq_system *SYST)
{
  printf(" Subsystem #%d ***\n",ALGO->step);
  printf(" Size     : %d \n",ALGO->last_subsys_size);
//  printf(" Solution : ");
//  vector_to_screen(AR->solutions[ALGO->step-1],SYST->nc);
  printf("\n Average abs error               : %g\n",AR->av_abs_err);
  printf(" Average abs quadratic error     : %g\n",AR->av_quadr_err);
  printf(" Remaining number of equations   : %d\n\n", ALGO->nb_couples);
}

/* print AR error values on screen */
void print_AR_error(struct_ARmodel *AR)
{
  printf ("----------------------------------------------------------------\n");
  printf ("Average abs error for all subsystems           : %g\n",AR->total_av_abs_err);
  printf ("Average abs quadratic error for all subsystems : %g\n",AR->total_av_quadr_err);
  printf ("----------------------------------------------------------------\n");
}

/* print AR final results */
void print_AR_final_results(int nb_subsyst, struct_ARmodel *AR)
{
  for(int i=0; i<nb_subsyst; i++) {
    printf("#%-3d %-4d ", i+1, AR->sizes[i]);
    vector_to_screen (AR->solutions[i], AR->SYST->nc);
  }
}

/* Write files for visualisation of AR subsystem partitioning */
void write_matlab_AR_file(struct_algo_param *P_ALGO,
			  struct_algo *ALGO,
			  struct_ARmodel *AR)
#define A AR->SYST->A
#define b AR->SYST->b
#define S AR->solutions
{
  int classified, ambiguity, i, j, k;
  int min_AR_dist;
  float err, min_AR_error, lambd1, lambd2;

  /* open ouput files */
  char mapping_name[100];
  char solutions_name[100];
  strcpy(mapping_name,AR->out_name);
  strcat(mapping_name,"_map");
  strcpy(solutions_name,AR->out_name);
  strcat(solutions_name,"_sol");

//  FILE *out_file = fopen_w(AR->out_name);
  FILE *mapping_file = fopen_w(mapping_name);
  FILE *solutions_file = fopen_w(solutions_name);

  printf("Mapping file created (%s)\n",mapping_name);
  printf("Solutions file created (%s)\n",solutions_name);

  /* write solutions file */
  for(i=0; i<ALGO->num_extr_subsys; i++) {
    vector_to_file(solutions_file, AR->solutions[i], AR->SYST->nc," ");
    fprintf(solutions_file,"\n");
  }

  /* write output file */
//   for (i = 0; i < (AR->num_data-AR->order) ; i++) {
//     classified = 0;
//     ambiguity = 0;
//     min_AR_error = FLT_MAX;

//     for (k=0; k < ALGO->num_extr_subsys; k++) {
//       err=ABS(qprod_scal(S[k], A[i], AR->SYST->nc) - b[i]);
//       if (err < P_ALGO->epsilon)
// 	if (!classified) {  /* eq not yet classified */
// 	  fprintf(out_file," %4d ",i);

// 	  for (j = 0; j<AR->order; j++)
// 	    fprintf(out_file," %12.5f ",A[i][j]);
// 	  fprintf(out_file," %12.5f ",b[i]);
// 	  for (j = 0; j < AR->order ; j++)
// 	    fprintf(out_file," %12.5f ",S[k][j]);

// 	  fprintf(out_file," %3d ",k+1);
// 	  classified=1;
// 	}
// 	else ambiguity = 1; /* eq already classified */

//       /* update min_AR_error */
//       if ((err < min_AR_error)){
// 	min_AR_error = err;
// 	min_AR_dist = k+1;
//       }
//     }

//     if (ambiguity)                  /* classified with ambiguity */
//       fprintf(out_file," 0 %3d ",min_AR_dist);
//     else
//       if (classified)               /* classified without ambiguity */
// 	fprintf(out_file," 1 %3d ",min_AR_dist);

//     if (AR->order == 2 && classified){
//       lambd1=ABS(0.5*S[min_AR_dist-1][1]+((float)sqrt((double)
//         S[min_AR_dist-1][1]*S[min_AR_dist-1][1]+4.0*S[min_AR_dist-1][0])));
//       lambd2=ABS(0.5*S[min_AR_dist-1][1]-((float)sqrt((double)
//         S[min_AR_dist-1][1]*S[min_AR_dist-1][1]+4.0*S[min_AR_dist-1][0])));

//       if ((lambd1 >= 1.0) || (lambd2 >= 1.0))
// 	fprintf(out_file," 1 ");
//       else fprintf(out_file," 0 ");
//     }
//     if (classified) fprintf(out_file,"\n");
//   }

#ifndef AR_MAPPING_MATLAB
  /* write AR mapping file (ascii)*/
  if (AR->contiguous) { /* contiguous option enable */
    for (i = 0; i < (AR->num_data - AR->order); i++) {
      fprintf(mapping_file," %5d ",i);
      for (k = 0; k < ALGO->num_extr_subsys  ; k++) {
	if   ((ABS(qprod_scal(S[k], A[i], AR->SYST->nc) - b[i])
	       < P_ALGO->epsilon))
	  if (i >= AR->partition_begin[k] && i <= AR->partition_end[k])
	    fprintf(mapping_file, " # ");
	  else fprintf(mapping_file," + ");
	else
	  fprintf(mapping_file," - ");
      }
      fprintf(mapping_file,"\n");
    }
  }
  else { /* contiguous option disable */
    for (i = 0; i < (AR->num_data - AR->order); i++) {
      fprintf(mapping_file," %5d ",i);
      for (k = 0; k < ALGO->num_extr_subsys  ; k++) {
	if   ((ABS(AR_dist(AR, ALGO, S[k], i))
	       < P_ALGO->epsilon))
	  fprintf(mapping_file," # ");
	else
	  fprintf(mapping_file," - ");
      }
      fprintf(mapping_file,"\n");
    }
  }
#else
  /* write AR mapping file (matlab) */
  int *part;
  part = (int*)malloc((ALGO->num_extr_subsys+1) * sizeof(char));

  for (i = 0; i < (AR->num_data - AR->order); i++) {
    part[0] = AR->partitions[i];
    for (k = 0; k < ALGO->num_extr_subsys; k++) {
#ifndef AR_RELATIVE_EPSILON
      if ((ABS(AR_dist(AR, ALGO, S[k], i))
	   < P_ALGO->epsilon) && part[0]!=k+1)
//	   < P_ALGO->epsilon) && AR->partitions[i]!=k+1)
#else
      if ((ABS(AR_dist(AR, ALGO, S[k], i))
	   < AR_RELATIVE_EPSILON*ABS(b[EQ[i]])) && part[0]!=k+1)
//	   < AR_RELATIVE_EPSILON*ABS(b[EQ[i]])) && AR->partitions[i]!=k+1)
#endif
	part[k+1]=k+1;
      else part[k+1] = 0;
    }
    vector_to_file(mapping_file, part, ALGO->num_extr_subsys+1, " ");
    fprintf(mapping_file,"\n");
  }

//  free(part);
#endif

  /* Close all AR related files */
//  fclose(out_file);
  fclose(mapping_file);
  fclose(solutions_file);
}
#undef A
#undef b
#undef S

/***********************************************************************/
/* Optical flow		      				       	       */
/***********************************************************************/

/* print optical parameters on screen & result_file */
void print_optical_param(struct_algo_param *PAR, struct_algo *ALGO,struct_optical *OPT)
{
  printf ("Input file for x movement : %s\n",OPT->mov_x_name);
  printf ("Input file for y movement : %s\n",OPT->mov_y_name);
  printf ("Images size               : %d %d\n",OPT->dim_x,OPT->dim_y);
  printf ("Maximum absolute error : %-10.6f \n",PAR->epsilon);

  if (PAR->auto_temp_set){
    printf ("Automatic setting of initial temperature \n");
    printf ("Gain factor tzero  : %-10.6f \n",PAR->tzero);
    printf ("Gain update tzero  : %-10.6f \n",PAR->update_tz);
    printf ("Fraction old tzero : %-10.6f \n",PAR->alpha_tz);
  }
  else {
    printf ("Initial temperature: %-8.3f \n",PAR->T);
  }
  printf ("Number of iterations   : %d \n",PAR->nb0_cycles);
  printf ("Maximum number of subsystems: %d \n",PAR->max_num_subsys);

  if (!PAR->exponential){
    printf("Temperature decrease   : linear \n");
  }
  else {
    printf("Temperature decrease   : exponential \n");
  }

  if (!PAR->auto_temp_set){
    printf("Gain factor %10.6f \n",PAR->alpha);
  }

  if (PAR->no_corrections){
    printf("Correction mode: conditioned \n");
  }
  else {
    printf("Correction mode: non-conditioned \n");
  }

  printf("\n------------------------------------------------\n");
  printf ("Initial number of equations: %d\n",ALGO->nb_couples);
  printf ("------------------------------------------------\n\n");

}

/***********************************************************************/
/* Line detection	       					       */
/***********************************************************************/

void calculate_ldetect_bench_mark(struct_ldetect *LDT, struct_algo_param *P_ALGO,
				  struct_algo *ALGO)
{
  int index, i;
  int *sizes;
  sizes = (int *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(int));
  for (i=0; i<=P_ALGO->max_num_subsys; i++) sizes[i] = LDT->sizes[i];

  int nb_subsys = P_ALGO->BENCH->nb_subsys;

  P_ALGO->BENCH->result = 0;
  if (nb_subsys > ALGO->num_extr_subsys)
    nb_subsys = ALGO->num_extr_subsys;

  for (i=0; i < nb_subsys; i++) {
    index = index_max_value(sizes, ALGO->num_extr_subsys);
    P_ALGO->BENCH->result += sizes[index];
    sizes[index] = 0;
  }
  P_ALGO->BENCH->result *= 100 / (float)LDT->tot_nb_pts_no_noise;

  free(sizes);
}

void calculate_lms_param(struct_ldetect *LDT)
{
  int i;
  float m_x=0;
  float m_y=0;

  LDT->sum_xi=0;
  LDT->a_lms=0;

  for (i=0; i<LDT->tot_nb_pts; i++) {
    m_x+=LDT->SYST->A[i][0];
    m_y+=LDT->SYST->A[i][1];
  }
  m_x/=LDT->tot_nb_pts;
  m_y/=LDT->tot_nb_pts;

  for (i=0; i<LDT->tot_nb_pts; i++)
    LDT->sum_xi+=(LDT->SYST->A[i][0]-m_x)*(LDT->SYST->A[i][0]-m_x);

  for (i=0; i<LDT->tot_nb_pts; i++)
    LDT->a_lms+=(LDT->SYST->A[i][0]-m_x)*(LDT->SYST->A[i][1]-m_y);

  LDT->a_lms/=LDT->sum_xi;
}

void print_ldetect_param(struct_algo_param *PAR, struct_algo *ALGO,
			 struct_ldetect *LDT)
{
  if (LDT->tan_estimation)
    printf ("** LINE DETECTION (with local tangent estimation) **\n");
  else printf ("** LINE DETECTION (with AMS relaxation) **\n");
  if (PAR->syst_in_file) {
    printf ("Input file   : %s_a.txt (eq_syst file)\n",PAR->syst_in_file_name);
    printf ("Images size  : -\n");
    printf ("Add. noise   : -\n");
  }
  else {
    printf ("Input file   : %s\n",LDT->image_file_name);
    printf ("Images size  : %d %d\n",LDT->image.dim.x,LDT->image.dim.y);
    printf ("Add. noise   : %4.1f%%\n",LDT->noise);
  }
  printf ("Epsilon      : %-10.6f \n",PAR->epsilon);
  if (LDT->tan_estimation)
    printf("Local LMS tangente estimation (sq of %d pixels)\n",
	   LDT->tan_estimation*2+1);

  if (PAR->auto_temp_set){
    printf ("Automatic setting of initial temperature \n");
    printf (" Gain factor tzero  : %-10.6f \n",PAR->tzero);
    printf (" Gain update tzero  : %-10.6f \n",PAR->update_tz);
    printf (" Fraction old tzero : %-10.6f \n",PAR->alpha_tz);
  }
  else {
    printf ("Initial temperature: %-8.3f \n",PAR->T);
  }
  printf ("Number of iterations        : %d \n",PAR->nb0_cycles);
  printf ("Maximum number of subsystems: %d \n",PAR->max_num_subsys);
  printf ("Minimum size of subsystems  : %d \n",PAR->stop_cycles);
  printf ("Prop. of eq. in inner loop  : %f \n",PAR->partial);

  if (!PAR->exponential)
    printf("Temperature decrease        : linear \n");
  else printf("Temperature decrease        : exponential \n");

  if (!PAR->auto_temp_set)
    printf("Gain factor %10.6f \n",PAR->alpha);

  if (PAR->no_corrections)
    printf("Correction mode: conditioned (no corr. if avg_d/ti < %f)\n",
	   PAR->no_corrections);
  else printf("Correction mode: non-conditioned \n");

  if (LDT->segment) {
    printf("Segmentation with sliding win : %d pixel(s)\n",LDT->segment);
    printf("                  threshold   : %d\n",LDT->seg_win_threshold);
    printf("                  epsilon     : %0.2f\n",LDT->seg_epsilon);
  }

  if (PAR->rnd == 0) printf("** NO RANDOM INITIALISATION **\n");

  printf("\n------------------------------------------------\n");
  printf ("Initial number of equations: %d\n",LDT->tot_nb_pts);
  printf ("------------------------------------------------\n\n");

}

void create_ldetect_output_bmp(struct_ldetect *LDT, struct_algo *ALGO,
		       struct_algo_param *P_ALGO)
{
  int i,j;
  float x1,y1,x2,y2;
  float *s;
  char file_name[100];
  unsigned char **im;

  /* allocate & init temporary image */
  im = (unsigned char **) matrix_alloc(LDT->image.dim.y, LDT->image.dim.x,
				       sizeof(unsigned char));
  for(i=0; i<LDT->image.dim.y; i++)
    for(j=0; j<LDT->image.dim.x; j++) im[i][j]=0;
  /* draw clusters' edges */
  if (LDT->cluster) {
    int cl = LDT->cluster;
    int x,y;
    for (i=1; i<cl; i++) {
      x = i*LDT->image.dim.x/cl;
      y = i*LDT->image.dim.y/cl;
      put_line_in_image(im, LDT->image.dim, x, 0, x, 1,100);
      put_line_in_image(im, LDT->image.dim, 0, y, 1, y,100);
    }
  }

  /* draw all lines finded by the algorithm */
  if (LDT->segment) { /* segment detection */
    for(i=0; i < ALGO->num_extr_subsys; i++)
      put_segment_in_image(im, LDT->image.dim, LDT->segments[i][0],
			LDT->segments[i][1], LDT->segments[i][2],
			LDT->segments[i][3], 200 + LDT->corr_cl[i]);
  }
  else { /* full line detection */
    for(i=0; i < ALGO->num_extr_subsys; i++)
      if (LDT->sizes[i] >= P_ALGO->stop_cycles) {
	s = LDT->solutions[i];
	if (ABS(s[1]/s[0]) < 1) {
	  x1 = 0;
	  x2 = 25;
	  y1 = -s[2]/s[1]; /* y1=-c/b */
	  y2 = (-s[2]-s[0]*25)/s[1]; /* y2=(-c-ax)/b */
	}
	else {
	  x1 = -s[2]/s[0]; /* x1=-c/a */
	  x2 = (-s[2]-s[1]*25)/s[0];/* (-c-by)/a */
	  y1 = 0;
	  y2 = 25;
	}
	x1 += (float)LDT->image.dim.x/2;
	y1 += (float)LDT->image.dim.y/2;
	x2 += (float)LDT->image.dim.x/2;
	y2 += (float)LDT->image.dim.y/2;
	put_line_in_image(im, LDT->image.dim, x1, y1, x2, y2,
			  200 + LDT->corr_cl[i]);
      }
  }

  /* mix source & result bmp files */
  for (i=0; i<LDT->image.dim.y; i++)
    for (j=0; j<LDT->image.dim.x; j++) {
      if (LDT->image.im[i][j] != LDT->image.background_color
	  && im[i][j] > 0) im[i][j]=2;
      if (LDT->image.im[i][j] != LDT->image.background_color
	  && im[i][j] == 0) im[i][j]=1;
    }

  sprintf(file_name,"%s_out.bmp",LDT->image_file_name);
  save_bmp_8b(file_name, im, LDT->image.dim);
  printf("Result bmp file written (%s)\n\n",file_name);
  matrix_free((void **)im,LDT->image.dim.y);
}

/***********************************************************************/
/* conic							       */
/***********************************************************************/
void print_cdetect_param(struct_algo_param *PAR, struct_algo *ALGO,
			 struct_cdetect *CDT)
{
  if (CDT->conic_type==GENERAL) printf ("\n** CONIC DETECTION **\n");
  if (CDT->conic_type==CIRCLE) printf ("\n** CIRCLE DETECTION **\n");
  if (CDT->conic_type==ELLIPSE) printf ("\n** ELLIPSE DETECTION **\n");
  printf ("Input file   : %s\n",CDT->image_file_name);
  printf ("Back color   : %d\n",CDT->image.background_color);
  printf ("Images size  : %d %d\n",CDT->image.dim.x,CDT->image.dim.y);
  printf ("Add. noise   : %4.1f%%\n",CDT->noise);
  if (PAR->relative_eps) {
    printf("Abs. epsilon : %-7.3f\n",PAR->epsilon);
    printf("Rel. epsilon : %-7.3f%%\n",PAR->relative_eps*100);
  }
  else
    printf ("Epsilon      : %-7.3f\n",PAR->epsilon);

  if (PAR->auto_temp_set){
    printf ("Automatic setting of initial temperature \n");
    printf (" Gain factor tzero  : %-10.6f \n",PAR->tzero);
    printf (" Gain update tzero  : %-10.6f \n",PAR->update_tz);
    printf (" Fraction old tzero : %-10.6f \n",PAR->alpha_tz);
  }
  else {
    printf ("Initial temperature : %-8.3f \n",PAR->T);
    printf("Gain factor          : %-8.5f \n",PAR->alpha);
  }
  printf ("Number of iterations        : %d \n",PAR->nb0_cycles);
  printf ("Maximum number of subsystems: %d \n",PAR->max_num_subsys);
  printf ("Abs min size of subsystems  : %d \n",PAR->stop_cycles);
  if (PAR->stop_cycles_prop)
    printf ("Min size (%% of typ.)        : %5.1f%%\n",PAR->stop_cycles_prop);
  printf ("Prop. of eq. in inner loop  : %f \n",PAR->partial);

  if (!PAR->exponential)
    printf("Temperature decrease        : linear \n");
  else printf("Temperature decrease        : exponential \n");

  if (PAR->no_corrections)
    printf("Correction mode: conditioned (no corr. if ti > %f)\n",
	   PAR->no_corrections);
  else printf("Correction mode: non-conditioned \n");

  if (CDT->rnd_origin)
    printf("Random origin  : enable \n");
  else printf("Random origin  : disable \n");

  if (CDT->postprocess_mode)
    printf("Postprocessing : enhanced [%d]\n",CDT->postprocess_mode);
  else printf("Postprocessing : normal\n");

  if (CDT->tan_estimation) {
    printf("Local LMS tangente estimation (sq of %d pixels)\n",
	   CDT->tan_estimation*2+1);
    if (CDT->remove_bad_pts)
      printf("Corr. coeff.   : enable\n");
    else printf("Corr. coeff.   : disable\n");
  }

  printf("\n------------------------------------------------\n");
  printf ("Initial number of equations: %d\n",CDT->tot_nb_pts);
  printf ("------------------------------------------------\n\n");

//        1  *  20  193 /194 /130  221.8%    0 1757 [191  35  13]
  printf("     Nb of cycle [%d]\n",PAR->nb0_cycles);
  printf("     |    Nb of pts classified among eq left\n");
  printf("     |    |    Nb of pts classified among all eq\n");
  printf("     |    |    |    Typical nb of pts (2*pi*R)\n");
  printf("     |    |    |    |    %% classified v.r.t. typical [%0.1f%%]\n",
	 PAR->stop_cycles_prop);
  printf("     |    |    |    |    |      %% classified with enhanced post.\n");
  printf("     |    |    |    |    |      |     Nb of eq. left [%d]\n",
	 CDT->tot_nb_pts);
  printf("     |    |    |    |    |      |     |\n");
}

void create_cdetect_output_bmp(struct_cdetect *CDT, struct_algo *ALGO,
		       struct_algo_param *P_ALGO)
{
  int i,j;
  char file_name[100];
  unsigned char **im;

  /* allocate & init temporary image */
  im = (unsigned char **) matrix_alloc(CDT->image.dim.y, CDT->image.dim.x,
				       sizeof(unsigned char));
  for(i=0; i<CDT->image.dim.y; i++)
    for(j=0; j<CDT->image.dim.x; j++) im[i][j]=0;
  /* draw clusters' edges */
  if (CDT->cluster) {
    int cl = CDT->cluster;
    int x,y;
    for (i=1; i<cl; i++) {
      x = i*CDT->image.dim.x/cl;
      y = i*CDT->image.dim.y/cl;
      put_line_in_image(im, CDT->image.dim, x, 0, x, 1,100);
      put_line_in_image(im, CDT->image.dim, 0, y, 1, y,100);
    }
  }

  /* draw all conics finded by the algorithm */
  for(i=0; i < ALGO->num_extr_subsys; i++)
//    if (CDT->sizes[i] >= P_ALGO->stop_cycles)
      put_conic_in_image(im, CDT->image.dim, CDT->solutions[i], 200 + CDT->corr_cl[i]);

  /* mix source & result bmp files */
  for (i=0; i<CDT->image.dim.y; i++)
    for (j=0; j<CDT->image.dim.x; j++) {
      if (CDT->image.im[i][j]!=CDT->image.background_color && im[i][j] >0)
	im[i][j]=2;
      if (CDT->image.im[i][j]!=CDT->image.background_color && im[i][j]==0)
	im[i][j]=1;
    }

  sprintf(file_name,"%s_out.bmp",CDT->image_file_name);
  save_bmp_8b(file_name, im, CDT->image.dim);
  printf("Result bmp file written (%s)\n\n",file_name);
  matrix_free((void **)im,CDT->image.dim.y);
}

void create_c23detect_output_bmp(struct_cdetect *CDT, struct_algo *ALGO,
				struct_algo_param *P_ALGO)
{
  int i,j;
  char file_name[100];
  unsigned char **im;

  /* allocate & init temporary image */
  im = (unsigned char **) matrix_alloc(CDT->image.dim.y, CDT->image.dim.x,
				       sizeof(unsigned char));
  for (i=0; i<CDT->image.dim.y; i++)
    for(j=0; j<CDT->image.dim.x; j++) im[i][j]=0;
  if (CDT->conic_type==CIRCLE) {
    /* draw all circles finded by the algorithm */
    for(i=0; i < ALGO->num_extr_subsys; i++) {
      if (CDT->sizes[i] >= P_ALGO->stop_cycles) {
	put_circle_in_image(im, CDT->image.dim, CDT->solutions[i][0],
			    CDT->solutions[i][1], CDT->solutions[i][2], 255);
      }
    }
  }
  else { /*draw all ellipses finded by the algorithm */
    for(i=0; i < ALGO->num_extr_subsys; i++) {
      if (CDT->sizes[i] >= P_ALGO->stop_cycles) {
	put_ellipse_in_image(im, CDT->image.dim, CDT->solutions[i], 255);
      }
    }
  }

   /* mix source & result bmp files */
   for (i=0; i<CDT->image.dim.y; i++)
     for (j=0; j<CDT->image.dim.x; j++) {
       if (CDT->image.im[i][j]!=CDT->image.background_color && im[i][j] >0)
	 im[i][j]=2;
       if (CDT->image.im[i][j]!=CDT->image.background_color && im[i][j]==0)
	 im[i][j]=1;
     }

  sprintf(file_name,"%s_out.bmp",CDT->image_file_name);
  save_bmp_8b(file_name, im, CDT->image.dim);
  printf("Result bmp file written (%s)\n\n",file_name);
  matrix_free((void **)im,CDT->image.dim.y);
}

void calculate_cdetect_bench_mark(struct_cdetect *CDT,
				  struct_algo_param *P_ALGO,
				  struct_algo *ALGO)
{
  int index, i;
  int *sizes;
  sizes = (int *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(int));
  for (i=0; i<=P_ALGO->max_num_subsys; i++) sizes[i] = CDT->sizes[i];

  int nb_subsys = P_ALGO->BENCH->nb_subsys;
  FILE *file;

  P_ALGO->BENCH->result = 0;
  if (nb_subsys > ALGO->num_extr_subsys)
    nb_subsys = ALGO->num_extr_subsys;

  for (i=0; i < nb_subsys; i++) {
    index = index_max_value(sizes, ALGO->num_extr_subsys);
    P_ALGO->BENCH->result += sizes[index];
    sizes[index] = 0;
  }
  P_ALGO->BENCH->result *= 100 / (float)CDT->tot_nb_pts_no_noise;

  free(sizes);
}


