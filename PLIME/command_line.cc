#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "leon.h"
#include "struct.h"
#include "utils.h"

/***********************************************************************/
/* Usage							       */
/***********************************************************************/
void usage(char error_msg[]){
  if (strlen(error_msg)) printf ("%s\n\n",error_msg);
  printf(" usage: \n");
  printf("Common parameters:\n");
  printf("    -F                xxx     config filename.\n");
  printf("    -cycles           #       number of cycles\n");
  printf("    -max_num_subsys   #       maximum number of subsystems\n");
  printf("    -epsilon          #.#     error tolerance\n");
  printf("    -no_corr                  no correction to the current solution if\n");
  printf("                              the current solution does not exceed epsilon\n");
  printf("    -stop_cycles      #       abs inferior limit of the size of the subsys.\n");
  printf("    -stop_cycles_prop #.#     inf. subsys. size prop. to typical one (%%)\n");
  printf("    -stop_temp        #.#     superior limit of dist/t\n");
  printf("    -max_retry        #       max nb of retry\n");
  printf("    -retry_with_last  #       nb of retry with last result\n");
  printf("    -no_pts_rmv               do not remove classified points\n");
  printf("    -rnd                      Time initialisation of random number generator.\n\n");

  printf("  Manual temperature:\n");
  printf("    -T                #.#     initial temperature\n");
  printf("    -alpha            #.#     correction parameter\n");
  printf("    -exponential              exponential decrease of the temperature\n");
  printf("  Automatic temperature:\n");
  printf("    -auto_temp_set            Automatic initial temperature setting \n");
  printf("    -tzero            #.#     gain factor for initial tzero\n");
  printf("    -update_tz        #.#     gain factor for update tzero\n");
  printf("    -alpha_tz         #.#     fraction of old tzero\n\n");

  printf("AR modeling parameters:\n");
  printf("    -AR_model                 enable AR modeling\n");
  printf("    -winnow                   apply winnow algorithm\n");
  printf("    -in               xxx     name of input file for AR modeling,\n");
  printf("    -in2              xxx     name of dependant AR input file.\n");
  printf("    -index            x x     partial file extraction.\n");
  printf("    -AR_order         #       order of the AR model\n");
  printf("    -AR_out_file      xxx     name of AR output file\n");
  printf("    -contiguous       xxx     force contiguous partitionning\n\n");

  printf("Optical flow modeling:\n");
  printf("    -optical_flow             enable optical flow modeling \n");
  printf("    -x                xxx     name of x-motion input file for real image\n");
  printf("    -y                xxx     name of y-motion input file for real image\n");
  printf("    -dim_x            #       x dimension for real image\n");
  printf("    -dim_y            #       y dimension for real image\n");
  printf("    -param            #       number of parameters\n\n");

  printf("Line detection:\n");
  printf("    -line_detection           enable line detection\n");
  printf("    -tan_estimation   #       enable line detection with local tan. estimation\n");
  printf("                              it specify the size of neighbourhood for LMS\n");
  printf("                              (default is 0 i.e. AMS relaxation algorithm)\n");
  printf("    -segment		#	length of the sliding window for segment\n");
  printf("				extraction. (odd integer value)\n");
  printf("    -seg_win_threshold #      threshold for segment extraction\n");
  printf("                              (default val. is segment/2)\n");
  printf("    -seg_epsilon      #.#     epsilon for segment extraction (def. epsilon)\n");
  printf("    -mon              #       display partial results\n");
  printf("                              (0=no disp, 1=graph, 2=graph+txt)\n");
  printf("    -bmp                      input file is in bmp format\n");
  printf("    -back_color       #       background color of bmp file (def. 0)\n");
  printf("    -bmp_out                  create bmp result file\n");
  printf("    -cluster          #       nb of cluster (0=no clustering)\n");
  printf("    -noise            #.#     add. noise (%% of total nb of equation)\n");
  printf("    -partial          #.#     prop. of pts to randomly take at each iteration\n");
  printf("    -delta_c_min      #.#     minimum variation of c in one cycle\n");
  printf("    -last_as_start            use last solution as starting point\n\n");

  printf("Conic_detection:\n");
  printf("    -conic_detection          enable general conic detection (6 parameters)\n");
  printf("    -conic2_detection         enable circle detection\n");
  printf("    -conic3_detection         enable ellipse detection\n");
  printf("    -relative_epsilon #.#     relative error tolerance (%% of R)\n");
  printf("                              (|err|<max[epsilon,relative_epsilon/100*R])\n");
  printf("    -initial_r        #       radius of initial solution\n");
  printf("    -remove_bad_pts           flag to enable bad points filtering\n");
  printf("                              (remove pts where tangent estimation is incorrect\n");
  printf("                               i.e. |corr. coeff.| < %4.2f)\n",CORRELATION_THRESHOLD);
  printf("    -postprocess_mode #       0=simple threshold / 1=enhanced threshold\n");
  printf("    -tan_estimation   #       size of neighbourhood for LMS tangent estimation\n\n");
  printf("       -result        File name in witch add final results (bench)\n");
  printf("       -mon_t         Temperature monitoring (create 'mon_t.txt').\n");
  printf("       -mon_ab        Solution monitoring (create 'mon_ab_#.txt').\n\n");
  printf("       -syst_out      xxx     output eq system in two files (xxx_a.txt & xxx_b.txt)\n");
  printf("       -syst_in       xxx     intput eq system in two files (xxx_a.txt & xxx_b.txt)\n");
  printf("       -graph         Enable real-time graphical display.\n");
  printf("        -mon_well_classified  Enable graphical display of well classified pts.\n");
  printf("        -draw_bad_subsystem   Enable graphical display of bad subsystem.\n");
  exit(0);
}

