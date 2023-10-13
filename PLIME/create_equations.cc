/***********************************************************************/
/* This module contains functions that create the equation system      */
/* (matrix A and vector b).                                            */
/***********************************************************************/

#include <stdlib.h>
#include <math.h>
#include "memory_alloc.h"
#include "struct.h"
#include "utils.h"

/***********************************************************************/
/* Create AR model equation system				       */
/***********************************************************************/
void create_AR_equations(struct_ARmodel *AR)
{
  int i,j;

  AR->SYST = (struct_eq_system *)malloc(sizeof(struct_eq_system));
  AR->SYST->nc = AR->order;
  AR->SYST->nr = AR->num_data - AR->order;

  AR->SYST->A = (float **) matrix_alloc(AR->SYST->nr, AR->SYST->nc,
					sizeof(float));
  AR->SYST->b = (float *) vector_alloc(AR->SYST->nr, sizeof(float));
  AR->norm_eq = (float *) vector_alloc(AR->SYST->nr, sizeof(float)); 

  for (i = 0; i < (AR->SYST->nr); i++) {
    AR->norm_eq[i] = 0;
    for (j = 0; j < AR->SYST->nc; j++) {
      AR->SYST->A[i][j] = AR->time_series[i+j];
      AR->norm_eq[i] += AR->time_series[i+j]*AR->time_series[i+j];
    }
    AR->norm_eq[i] = sqrt(AR->norm_eq[i]);

    if (AR->inname2[0]=='\0')
      AR->SYST->b[i] = AR->time_series[i+AR->order];
    else 
      AR->SYST->b[i] = AR->time_series2[i+AR->order - 1];
  }
}

/***********************************************************************/
/* Create optical flow equation system				       */
/***********************************************************************/
void create_opt_equations(struct_optical *OPT)
{
  int x,y;

  OPT->SYST = (struct_eq_system *)malloc(sizeof(struct_eq_system));
  OPT->SYST->nc = 3;
  OPT->SYST->nr = OPT->dim_x * OPT->dim_y;

  /* allocate memory for the eq. system */
  OPT->SYST->A=(float **)matrix_alloc(OPT->SYST->nr, OPT->SYST->nc, sizeof(float));
  OPT->SYST->b=(float *) vector_alloc(OPT->SYST->nr, sizeof(float));

  for (x = 0; x < OPT->dim_x; x++)
    for (y = 0; y < OPT->dim_y; y++) {
      (OPT->SYST->A)[x*OPT->dim_y + y][0] = 1;
      (OPT->SYST->A)[x*OPT->dim_y + y][1] = (float)(x);
      (OPT->SYST->A)[x*OPT->dim_y + y][2] = (float)(y);
      (OPT->SYST->b)[x*OPT->dim_y + y] = OPT->mov_x[x][y];
    }
}

