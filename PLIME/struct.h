/****************************************************************************/
/*                                                                          */
/* Declaration of types                                                     */
/*                                                                          */
/****************************************************************************/

#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#include "tcl.h"

typedef struct
{
  int x;
  int y;
}
point_2d_int;

typedef struct
{
  float x;
  float y;
}
point_2d_float;

typedef struct
{
  int min;
  int max;
}
range_int;

/****************************************************************************/
/* struct_tk | Tk graphical windows informations                            */
/****************************************************************************/
typedef struct
{
  Tcl_Interp *interp;
  char canvas_tag[20];
}
struct_tk;

/****************************************************************************/
/* struct_Bench								    */
/****************************************************************************/
typedef struct
{
  char result_file_name[100]; /* bench file name                            */
  float result;		      /* % of well classified eq.                   */
  int nb_subsys;	      /* true nb of subsys                          */
  float computation_time;     /* time spend for relaxation only             */
  float postprocessing_time;  /* time spend for postprocessing only         */
  float time;		      /* calculation time			    */
}
struct_bench;

/****************************************************************************/
/* struct_image								    */
/****************************************************************************/
typedef struct
{
  unsigned char **im;	      /* input image				    */
  point_2d_int dim;	      /* dimension of image			    */
  int	background_color;     /* background color of bmp image       	    */
}
struct_image;

/****************************************************************************/
/* struct_graph_monitoring | graphical monitoring parameters & info.	    */
/****************************************************************************/
typedef struct
{

}
struct_graph_monitoring;

/****************************************************************************/
/* struct_algo_param | Algorithm parameters from command line		    */
/****************************************************************************/
typedef struct
{
			/* ******* automatic setting of temperature	    */
  int     auto_temp_set;  /* flag					    */
   float	tzero;	  /*					            */
   float	update_tz;/* update factor for tzero		            */ 
   float	alpha_tz; /* fraction of old tzero for update	            */
  
  int winnow;           /* enable winnow algorithm                          */
  float	T;		/* initial temperature 			            */
  float T_param;	/* parameter for temp. function			    */
  int	exponential;	/* exponential decrease of the temperature          */
			/* (linear decrease if set to 0)	            */
  int	max_num_subsys;	/* maximum number of susbsystem iterations          */
  int   stop_cycles;    /* inferior limit of the size of the system         */
  float	stop_cycles_prop;/*inf subsys sizes prop. to typical one (%)	    */
  int   stop_temp;	/* superior limit of avg/ti			    */
  int	nb0_cycles;	/* initial number of iterations                     */
  float partial;	/* % of pts to randomly take at each iteration	    */

  float	epsilon;       	/* error tolerated when classifying equations       */
  float relative_eps;	/* flag to take epsilon as a relative value	    */
  float no_corrections; /* flag that does not allow corrections to the      */
                        /* solution if the maximum allowed error is not     */
                        /* exceed by the current solution.                  */
  float alpha;          /* parameter witch determins the correction at each */
                        /* step                                             */
  int   rnd;		/* flag witch enable time initialisation of random  */
			/* serie					    */
  struct_bench *BENCH;
  int max_retry;	/* nb max of retry after a non-convergent cycle     */
  int retry_with_last;  /* nb of retry with last result                     */
  int mon;		/* flag witch enable partial results output         */
  int mon_t;		/* flag witch enable temperature monitoring         */
  int mon_ab;		/* flag witch enable sol. mon. in reciprocal space  */
  FILE *mon_t_file;	/* mon_t file					    */

  char syst_out_file_name[100];/* name of out files for eq syst (-syst_out) */
  char syst_in_file_name[100]; /* name of in files for eq syst (-syst_in)   */
  int syst_in_file;	       /* flag to indicate the presence of -syst_in */
  int graph;		   /* flag witch enable real time graphical display */
  int mon_well_classified; /* flag witch enable graphical monitoring of     */
			   /* well classified points			    */
  int draw_bad_subsystem;  /* flag witch enable graphical monitoring of     */
			   /* irrelevant subsystems			    */
}
struct_algo_param;

