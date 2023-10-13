#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "utils.h"
#include "memory_alloc.h"
#include "bmp_8b.h"

/****************************************************************/
/* AR model							*/
/****************************************************************/

void read_AR_data(struct_ARmodel *AR)
{
   int j;

   FILE * f; /* file containing the time series */

   f = fopen(AR->inname, "r");
   if (f==NULL) exit_with_error ("Can't open file given by -in");

   /* reading of the first integer whitch is the number of data */
   AR->num_data = read_int_from_file(f);
   if (AR->index.min!=0 && AR->index.max!=0) /* partial file extraction */
     if (AR->index.min>AR->num_data || AR->index.max>AR->num_data)
       exit_with_error("index out of range");
     else
       AR->num_data = AR->index.max-AR->index.min;
   else { /* no partial file */
     AR->index.min = 1;
     AR->index.max = AR->num_data;
   }

   if (AR->num_data <= AR->order)
     exit_with_error("Time serie length < order.");

   /* memory allocation for time_series */
   AR->time_series = (float *) (malloc(AR->num_data * sizeof(float)));
   if (AR->time_series==0)
     exit_with_error("Can't allocate memory for AR_time_series");

   /* reading time_series */
   if (AR->index.min>1)
     for (int k=0; k<AR->index.min-1; k++) read_float_from_file(f);
   for (j = 0; j < AR->num_data; j++)
     AR->time_series[j] = read_float_from_file(f);

   fclose (f);

   /* test if -in2 is present */
   if (AR->inname2[0]!='\0') {
     f = fopen(AR->inname2, "r");
     if (f==NULL) exit_with_error ("Can't open file given by -in2");

     /* reading of the first integer whitch is the number of data */
     if (AR->num_data != read_int_from_file(f))
       exit_with_error("-in & -in2 have two different sizes");
     AR->time_series2 = (float *) (malloc(AR->num_data * sizeof(float)));
     if (AR->time_series2==0)
       exit_with_error("Can't allocate memory for AR_time_series2");

     /* reading time_series2 */
     for (j = 0; j < AR->num_data; j++)
       AR->time_series2[j] = read_float_from_file(f);
    
     fclose(f);
   }
}

void init_ar (struct_algo_param *P_ALGO, struct_algo *ALGO, struct_ARmodel *AR)
{ 
  int i;

  ALGO->nb_couples = AR->num_data - AR->order;
  ALGO->nb0_couples = ALGO->nb_couples;
  ALGO->step=0;
  ALGO->nb_of_correction = 0;
  ALGO->num_extr_subsys=0;
  ALGO->nb_cycles = P_ALGO->nb0_cycles;

  ALGO->equations = (int *) vector_alloc(AR->SYST->nr, sizeof(int));
  for (i=0; i < ALGO->nb_couples; i++) ALGO->equations[i]=i;

  AR->solutions = (float **) matrix_alloc (P_ALGO->max_num_subsys + 1,
					   AR->SYST->nc, sizeof(float));
  AR->sizes = (int *) vector_alloc (P_ALGO->max_num_subsys + 1, sizeof(int));

  if (AR->contiguous) {
    AR->partition_begin = (int *)
      vector_alloc (P_ALGO->max_num_subsys + 1, sizeof(int));
    AR->partition_end = (int *)
      vector_alloc (P_ALGO->max_num_subsys + 1, sizeof(int));
  }
  AR->partitions = (int*) vector_alloc (AR->SYST->nr, sizeof(int));
  for (i=0; i < ALGO->nb_couples; i++) AR->partitions[i]=0;
  
  /* no weight correction for AR model */
  ALGO->weight = (float *) vector_alloc (AR->SYST->nc, sizeof(float));
  for (i=0; i < AR->SYST->nc; i++) ALGO->weight[i]=1;

  if (P_ALGO->stop_cycles < AR->order) P_ALGO->stop_cycles=AR->order;

  AR->av_abs_err = 0;
  AR->total_av_abs_err = 0;
  AR->av_quadr_err = 0;
  AR->total_av_quadr_err = 0;
}