/***********************************************************************/
/* if -F option (i.e. there is a config file)			       */
/* argv is realloced and update with command line		       */
/* informations comming from config file.			       */
/***********************************************************************/

char**  update_command_line(int *argc, char *argv[])
{
  int i;
  char file_name[50];
  FILE *f;
  char temp_argv[MAX_CMD_LINE_PARAM][50];
  char **new_argv;
  int new_argc=0;
  file_name[0]='\0';

  for (i=0 ; i<*argc-1; i++)
    if (!strcmp(argv[i],"-F"))
      strcpy(file_name, argv[i+1]);

  if (file_name[0]!='\0') { /* -F enable */
    f=fopen(file_name, "r");
    if (f==NULL) exit_with_error("Can't open file given by -F");
    /* read config file */
    while ((read_string_from_file(temp_argv[new_argc++], 50, f)) 
	   && (new_argc<MAX_CMD_LINE_PARAM-1));
    fclose(f);

    if (new_argc >= MAX_CMD_LINE_PARAM-1)
      exit_with_error("Too many command line parameters in *.cfg file.\n     (MAX_CMD_LINE_PARAM in source code)");

    /* Make new argv with command line & file's parameters */
    new_argv=(char **) malloc((new_argc + *argc)*sizeof(char *));
    for (i=0; i< new_argc; i++) {
      new_argv[i]= (char *) malloc(strlen(temp_argv[i])*sizeof(char));
      strcpy(new_argv[i], temp_argv[i]);
    }
    for (i=1; i<*argc; i++) new_argv[i - 1 + new_argc]=argv[i];
    *argc +=new_argc;
  }
  else { /* no -F option */
    new_argv=(char **) malloc((*argc + 1) * sizeof(char *));
    for (i=0; i<*argc; i++)
      new_argv[i]=argv[i];
    (*argc)++;
  }

    /* Add a null element to argv to prevent core dump              */
    /* with a wrong command line such as "plime -AR_model -in" ...  */
    new_argv[*argc-1]=(char *) malloc(sizeof(char));
    new_argv[*argc-1][0]='\0';

//for(i=0;i<*argc;i++) printf("%d / %s\n",i,new_argv[i]);

  return new_argv;
}

/***********************************************************************/
/* scan command line and return an integer witch depend		       */
/* on the kind of data to process.				       */
/* AR           <=> 1						       */
/* optical_flow <=> 2	(6 parameters)				       */
/* optical_flow <=> 3   (3 parameters)				       */
/* line         <=> 4						       */
/* conic        <=> 5						       */
/* conic2	<=> 6	(circle)       				       */
/* conic3	<=> 7	(ellipse)      				       */
/***********************************************************************/