/****************************************************************************/
/* struct_algo | Plime algoritm variables				    */
/****************************************************************************/
typedef struct
{
  int step;              /* number of segment being currently detected      */
  int nb0_couples;       /* initial number of equations                     */
  int nb_couples;        /* numbers of couples of equations which have not  */
                         /* yet been classified.                            */
  int *equations;        /* array containing the equations which have not   */
                         /* yet been classified.                            */
  int num_extr_subsys;   /* number of extracted subsystem                   */
  float t0,ti;           /* current temperatures                            */
  int equation_index;    /* index of current equation in eq. list           */

  int last_subsys_size;		/* dimension of last extracted subsystem    */
  int last_subsys_size_data;    /* idem but based on all equations          */
  int last_subsystem_enhanced_size; /* idem but with enhanced postprocessing*/
  int last_subsystem_typical_size;  /* 2*pi*last_r*prop_in_image            */
  int last_subsys_size_threshold;   /* size threshold of last subsystem	    */

  int last_subsystem_accepted;  /* flag = true if last subsyst was accepted */
  int last_subsystem_nb_cycles; /* effective nb of cycles for last subsys.  */

  int index_cycle;       /* index of outer loop                             */
  int iteration;         /* index of inner loop                             */
  int nb_cycles;         /* number of iterations for current subsystem      */

  float *weight;	 /* correction weights				    */
  int retry_flag;	 /* flag for retry cycle       	         	    */

  int nb_of_correction;  /* tot nb of correction                            */
}
struct_algo;

/****************************************************************************/
/* struct_eq_system | Equation system					    */
/****************************************************************************/
typedef struct
{
                        /* ******* Eq system                                */
  float **A;            /* matrix A                                         */
  float *b;             /* vector b                                         */
  int   nr,nc;          /* size of A                                        */
}
struct_eq_system;

/****************************************************************************/
/* struct_ARmodel | AR model						    */
/****************************************************************************/
typedef struct
{
  struct_eq_system *SYST;
  float *norm_eq;

  int	num_data;	/* number of time series points		            */
  int	order;		/* order of the AR model		            */
  int	nb_pt_corr;	/* */
  float *time_series;	/* vector of time series data		            */
  float *time_series2;	/* vector of dependant time series		    */
  range_int index;	/* partial file extraction			    */

  float av_abs_err;        /* average absolute error of current subsystem   */
  float total_av_abs_err;  /* total average abs error ...                   */
  float av_quadr_err;      /* average quadratic error of current subsystem  */
  float total_av_quadr_err;/* total average quadratic error ...             */

  float **solutions;       /* matrix of the various AR solutions            */
  int	*sizes;	           /* size of subsystems                            */
  int	contiguous;	   /* flag to enable contiguous partitionning	    */
  int	*partition_begin;  /*						    */
  int	*partition_end;	   /*						    */
  int	*partitions;	   /* index of chosen solution for each equation    */
	
			/* ******* file names				    */
  char inname[100];       /* file name (in)				    */
  char inname2[100];      /* file name dependant (in2)   		    */
  char out_name[100];     /*						    */
  char mapping_name[100]; /* mapping file name		                    */
}
struct_ARmodel;

/****************************************************************************/
/* struct_optical | Optical flow 					    */
/****************************************************************************/
typedef struct
{
  struct_eq_system *SYST;

  float **mov_x;        /* matrices containing velocities in dir. of x & y  */
  float **mov_y;        /* ...                                              */
  int   dim_x;		/* x-axis dimension of image	      	            */
  int   dim_y;		/* y-axis dimension of image                        */

			/* ******* file names                               */
  char mov_x_name[100]; /* name of file containing velocities (-x)	    */
  char mov_y_name[100]; /* ... (-y)					    */

			/* ******* files                                    */
  FILE * mov_x_file;	/* file containing velocities in direction of x & y */
  FILE * mov_y_file;	/* ...                                              */

  float **solutions;    /* matrix of the solutions                          */
  int *sizes;	        /* size of subsystems                               */
  int *nb_retry;	/* nb of retry for each subsys                      */

  int **out;
  char **out0;
}
struct_optical;