/***********************************************************************/
/* Create line detection equation system			       */
/***********************************************************************/
void create_ldetect_equations(struct_ldetect *LDT, char *sys_file_name)
{
  int x,y;
  FILE *eq_file;

  /* Calculate number of points */
  if(sys_file_name[0]=='\0') { /* no syst_in files */
    for (y = 0; y < LDT->image.dim.y; y++)
      for (x = 0; x < LDT->image.dim.x; x++)
	if (LDT->image.im[y][x]!=LDT->image.background_color) LDT->tot_nb_pts++;
  }
  else { /* read syst_in file */
    char aux[100];
    LDT->tot_nb_pts = 0;
    sprintf(aux, "%s_a.txt", sys_file_name);
    eq_file = fopen_r(aux);
    while (!feof(eq_file)) {
      read_float_from_file(eq_file);
      LDT->tot_nb_pts++;
    }
    LDT->tot_nb_pts /= 3;
    rewind(eq_file);
  }

  /* Add noise */
  LDT->tot_nb_pts_no_noise = LDT->tot_nb_pts;
  if (LDT->noise) {
    for (int j=0; j < LDT->tot_nb_pts*LDT->noise/100; j++)
      LDT->image.im[(int)floor(drand48()*LDT->image.dim.y)]
	[(int)floor(drand48()*LDT->image.dim.x)]=255;
    LDT->tot_nb_pts = 0;
    for (y = 0; y < LDT->image.dim.y; y++)
      for (x = 0; x < LDT->image.dim.x; x++)
	if (LDT->image.im[y][x]!=LDT->image.background_color) LDT->tot_nb_pts++;
  }

  if (LDT->cluster==0) { /* no clustering */
    LDT->cluster_edge = (int **)
      matrix_alloc(1, 4, sizeof(int));
    LDT->cluster_edge[0][0] = 0;
    LDT->cluster_edge[0][1] = LDT->image.dim.x;
    LDT->cluster_edge[0][2] = 0;
    LDT->cluster_edge[0][3] = LDT->image.dim.y;
    /* allocate memory for the eq. system */
    LDT->SYST = (struct_eq_system *)malloc(sizeof(struct_eq_system));
    LDT->SYST->nr = LDT->tot_nb_pts;
    LDT->SYST->nc = 3;
    LDT->SYST->A = (float **)
      matrix_alloc(LDT->SYST->nr, LDT->SYST->nc, sizeof(float));
    LDT->SYST->b = (float *) vector_alloc(LDT->SYST->nr, sizeof(float));
    
    if(sys_file_name[0]=='\0') { /* no syst_in files */
      /* create eq from image */
      int i=0;
      for (x=0; x < LDT->image.dim.x; x++)
	for (y=0; y < LDT->image.dim.y; y++) {
	  if (LDT->image.im[y][x]!=LDT->image.background_color) {
	    (LDT->SYST->A)[i][0] = (x + .5 - (float)LDT->image.dim.x/2);   
	    (LDT->SYST->A)[i][1] = (y + .5 - (float)LDT->image.dim.y/2);
	    (LDT->SYST->A)[i][2] = 1;
	    (LDT->SYST->b)[i] = 0;
	    i++;
	  }
	}
    }
    else { /* read the eq system directly from syst_in file */
      float aux_f;
      for(int i=0; i<LDT->tot_nb_pts; i++) {
	(LDT->SYST->A)[i/3][i%3] = read_float_from_file(eq_file);
	(LDT->SYST->b)[i] = 0;
      }
      fclose(eq_file);
    }
  }
  else { /* clustering */
    int i=0;
    int cl2= LDT->cluster*LDT->cluster;
    int cl = LDT->cluster;
    int delta_x = LDT->image.dim.x/cl;
    int delta_y = LDT->image.dim.y/cl;
    int x_min,x_max,y_min,y_max;

    LDT->cluster_edge = (int **)
      matrix_alloc(LDT->cluster*LDT->cluster, 4, sizeof(int));

    LDT->SYST_CLUSTERS = (struct_eq_system **)
      vector_alloc(cl2, sizeof(struct_eq_system *));

    for (int j=0; j<cl2; j++) { /* cl^2 = total nb of cluster */
      /* allocate memory for current cluster */
      LDT->SYST_CLUSTERS[j] = (struct_eq_system *)
	malloc(sizeof(struct_eq_system));
      LDT->SYST_CLUSTERS[j]->A=(float **)
	vector_alloc(LDT->tot_nb_pts, sizeof(float *));
      LDT->SYST_CLUSTERS[j]->b=(float *)
	vector_alloc(LDT->tot_nb_pts, sizeof(float));
      
      /* calculate current cluster edge */
      LDT->cluster_edge[j][0] = x_min = (j%cl)*delta_x;
      LDT->cluster_edge[j][1] = x_max
			      = j%cl!=cl-1 ? x_min+delta_x : LDT->image.dim.x;
      LDT->cluster_edge[j][2] = y_min = (j/cl)*delta_y;
      LDT->cluster_edge[j][3] = y_max
			      = j/cl!=cl-1 ? y_min+delta_y : LDT->image.dim.y;
      /* scan image for current cluster */
      i = 0;
      for (x=x_min; x<x_max; x++)
	for (y=y_min; y<y_max; y++) {
	  if (LDT->image.im[y][x]!=LDT->image.background_color) {
	    LDT->SYST_CLUSTERS[j]->A[i] = (float *)
	      vector_alloc(3, sizeof(float));
	    LDT->SYST_CLUSTERS[j]->A[i][0] = (x + .5 - (float)LDT->image.dim.x/2);   
	    LDT->SYST_CLUSTERS[j]->A[i][1] = (y + .5 - (float)LDT->image.dim.y/2);
	    LDT->SYST_CLUSTERS[j]->A[i][2] = 1;
	    LDT->SYST_CLUSTERS[j]->b[i] = 0;
	    i++;
	  }
	}
      /* realloc & init current cluster */
      LDT->SYST_CLUSTERS[j]->A=(float **)
	realloc(LDT->SYST_CLUSTERS[j]->A,i*sizeof(float *));
      LDT->SYST_CLUSTERS[j]->b=(float *)
	realloc(LDT->SYST_CLUSTERS[j]->b,i*sizeof(float));
      LDT->SYST_CLUSTERS[j]->nc = 3;
      LDT->SYST_CLUSTERS[j]->nr = i;
    }
  }
}