int read_command_line_data_type(int argc,char * argv[])
{ 
  int i;
  int data_type=0;

  if (argc <= 2) usage("");

  for (i=0 ; i<argc ; i++) {
    if (!strcmp(argv[i],"-AR_model")){
      if (data_type==0 || data_type==1)
        data_type=1;
      else
        exit_with_error("-AR_model is not compatible with previous parameters");
    }
    if (!strcmp(argv[i],"-optical_flow")){
      if (data_type==0 || data_type==2)
        data_type=2;
      else
        exit_with_error("-optical_flow is not compatible with previous parameters");
     }
    if (!strcmp(argv[i],"-line_detection")){
      if (data_type==0 || data_type==4)
        data_type=4;
      else
        exit_with_error("-line_detection is not compatible with previous parameters");
     }
    if (!strcmp(argv[i],"-conic_detection")){
      if (data_type==0 || data_type==5)
        data_type=5;
      else
        exit_with_error("-conic_detection is not compatible with previous parameters");
     }
    if (!strcmp(argv[i],"-conic2_detection")){
      if (data_type==0 || data_type==6)
        data_type=6;
      else
        exit_with_error("-conic2_detection is not compatible with previous parameters");
     }

    if (!strcmp(argv[i],"-conic3_detection")){
      if (data_type==0 || data_type==7)
        data_type=7;
      else
        exit_with_error("-conic3_detection is not compatible with previous parameters");
     }

  }
  if (data_type==0) exit_with_error ("Data type is not specified");
  return data_type;
}

/***********************************************************************/
/* scan command line for commun options & parameters		       */
/***********************************************************************/