/****************************************************************/
/* Optical flow							*/
/****************************************************************/

void read_optical_data (struct_optical *OPT)
{
  int i,j;
  int x=OPT->dim_x;
  int y=OPT->dim_y;

  /* open image files given by the command line parameters -x & -y */
  OPT->mov_x_file= fopen(OPT->mov_x_name,"r");
  if (OPT->mov_x_file==NULL) exit_with_error ("Can't open file given by -x");

  OPT->mov_y_file= fopen(OPT->mov_y_name,"r");
  if (OPT->mov_y_file==NULL) exit_with_error ("Can't open file given by -y");

  /* allocate memory & read files */
  OPT->mov_x = (float **)matrix_alloc(x, y, sizeof(float));
  OPT->mov_y = (float **)matrix_alloc(x, y, sizeof(float));

  for (i=0; i<y; i++)
    for (j=0; j<x; j++) {
      OPT->mov_x[i][j] = read_float_from_file(OPT->mov_x_file);
      OPT->mov_y[i][j] = read_float_from_file(OPT->mov_y_file);
    }

  /* close files */
  fclose (OPT->mov_x_file);
  fclose (OPT->mov_y_file);
}

void init_optical (struct_algo_param *P_ALGO,struct_algo *ALGO, struct_optical *OPT)
{
  int i,j;

  ALGO->nb_couples = OPT->dim_x * OPT->dim_y;
  ALGO->nb0_couples = ALGO->nb_couples;
  ALGO->step=0;
  ALGO->nb_of_correction = 0;
  ALGO->num_extr_subsys=0;

  ALGO->equations = (int *) vector_alloc(OPT->SYST->nr, sizeof(int));
  for (i=0; i<ALGO->nb_couples; i++) ALGO->equations[i]=i;

  OPT->solutions = (float **)
    matrix_alloc(P_ALGO->max_num_subsys+1, OPT->SYST->nc, sizeof(float));
  OPT->sizes = (int *) vector_alloc (P_ALGO->max_num_subsys+1, sizeof(int));
  OPT->out0 = (char **) matrix_alloc (OPT->dim_y, OPT->dim_x, sizeof(char));
  OPT->out = (int **) matrix_alloc (OPT->dim_y, OPT->dim_x, sizeof(int));
  for (i=0; i<OPT->dim_y; i++)
    for (j=0; j<OPT->dim_x; j++)
      OPT->out[i][j]=0;

  ALGO->weight=(float *) vector_alloc(OPT->SYST->nc, sizeof(float));
}

/****************************************************************/
/* line detection      						*/
/****************************************************************/

void read_ldetect_data (struct_ldetect *LDT)
{
  int i,j;
  FILE *image_file;

  if (LDT->bmp_flag) { /* input file is BMP */
    /* open bmp file given by the command line parameter -in */
    char bmp_file_name[100];
    sprintf(bmp_file_name,"%s.bmp",LDT->image_file_name);
    image_file= fopen (bmp_file_name,"rb");
    if (image_file==NULL) exit_with_error("Can't open file",bmp_file_name);
    /* load bmp file */
    LDT->image.im = load_bmp_8b(bmp_file_name, &LDT->image.dim);
      }
  else { /* input file is ASCII */
  /* open image file given by the command line parameter -in */
  image_file= fopen (LDT->image_file_name,"r");
  if (image_file==NULL) exit_with_error ("Can't open file",LDT->image_file_name);

  /* allocate memory & read file */
  LDT->image.im=(unsigned char **)
    matrix_alloc(LDT->image.dim.x, LDT->image.dim.y, sizeof(unsigned char));

  for (i=0; i<LDT->image.dim.y; i++)
    for (j=0; j<LDT->image.dim.x; j++)
      LDT->image.im[i][j]=(unsigned char)read_int_from_file(image_file);
  }
  /* close file */
  fclose (image_file);
}