/****************************************************************************/
/* struct_ldetect | Line detection 					    */
/****************************************************************************/
typedef struct
{
  struct_tk *TK;
  struct_eq_system *SYST;
			/* ******* clustering */
  struct_eq_system **SYST_CLUSTERS;
  int	**cluster_edge;	 /* array of clusters edges			    */
  int   cluster;         /* nb of cluster (tot nb of cluster = cluster^2)   */
  int	current_cluster; /* index of the current cluster		    */
  int	*corr_cl;	 /* corresponding cluster of each subsys       	    */

  struct_image image;	/* input image					    */
  int   last_as_start;  /* flag to use last solution as starting point      */
  float delta_c_min;    /* minimum variation of c in one cycle              */

  char image_file_name[100]; /* name of image file (-in)	            */
  int   bmp_flag;	/* flag to input data as bmp file                   */
  int	bmp_out_flag;	/* flag to enable bmp result file		    */
  float noise;		/* noise to add in % of the total nb of points	    */
  int   tot_nb_pts;	/* total number of points			    */
  int   tot_nb_pts_no_noise;
  range_int index_x,index_y; /* partial image extraction                    */
  int	segment;	/* length of windows for segment detection	    */
  int	seg_win_threshold; /* threshold for segment detection		    */
  float seg_epsilon;    /* epsilon for segmentation (epsilon as default)    */
  int	tan_estimation;	/* size of the neighbourhood for LMS tan estimation */
			/* default is 0 i.e. AMS relaxation algorithm	    */
  int	remove_bad_pts; /* flag to enable pts filterring (with corr. coeff.)*/
  point_2d_float u;	/* Direction vector of tan. of the current point    */

  float **solutions;    /* matrix of the solutions                          */
  int	*sizes;	        /* size of subsystems                               */
  int	*nb_retry;	/* nb of retry for each subsys                      */
  int	**segments;	/* array of segment extremities (when segment != 0) */
  
  float *last_cycle_starting_point;  /*                                     */

  float a_lms;		/* lms estimation of a (y=ax+b)			    */
  float sum_xi;		/* sum(xi-E[xi])^2)				    */
  float *sq_error;	/* sq sum of errors of corresponding subsys         */
  float *mean;		/* mean of residu of corr...			    */
}
struct_ldetect;

/****************************************************************************/
/* struct_cdetect | Conic detection        				    */
/****************************************************************************/
#define GENERAL 1
#define CIRCLE 2
#define ELLIPSE 3
typedef struct
{
  int	conic_type;	/* 1:general / 2:circle / 3:ellipse		    */
  struct_tk *TK;
  struct_eq_system *SYST;
			/* ******* clustering */
  struct_eq_system **SYST_CLUSTERS;
  int	**cluster_edge;	 /* array of clusters edges			    */
  int   cluster;         /* nb of cluster                                   */
  int	current_cluster; /* index of the current cluster		    */
  int	*corr_cl;	 /* corresponding cluster of each subsys       	    */

  struct_image image;	/* input image					    */
  int   postprocess_mode;/* flag to enable enhanced postp. with ellipse     */
  int	tan_estimation; /* flag to enable mean sq tan estimation	    */
  int	remove_bad_pts; /* flag to enable pts filterring (with corr. coeff.)*/

  int   rnd_origin;	/* flag to enable random origin                     */
  int   initial_r;      /* radius of starting solution                      */
  point_2d_int origin;	/* current origin (relative to first one)           */

  char image_file_name[100]; /* name of image file (-in)	            */
  int   bmp_flag;	/* flag to input data as bmp file                   */
  int	bmp_out_flag;	/* flag to enable bmp result file		    */
  float noise;		/* noise to add in % of the total nb of points	    */
  int   tot_nb_pts;	/* total number of points			    */
  int   tot_nb_pts_no_noise;

  float **solutions;    /* matrix of the solutions                          */
  int	*sizes;	        /* size of subsystems                               */
  int	*nb_retry;	/* nb of retry for each subsys                      */

  point_2d_float u;	/* Direction vector of tan. of the current point    */
  
  float *last_cycle_starting_point; /*                                      */
}
struct_cdetect;

#endif