void read_command_line_common(int argc,char * argv[],struct_algo_param *PAR)
{ 
int i;
/* default parameters */
PAR->auto_temp_set = 0;
PAR->tzero = 2;
PAR->update_tz = 1.5;
PAR->alpha_tz = 0.4;
PAR->T = 30;
PAR->T_param = 0.5;
PAR->exponential = 0;
PAR->max_num_subsys = 10;
PAR->nb0_cycles = 5000;
PAR->stop_cycles = 0;
PAR->stop_cycles_prop = 0;
PAR->epsilon = 0.1;
PAR->relative_eps = 0;
PAR->no_corrections = 0;
PAR->alpha = 0.001;
PAR->rnd = 0;
PAR->BENCH->result_file_name[0] = '\0';
PAR->BENCH->nb_subsys = 0;
PAR->max_retry = 3;
PAR->retry_with_last = 1;
PAR->stop_temp = 300;
PAR->mon = 0;
PAR->mon_t = 0;
PAR->mon_ab = 0;
PAR->partial = 1;
PAR->graph = 0;
PAR->mon_well_classified = 0;
PAR->draw_bad_subsystem = 0;
PAR->syst_out_file_name[0] = '\0';
PAR->syst_in_file_name[0] = '\0';
PAR->syst_in_file = 0;
PAR->winnow = 0;

  for (i=1; i<argc; i++) {

/* auto_set_temp */
    if (!strcmp(argv[i],"-auto_temp_set"))
      PAR->auto_temp_set=1;

    if (!strcmp(argv[i],"-tzero")){
      PAR->tzero=atof(argv[++i]);
      if (PAR->tzero<=0) exit_with_error("Invalid value (tzero)");
    }

    if (!strcmp(argv[i],"-update_tz")){
      PAR->update_tz=atof(argv[++i]);
      if (PAR->update_tz<=0) exit_with_error("Invalid value (update_tz)");
    }

    if (!strcmp(argv[i],"-alpha_tz")){
      PAR->alpha_tz=atof(argv[++i]);
      if (PAR->update_tz<0) exit_with_error("Invalid value (alpha_tz)");
    }

/* *** */
    if (!strcmp(argv[i],"-T")){
      PAR->T=atof(argv[++i]);
      if (PAR->T<=0) exit_with_error("Invalid value (T)");
    }

    if (!strcmp(argv[i],"-T_param")){
      PAR->T_param=atof(argv[++i]);
      if (PAR->T_param<0 || PAR->T_param>1)
	exit_with_error("Invalid value (T_param)");
    }

    if (!strcmp(argv[i],"-exponential")){
      PAR->exponential=1;
    }

/* *** */
    if (!strcmp(argv[i],"-max_num_subsys")){
      PAR->max_num_subsys=atoi(argv[++i]);
      if (PAR->max_num_subsys<=0) exit_with_error("Invalid value (max_num_subsys)");
    }

    if (!strcmp(argv[i],"-cycles")){
      PAR->nb0_cycles=atoi(argv[++i]);
      if (PAR->nb0_cycles<=0) exit_with_error("Invalid value (cycles)");
    }

    if (!strcmp(argv[i],"-stop_cycles")){
      PAR->stop_cycles=atoi(argv[++i]);
      if (PAR->stop_cycles < 0) exit_with_error("Invalid value (stop_cycles)");
    }

    if (!strcmp(argv[i],"-stop_cycles_prop")){
      PAR->stop_cycles_prop=atof(argv[++i]);
      if (PAR->stop_cycles_prop < 0) exit_with_error("Invalid value (stop_cycles_prop)");
    }

    if (!strcmp(argv[i],"-stop_temp")){
      PAR->stop_temp=atoi(argv[++i]);
      if (PAR->stop_temp < 0) exit_with_error("Invalid value (stop_temp)");
      if (PAR->stop_temp > 300)
	exit_with_error("Invalid value (0<stop_temp<=300)");
    }

    if (!strcmp(argv[i],"-partial")){
      PAR->partial=atof(argv[++i]);
      if (PAR->partial < 0)
	exit_with_error("Invalid value (partial)");
    }

    if (!strcmp(argv[i],"-max_retry")){
      PAR->max_retry=atoi(argv[++i]);
      if (PAR->max_retry < 0) exit_with_error("Invalid value (max_retry)");
    }

    if (!strcmp(argv[i],"-retry_with_last")){
      PAR->retry_with_last=atoi(argv[++i]);
      if (PAR->retry_with_last < 0 || PAR->retry_with_last > PAR->max_retry)
	exit_with_error("Invalid value (retry_with_last)");
    }

/* epsilon */
    if (!strcmp(argv[i],"-epsilon")){
      PAR->epsilon=atof(argv[++i]);
      if (PAR->epsilon<=0) exit_with_error("Invalid value (epsilon)");
    }

    if (!strcmp(argv[i],"-relative_epsilon"))
      PAR->relative_eps = atof(argv[++i])/100;

/* no_corr */
    if (!strcmp(argv[i],"-no_corr")){
      PAR->no_corrections = atof(argv[++i]);
      if (PAR->no_corrections < 0) exit_with_error("Invalid value (no_corr)");
    }

/* alpha */
    if (!strcmp(argv[i],"-alpha")){
      PAR->alpha = atof(argv[++i]);
      if (PAR->alpha <= 0) exit_with_error("Invalid value (alpha)");
    }

/* rnd option */
    if (!strcmp(argv[i],"-rnd"))
      PAR->rnd=1;

/* winnow option */
    if (!strcmp(argv[i],"-winnow"))
      PAR->winnow=1;

/* bench parameters */
    if (!strcmp(argv[i],"-result"))
      strcpy(PAR->BENCH->result_file_name,argv[++i]);

    if (!strcmp(argv[i],"-nb_subsys"))
      PAR->BENCH->nb_subsys = atoi(argv[++i]);

/* monitoring options */
    if (!strcmp(argv[i],"-mon")) {
      PAR->mon = atoi(argv[++i]);
      if ((PAR->mon < -1) || (PAR->mon > 3))
	exit_with_error ("Invalid value (-mon)");
    }

    if (!strcmp(argv[i],"-mon_t"))
      PAR->mon_t=1;

    if (!strcmp(argv[i],"-mon_ab")) {
      PAR->mon_ab = atoi(argv[i+1]);
      if (PAR->mon_ab<=0) PAR->mon_ab=1;
    }

/* graphical monitoring options */
    if (!strcmp(argv[i],"-graph"))
      PAR->graph=1;

    if (!strcmp(argv[i],"-mon_well_classified"))
      PAR->mon_well_classified = 1;

    if (!strcmp(argv[i],"-draw_bad_subsystem"))
      PAR->draw_bad_subsystem = 1;

/* system input/output file name */
    if (!strcmp(argv[i],"-syst_out"))
      strcpy(PAR->syst_out_file_name,argv[++i]);

    if (!strcmp(argv[i],"-syst_in")) {
      strcpy(PAR->syst_in_file_name,argv[++i]);
      PAR->syst_in_file = 1;
    }
  }
}

/***********************************************************************/
/* scan command line for AR_model parameters			       */
/***********************************************************************/