void init_ldetect (struct_algo_param *P_ALGO, struct_algo *ALGO, struct_ldetect *LDT)
{
  int i,j;

  ALGO->nb0_couples = ALGO->nb_couples = LDT->SYST->nr;
  ALGO->step=0;
  ALGO->nb_of_correction = 0;
  ALGO->num_extr_subsys=0;

  /* memory allocation */
  ALGO->equations = (int *) vector_alloc(LDT->tot_nb_pts, sizeof(int));

  //  ALGO->weight=(float *) vector_alloc(LDT->SYST->nc, sizeof(float));
  ALGO->weight=(float *) vector_alloc(3, sizeof(float));

  LDT->solutions = (float **)
    matrix_alloc(P_ALGO->max_num_subsys+1, 3, sizeof(float));
    //    matrix_alloc(P_ALGO->max_num_subsys+1, LDT->SYST->nc, sizeof(float));
  LDT->sizes = (int *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(int));
  LDT->corr_cl = (int *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(int));

  LDT->nb_retry = (int *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(int));
  for (i=0; i<P_ALGO->max_num_subsys+1; i++) LDT->nb_retry[i]=0;
  if (LDT->segment)
    LDT->segments = (int**) matrix_alloc(P_ALGO->max_num_subsys+1, 4,sizeof(int));
  /* statistic data */
  LDT->mean = (float *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(float));
  LDT->sq_error = (float *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(float));
  for (i=0; i<P_ALGO->max_num_subsys+1; i++) LDT->sq_error[i]=LDT->mean[i]=0;

  /* init of the starting point of the last cycle */
  LDT->last_cycle_starting_point = (float *) vector_alloc(3, sizeof(float));
}

/****************************************************************/
/* conic detection      					*/
/****************************************************************/
void read_cdetect_data (struct_cdetect *CDT)
{
  FILE *image_file;

  if (CDT->bmp_flag) { /* input file is BMP */
    /* open bmp file given by the command line parameter -in */
    char bmp_file_name[100];
    sprintf(bmp_file_name,"%s.bmp",CDT->image_file_name);
    image_file= fopen (bmp_file_name,"rb");
    if (image_file==NULL) exit_with_error("Can't open file",bmp_file_name);
    /* load bmp file */
    CDT->image.im = load_bmp_8b(bmp_file_name, &CDT->image.dim);
    /* close file */
    fclose (image_file);
      }
  else { /* no yet implemented */
    exit_with_error ("Conic detection need bmp file");
  }
}

void init_cdetect (struct_algo_param *P_ALGO, struct_algo *ALGO,
		   struct_cdetect *CDT)
{
  int i;
  ALGO->nb0_couples = ALGO->nb_couples = CDT->SYST->nr;
  ALGO->step=0;
  ALGO->nb_of_correction = 0;
  ALGO->num_extr_subsys=0;
  /* memory allocation */
  ALGO->equations = (int *) vector_alloc(CDT->tot_nb_pts, sizeof(int));
  //  ALGO->weight=(float *) vector_alloc(CDT->SYST->nc, sizeof(float));
  ALGO->weight=(float *) vector_alloc(6, sizeof(float));
  CDT->solutions = (float **)
    matrix_alloc(P_ALGO->max_num_subsys+1, 6, sizeof(float));
    //    matrix_alloc(P_ALGO->max_num_subsys+1, CDT->SYST->nc, sizeof(float));

  CDT->sizes = (int *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(int));

  CDT->nb_retry = (int *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(int));
  for (i=0; i<P_ALGO->max_num_subsys+1; i++) CDT->nb_retry[i]=0;

  CDT->corr_cl = (int *) vector_alloc(P_ALGO->max_num_subsys+1, sizeof(int));

  /* init of the starting point of the last cycle */
  CDT->last_cycle_starting_point = (float *) vector_alloc(6,sizeof(float));
}