/***********************************************************************/
/* Create conic detection equation system			       */
/***********************************************************************/
void create_cdetect_equations(struct_cdetect *CDT)
{
  int x,y;
  int i=0;

  /* Calculate number of points */
  for (y = 0; y < CDT->image.dim.y; y++)
    for (x = 0; x < CDT->image.dim.x; x++)
      if (CDT->image.im[y][x]!=CDT->image.background_color) CDT->tot_nb_pts++;

  /* Add noise */
  CDT->tot_nb_pts_no_noise = CDT->tot_nb_pts;
  if (CDT->noise) {
    for (int j=0; j < CDT->tot_nb_pts*CDT->noise/100; j++)
      if (CDT->image.background_color!=255)
	CDT->image.im[(int)floor(drand48()*CDT->image.dim.y)]
	  [(int)floor(drand48()*CDT->image.dim.x)]=255;
      else
	CDT->image.im[(int)floor(drand48()*CDT->image.dim.y)]
	  [(int)floor(drand48()*CDT->image.dim.x)]=0;
    CDT->tot_nb_pts = 0;
    for (y = 0; y < CDT->image.dim.y; y++)
      for (x = 0; x < CDT->image.dim.x; x++)
	if (CDT->image.im[y][x]!=CDT->image.background_color) CDT->tot_nb_pts++;
  }

  if (CDT->cluster==0) { /* no clustering */
    /* allocate memory for the eq. system */
    CDT->SYST = (struct_eq_system *)malloc(sizeof(struct_eq_system));
    CDT->SYST->nr = CDT->tot_nb_pts;
    CDT->SYST->nc = 6;
    CDT->SYST->A=(float **)
      matrix_alloc(CDT->SYST->nr, CDT->SYST->nc, sizeof(float));
    CDT->SYST->b=(float *)  vector_alloc(CDT->SYST->nr, sizeof(float));
   
    for (x=0; x < CDT->image.dim.x; x++)
      for (y=0; y < CDT->image.dim.y; y++) {
	if (CDT->image.im[y][x]!=CDT->image.background_color) {
	  (CDT->SYST->A)[i][0] = x * x;   
	  (CDT->SYST->A)[i][1] = x * y;
	  (CDT->SYST->A)[i][2] = y * y;
	  (CDT->SYST->A)[i][3] = x;   
	  (CDT->SYST->A)[i][4] = y;
	  (CDT->SYST->A)[i][5] = 1;
	  (CDT->SYST->b)[i] = 0;
	  i++;
	}
      }
  }
  else exit_with_error("Clustering not supported with conic_detection");
}