void read_command_line_AR(int argc,char * argv[],struct_ARmodel *AR)
{
/* default parameters */
AR->order = 1;
AR->index.min = AR->index.max = 0;
AR->contiguous = 0;
AR->nb_pt_corr = 0;
AR->inname[0] = '\0';
AR->inname2[0] = '\0';
AR->out_name[0] = '\0';
AR->mapping_name[0] = '\0';

 for (int i=1 ; i<argc ; i++) {

   /* order */
   if (!strcmp(argv[i],"-AR_order")){
     AR->order=atoi(argv[++i]);
     if (AR->order<=0) exit_with_error ("Invalid value (AR_order)");
   }

   /* index */
   if (!strcmp(argv[i],"-index")){
     AR->index.min = atoi(argv[++i]);
     AR->index.max = atoi(argv[++i]);
     if (AR->index.min<=0 || AR->index.max<=0
	 || AR->index.min >= AR->index.max)
       exit_with_error ("Invalid value (index)");
   }

   /* contiguous option */
   if (!strcmp(argv[i],"-contiguous")) {
     AR->contiguous = atoi(argv[i+1]);
     if (AR->contiguous==0) AR->contiguous=1;
   }

   /* multiple pts correction */
   if (!strcmp(argv[i],"-pt_corr")) {
     AR->nb_pt_corr = atoi(argv[i+1]);
     if (AR->nb_pt_corr<0) exit_with_error ("Invalid value (pt_corr)");
   }

   /* files */
   if (!strcmp(argv[i],"-in"))
      strcpy(AR->inname,argv[++i]);
   
   if (!strcmp(argv[i],"-in2"))
     strcpy(AR->inname2,argv[++i]);
   
   if (!strcmp(argv[i],"-AR_out_file")) strcpy(AR->out_name,argv[++i]);
 }
}

/* validation of AR model parameters */
void valid_AR_parameters(struct_algo_param *P_ALGO, struct_algo *ALGO,
			      struct_ARmodel *AR)
{
  if (P_ALGO->relative_eps)
    exit_with_error("-relative_epsilon not supported with AR model.");
  if (P_ALGO->mon_well_classified)
    exit_with_error("-mon_well_classified not supported with AR model.");

  /* -syst_in & -syst_out */
  if (P_ALGO->syst_in_file)
    exit_with_error("-syst_in not implemented with AR_model");
}


/***********************************************************************/
/* scan command line for optical_flow parameters		       */
/***********************************************************************/

void read_command_line_opt(int argc, char * argv[], struct_optical *OPT)
{
  for (int i=0; i<argc; i++) {

/* files */
    if (!strcmp(argv[i],"-x"))
      strcpy(OPT->mov_x_name, argv[++i]);

    if (!strcmp(argv[i],"-y"))
      strcpy(OPT->mov_y_name, argv[++i]);

    if (!strcmp(argv[i],"-dim_x")) {
      OPT->dim_x= atoi(argv[++i]);
      if (OPT->dim_x<=0) exit_with_error ("Invalid value (dim_x)");
    }

    if (!strcmp(argv[i],"-dim_y")) {
      OPT->dim_y= atoi(argv[++i]);
      if (OPT->dim_y<=0) exit_with_error ("Invalid value (dim_y)");
    }
  }
}

/* validation of optical flow parameters */
void valid_opt_parameters(struct_algo_param *P_ALGO, struct_algo *ALGO,
			      struct_optical *OPT)
{
  if (P_ALGO->relative_eps)
    exit_with_error("-relative_epsilon not supported with optical flow.");
  if (P_ALGO->mon_well_classified)
    exit_with_error("-mon_well_classified not supported optical flow.");

  /* -syst_in & syst_out */
  if (P_ALGO->syst_in_file)
    exit_with_error("-syst_in not implemented with optical_flow");
}

/***********************************************************************/
/* scan command line for line detection parameters		       */
/***********************************************************************/

void read_command_line_ldetect(int argc,char * argv[],struct_ldetect *LDT)
{
  /* default parameters */
  LDT->image_file_name[0] = '\0';
  LDT->bmp_flag = 0;
  LDT->bmp_out_flag = 0;
  LDT->cluster = 0;
  LDT->noise = 0;
  LDT->tan_estimation = 0;
  LDT->remove_bad_pts = 0; 
  LDT->image.background_color = 0;
  LDT->segment = 0;
  LDT->seg_win_threshold = 0;
  LDT->seg_epsilon = 0;
  LDT->image.dim.x = 0;
  LDT->image.dim.y = 0;
  LDT->last_as_start = 0;
  LDT->delta_c_min = 0;

  for (int i=1 ; i<argc ; i++) {
    /* file -in */
    if (!strcmp(argv[i],"-in"))
      strcpy(LDT->image_file_name,argv[++i]);
    
    if (!strcmp(argv[i],"-dim_x")) {
      LDT->image.dim.x = atoi(argv[++i]);
      if (LDT->image.dim.x <= 0) exit_with_error ("Invalid value (dim_x)");
    }

    if (!strcmp(argv[i],"-dim_y")) {
      LDT->image.dim.y = atoi(argv[++i]);
      if (LDT->image.dim.y <= 0) exit_with_error ("Invalid value (dim_y)");
    }

    if (!strcmp(argv[i],"-cluster")) {
      LDT->cluster = atoi(argv[++i]);
      if (LDT->cluster < 0)
	exit_with_error ("Invalid value (cluster)");
    }

   /* index */
   if (!strcmp(argv[i],"-index")){
     LDT->index_x.min = atoi(argv[++i]);
     LDT->index_y.min = atoi(argv[++i]);
     LDT->index_x.max = atoi(argv[++i]);
     LDT->index_y.max = atoi(argv[++i]);
     if (LDT->index_x.min<=0 || LDT->index_x.max<=0
	 || LDT->index_x.min >= LDT->index_x.max
	 || LDT->index_y.min<=0 || LDT->index_y.max<=0
	 || LDT->index_y.min >= LDT->index_y.max)
       exit_with_error ("Invalid value (index)");
   }

    if (!strcmp(argv[i],"-tan_estimation")) {
      LDT->tan_estimation = atoi(argv[++i]);
      if (LDT->tan_estimation < 0)
	exit_with_error ("Invalid value (tan_estimation)");
    }

    if (!strcmp(argv[i],"-remove_bad_pts")) LDT->remove_bad_pts=1;

    if (!strcmp(argv[i],"-noise")) {
      LDT->noise = atof(argv[++i]);
      if (LDT->noise < 0)
	exit_with_error ("Invalid value (noise)");
    }

    if (!strcmp(argv[i],"-bmp")) LDT->bmp_flag=1;
    if (!strcmp(argv[i],"-bmp_out")) LDT->bmp_out_flag=1;

    if (!strcmp(argv[i],"-last_as_start")) LDT->last_as_start=1;

    if (!strcmp(argv[i],"-back_color")) {
      LDT->image.background_color = atoi(argv[++i]);
      if (LDT->image.background_color < 0)
	exit_with_error ("Invalid value (back_color)");
    }

    if (!strcmp(argv[i],"-segment")) {
      LDT->segment = atoi(argv[++i]);
      if (LDT->segment <= 0)
	exit_with_error ("Invalid value (segment)");
    }

    if (!strcmp(argv[i],"-seg_win_threshold")) {
      LDT->seg_win_threshold = atoi(argv[++i]);
      if (LDT->seg_win_threshold <= 0)
	exit_with_error ("Invalid value (seg_win_threshold)");
    }

    if (!strcmp(argv[i],"-seg_epsilon")) {
      LDT->seg_epsilon = atof(argv[++i]);
      if (LDT->seg_epsilon < 0)
	exit_with_error ("Invalid value (seg_epsilon)");
    }

    if (!strcmp(argv[i],"-delta_c_min")) {
      LDT->delta_c_min = atof(argv[++i]);
      if (LDT->delta_c_min < 0)
	exit_with_error ("Invalid value (delta_c_min)");
    }
  }
}