/**********************************************************************/
/* Create conic_2 detection equation system			      */
/**********************************************************************/
void create_c23detect_equations(struct_cdetect *CDT)
{
  int x,y;
  int i=0;

  /* Calculate number of points */
  for (y = 0; y < CDT->image.dim.y; y++)
    for (x = 0; x < CDT->image.dim.x; x++)
      if (CDT->image.im[y][x]!=CDT->image.background_color) CDT->tot_nb_pts++;

  /* Add noise */
  CDT->tot_nb_pts_no_noise = CDT->tot_nb_pts;
  if (CDT->noise) {
    for (int j=0; j < CDT->tot_nb_pts*CDT->noise/100; j++)
      if (CDT->image.background_color!=255)
	CDT->image.im[(int)floor(drand48()*CDT->image.dim.y)]
	  [(int)floor(drand48()*CDT->image.dim.x)]=255;
      else
	CDT->image.im[(int)floor(drand48()*CDT->image.dim.y)]
	  [(int)floor(drand48()*CDT->image.dim.x)]=0;	
    CDT->tot_nb_pts = 0;
    for (y = 0; y < CDT->image.dim.y; y++)
      for (x = 0; x < CDT->image.dim.x; x++)
	if (CDT->image.im[y][x]!=CDT->image.background_color) CDT->tot_nb_pts++;
  }

  if (CDT->cluster==0) { /* no clustering */
    /* allocate memory for the eq. system */
    CDT->SYST = (struct_eq_system *)malloc(sizeof(struct_eq_system));
    CDT->SYST->nr = CDT->tot_nb_pts;
    if (CDT->conic_type==CIRCLE) CDT->SYST->nc = 3; /* circle */
    else CDT->SYST->nc = 5; /* ellipse */
    CDT->SYST->A=(float **)
      matrix_alloc(CDT->SYST->nr, CDT->SYST->nc, sizeof(float));
    CDT->SYST->b=(float *)  vector_alloc(CDT->SYST->nr, sizeof(float));
   
    for (x=0; x < CDT->image.dim.x; x++)
      for (y=0; y < CDT->image.dim.y; y++) {
	if (CDT->image.im[y][x]!=CDT->image.background_color) {
	  (CDT->SYST->A)[i][0] = x;   
	  (CDT->SYST->A)[i][1] = y;
	  (CDT->SYST->b)[i] = 0;
	  i++;
	}
      }
  }
  else { /* clustering */
    int cl2= CDT->cluster*CDT->cluster;
    int cl = CDT->cluster;
    int delta_x = CDT->image.dim.x/cl;
    int delta_y = CDT->image.dim.y/cl;
    int x_min,x_max,y_min,y_max;

    CDT->cluster_edge = (int **)
      matrix_alloc(CDT->cluster*CDT->cluster, 4, sizeof(int));

    CDT->SYST_CLUSTERS = (struct_eq_system **)
      vector_alloc(cl2, sizeof(struct_eq_system *));

    for (int j=0; j<cl2; j++) { /* cl^2 = total nb of cluster */
      /* allocate memory for current cluster */
      CDT->SYST_CLUSTERS[j] = (struct_eq_system *)
	malloc(sizeof(struct_eq_system));
      CDT->SYST_CLUSTERS[j]->A=(float **)
	vector_alloc(CDT->tot_nb_pts, sizeof(float *));
      CDT->SYST_CLUSTERS[j]->b=(float *)
	vector_alloc(CDT->tot_nb_pts, sizeof(float));
      
      /* calculate current cluster edge */
      CDT->cluster_edge[j][0] = x_min = (j%cl)*delta_x;
      CDT->cluster_edge[j][1] = x_max
			      = j%cl!=cl-1 ? x_min+delta_x : CDT->image.dim.x;
      CDT->cluster_edge[j][2] = y_min = (j/cl)*delta_y;
      CDT->cluster_edge[j][3] = y_max
			      = j/cl!=cl-1 ? y_min+delta_y : CDT->image.dim.y;
      /* scan image for current cluster */
      i = 0;
      for (x=x_min; x<x_max; x++)
	for (y=y_min; y<y_max; y++) {
	  if (CDT->image.im[y][x]!=CDT->image.background_color) {
	    CDT->SYST_CLUSTERS[j]->A[i] = (float *)
	      vector_alloc(3, sizeof(float));
	    CDT->SYST_CLUSTERS[j]->A[i][0] = x;   
	    CDT->SYST_CLUSTERS[j]->A[i][1] = y;
	    CDT->SYST_CLUSTERS[j]->b[i] = 0;
	    i++;
	  }
	}
      /* realloc & init current cluster */
      CDT->SYST_CLUSTERS[j]->A=(float **)
	realloc(CDT->SYST_CLUSTERS[j]->A,i*sizeof(float *));
      CDT->SYST_CLUSTERS[j]->b=(float *)
	realloc(CDT->SYST_CLUSTERS[j]->b,i*sizeof(float));
      if (CDT->conic_type==CIRCLE) CDT->SYST_CLUSTERS[j]->nc = 3; /* circle */
      else CDT->SYST_CLUSTERS[j]->nc = 5; /* ellipse */
      CDT->SYST_CLUSTERS[j]->nr = i;
    }
  }
}
















