/* validation of ldetect parameters */
void valid_ldetect_parameters(struct_algo_param *P_ALGO, struct_algo *ALGO,
			      struct_ldetect *LDT)
{
    if (P_ALGO->relative_eps) exit_with_error
	("relative_epsilon not supported with line detection.");
    if (LDT->segment && LDT->segment%2 == 0) exit_with_error
	("segment must be odd number");
    if (LDT->seg_win_threshold == 0)
      LDT->seg_win_threshold = LDT->segment/2;

    /* -syst_in & -syst_out */
    if (P_ALGO->syst_in_file && LDT->image_file_name[0]!='\0')
      exit_with_error("incompatible parameters (-syst_in & -in).");
    if ((P_ALGO->syst_in_file ||
	 P_ALGO->syst_out_file_name[0]!='\0') && LDT->cluster)
      exit_with_error("incompatible parameters (-syst_in/-syst_out & -cluster)");
    if (P_ALGO->syst_in_file) {
      if (P_ALGO->graph)
	exit_with_error("incompatible parameters (-graph & -syst_in)");
      if (LDT->bmp_out_flag)
	exit_with_error("incompatible parameters (-bmp_out & syst_in)");
      if (LDT->noise)
	exit_with_error("incompatible parameters (-noise & syst_in)");
      if (LDT->image.dim.x==0 || LDT->image.dim.y==0)
	exit_with_error("size of image is not specified (-dim_x & -dim_y)");
    }
}

/***********************************************************************/
/* scan command line for conic detection parameters                    */
/***********************************************************************/
void read_command_line_cdetect(int argc,char * argv[],struct_cdetect *CDT)
{
  /* default parameters */
  strcpy(CDT->image_file_name,"in");
  CDT->bmp_flag = 0;
  CDT->bmp_out_flag = 0;
  CDT->cluster = 0;
  CDT->noise = 0;
  CDT->rnd_origin = 0;
  CDT->tan_estimation = 0;
  CDT->image.background_color = 0;
  CDT->postprocess_mode = 0;
  CDT->remove_bad_pts = 0;
  CDT->initial_r = 0;
  
  for (int i=1 ; i<argc ; i++) {
    /* file -in */
    if (!strcmp(argv[i],"-in"))
      strcpy(CDT->image_file_name,argv[++i]);
    
    if (!strcmp(argv[i],"-cluster")) {
      CDT->cluster = atoi(argv[++i]);
      if (CDT->cluster < 0)
	exit_with_error ("Invalid value (cluster)");
    }

    if (!strcmp(argv[i],"-noise")) {
      CDT->noise = atof(argv[++i]);
      if (CDT->noise < 0)
	exit_with_error ("Invalid value (noise)");
    }

    if (!strcmp(argv[i],"-initial_r")) {
      CDT->initial_r = atoi(argv[++i]);
      if (CDT->initial_r < 0)
	exit_with_error ("Invalid value (initial_r)");
    }

    if (!strcmp(argv[i],"-tan_estimation")) {
      CDT->tan_estimation = atoi(argv[++i]);
      if (CDT->tan_estimation < 0)
	exit_with_error ("Invalid value (tan_estimation)");
    }

    if (!strcmp(argv[i],"-remove_bad_pts")) CDT->remove_bad_pts=1;

    if (!strcmp(argv[i],"-rnd_origin")) CDT->rnd_origin=1;

    if (!strcmp(argv[i],"-bmp")) CDT->bmp_flag=1;
    if (!strcmp(argv[i],"-bmp_out")) CDT->bmp_out_flag=1;

    if (!strcmp(argv[i],"-postprocess_mode")) {
      CDT->postprocess_mode = atoi(argv[++i]);
      if (CDT->postprocess_mode<0 || CDT->postprocess_mode>2)
	exit_with_error("Invalid value (postprocess_mode)");
    }

    if (!strcmp(argv[i],"-back_color")) {
      CDT->image.background_color = atoi(argv[++i]);
      if (CDT->image.background_color < 0)
	exit_with_error ("Invalid value (back_color)");
    }

  }
}

/* validation of cdetect parameters */
void valid_cdetect_parameters(struct_algo_param *P_ALGO, struct_algo *ALGO,
			      struct_cdetect *CDT)
{
  if (CDT->remove_bad_pts && !CDT->tan_estimation)
    exit_with_error("LMS tan estimation is necessary for option -remove_bad_pts");
  if (CDT->conic_type==ELLIPSE && CDT->tan_estimation==0)
    exit_with_error("-tan_estimation is missing");
  if (CDT->conic_type==GENERAL && CDT->postprocess_mode)
    exit_with_error("Enhanced postprocessing is implemented only for circle % ellipse detection");

  /* -syst_in & -syst_out */
  if (P_ALGO->syst_in_file)
    exit_with_error("-syst_in not implemented with conic_detection");
}

