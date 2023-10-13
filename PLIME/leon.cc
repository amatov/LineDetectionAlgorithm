#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"
#include "utils.h"
#include "math_functions.h"
#include "command_line.h"
#include "data_output.h"
#include "read_data.h"
#include "create_equations.h"
#include "search_biggest_subsystem_AR_perceptron.h"
#include "search_biggest_subsystem_AR_winnow.h"
#include "search_biggest_subsystem_OPT.h"
#include "search_biggest_subsystem_LDT.h"
#include "search_biggest_subsystem_CDT.h"
#include "search_biggest_subsystem_CDT2.h"
#include "search_biggest_subsystem_CDT3.h"

#include "line_detect_tk.h"
#include "conic_detect_tk.h"

/* temp */
#include "memory_alloc.h"
#include "bmp_8b.h"
#include <math.h>
float *_TAB_EXP;

/* ************************************************************************** */
int main (int argc, char *argv[])
{
  int data_type;
  struct_algo ALGO;
  struct_algo_param P_ALGO;
  P_ALGO.BENCH=(struct_bench *) malloc(sizeof(struct_bench));

  struct_ARmodel *AR;
  struct_optical *OPT;
  struct_ldetect *LDT;
  struct_cdetect *CDT;

/* ********** Read data and create the equation system ********************** */

  /* Read the config file if exist */
  argv= update_command_line(&argc, argv);
  data_type = read_command_line_data_type(argc, argv);

  /* Read command line parameters witch are not depending on data type */
  read_command_line_common (argc, argv, &P_ALGO);

  /* initialisation of the random number serie */
  if (P_ALGO.rnd) {
    unsigned int time_int = (unsigned int) time(0);
    srand(time_int);
    time_int = rand()*32765 + rand();
    srand(time_int);
  }

  /* initialisation of the exponential array */
  init_exp_array(&_TAB_EXP);

  switch (data_type) {

  case 1: /* ***** AR model */

    AR=(struct_ARmodel *) malloc(sizeof(struct_ARmodel));
    AR->SYST=(struct_eq_system *) malloc(sizeof(struct_eq_system));

    /* set defaults and read command line for specific paramaters */
    read_command_line_AR (argc, argv, AR);
    valid_AR_parameters(&P_ALGO, &ALGO, AR);

    /* read AR data  */
    read_AR_data (AR);

    /* create equation system */
    create_AR_equations (AR);
    create_eq_syst_files(AR->SYST, &P_ALGO);

    /* initialisation & memory allocation */
    init_ar (&P_ALGO, &ALGO, AR);
    /* print parameters on screen & result_file */
    print_AR_param (&P_ALGO, &ALGO, AR);
    
    break;

  case 2: /* ***** optical flow */

    OPT=(struct_optical *) malloc(sizeof(struct_optical));
    OPT->SYST=(struct_eq_system *) malloc(sizeof(struct_eq_system));

    /* set defaults and read command line for specific paramaters */
    read_command_line_opt (argc, argv, OPT);
    valid_opt_parameters(&P_ALGO, &ALGO, OPT);

    /* read optical data */
    read_optical_data (OPT);

    /* create equation system */
    create_opt_equations (OPT);
    create_eq_syst_files(OPT->SYST, &P_ALGO);

    /* initialisation & memory allocation */
    init_optical (&P_ALGO, &ALGO, OPT);

    /* print parameters on screen & result_file */
    print_optical_param (&P_ALGO, &ALGO, OPT);

    break;

  case 4: /* ***** line detection */

    LDT=(struct_ldetect *) malloc(sizeof(struct_ldetect));
    LDT->SYST=(struct_eq_system *) malloc(sizeof(struct_eq_system));

    /* set defaults and read command line for specific paramaters */
    read_command_line_ldetect(argc, argv, LDT);
    valid_ldetect_parameters(&P_ALGO, &ALGO, LDT);

    /* read data  */
    if (!P_ALGO.syst_in_file) read_ldetect_data (LDT);

    /* init TclTk if -graph enable */
    if (P_ALGO.graph) {
      LDT->TK = (struct_tk*)malloc(sizeof(struct_tk));
      LDT->TK->interp = init_TclTk(argv[0]);
      line_detect_tk_init_window(LDT);
    }

    /* create equation system */
    create_ldetect_equations (LDT, P_ALGO.syst_in_file_name);
    create_eq_syst_files(LDT->SYST, &P_ALGO);

    /* initialisation & memory allocation */
    init_ldetect (&P_ALGO, &ALGO, LDT);

    /* print parameters on screen & result_file */
    print_ldetect_param (&P_ALGO, &ALGO, LDT);

    break;

  case 5: /* ***** conic detection */

    CDT=(struct_cdetect *) malloc(sizeof(struct_cdetect));
    CDT->SYST=(struct_eq_system *) malloc(sizeof(struct_eq_system));

    /* set defaults and read command line for specific paramaters */
    read_command_line_cdetect (argc, argv, CDT);
    valid_cdetect_parameters(&P_ALGO, &ALGO, CDT);

    /* read data  */
    read_cdetect_data (CDT);

    /* init TclTk if -graph enable */
    if (P_ALGO.graph) {
      CDT->TK = (struct_tk*)malloc(sizeof(struct_tk));
      CDT->TK->interp = init_TclTk(argv[0]);
      conic_detect_tk_init_window(CDT);
    }

    /* create equation system */
    create_cdetect_equations (CDT);
    create_eq_syst_files(CDT->SYST, &P_ALGO);

    /* initialisation & memory allocation */
    init_cdetect (&P_ALGO, &ALGO, CDT);

    /* print parameters on screen & result_file */
    print_cdetect_param (&P_ALGO, &ALGO, CDT);
    break;

  case 6: /* ***** conic2 detection */
  case 7: /* ***** conic3 detection */

    CDT=(struct_cdetect *) malloc(sizeof(struct_cdetect));
    CDT->SYST=(struct_eq_system *) malloc(sizeof(struct_eq_system));
    CDT->conic_type = data_type-4;

    /* set defaults and read command line for specific paramaters */
    read_command_line_cdetect (argc, argv, CDT);
    valid_cdetect_parameters(&P_ALGO, &ALGO, CDT);

    /* read data  */
    read_cdetect_data (CDT);

    /* init TclTk if -graph enable */
    if (P_ALGO.graph) {
      CDT->TK = (struct_tk*)malloc(sizeof(struct_tk));
      CDT->TK->interp = init_TclTk(argv[0]);
      conic_detect_tk_init_window(CDT);
    }

    /* create equation system */
    create_c23detect_equations (CDT);
    create_eq_syst_files(CDT->SYST, &P_ALGO);

    /* initialisation & memory allocation */
    init_cdetect (&P_ALGO, &ALGO, CDT);

    /* print parameters on screen & result_file */
    print_cdetect_param (&P_ALGO, &ALGO, CDT);

    break;
  }

  /* temp monitoring (-mon_t option) */
  if (P_ALGO.mon_t) P_ALGO.mon_t_file = fopen_w("mon_t.txt");

/* ********** extract the compatible sub-systems *************************** */

  int continue_flag = 1;
  ALGO.retry_flag = 0;
  int retry;
  P_ALGO.BENCH->time = get_process_time();
  P_ALGO.BENCH->computation_time = 0;

  switch(data_type) {
  case 1: /* ********** AR data modeling */

    while ((ALGO.nb_couples > 0) && (continue_flag == 1)){
      ALGO.num_extr_subsys++;
      if (P_ALGO.winnow) /* WINNOW */
	search_biggest_subsystem_AR_winnow(AR, &P_ALGO, &ALGO);
      else /* PERCEPTRON */
	search_biggest_subsystem_AR_perceptron(AR, &P_ALGO, &ALGO);
      AR->sizes[ALGO.step-1] = ALGO.last_subsys_size;

      /* print AR subsyst results on screen */
      if (ALGO.last_subsys_size) print_AR_subsyst_results (AR, &ALGO, AR->SYST);
      if (ALGO.last_subsys_size < P_ALGO.stop_cycles) continue_flag = 0;
      if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys) continue_flag = 0;
    }
    break;

  case 2: /* ********** Motion estimation */

    while ((ALGO.nb_couples > 0) && (continue_flag == 1)){
      ALGO.num_extr_subsys++;
      search_biggest_subsystem_OPT(OPT, &P_ALGO, &ALGO); /* *** */
      matrix_to_screen (OPT->out, OPT->dim_y, OPT->dim_x);
      display_last_subsyst_size (&ALGO);
      OPT->sizes[ALGO.step-1] = ALGO.last_subsys_size;
      if (ALGO.last_subsys_size < P_ALGO.stop_cycles) continue_flag = 0;
      if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys ) continue_flag = 0;
    }
    break;

  case 4: /* ********** Line detection */

    LDT->current_cluster = 0;
    do {
      if (LDT->cluster) {
	LDT->SYST = LDT->SYST_CLUSTERS[LDT->current_cluster];
	ALGO.nb0_couples = ALGO.nb_couples = LDT->SYST->nr;
	if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys ) continue_flag = 0;
	printf("\nCluster #%d (Nb_pts:%d)\n",LDT->current_cluster+1,
	       ALGO.nb_couples);
      }
      for (int i=0; i<ALGO.nb_couples; i++) ALGO.equations[i]=i;

      retry = 0;
      ALGO.retry_flag = 0;

      while ((ALGO.nb_couples >= P_ALGO.stop_cycles) && (continue_flag == 1)){
	ALGO.num_extr_subsys++;

	search_biggest_subsystem_LDT(LDT, &P_ALGO, &ALGO); /* *** */
	/* update canvas if -graph option is enable */
	if (P_ALGO.graph) line_detect_tk_update_canvas(LDT, &ALGO, &P_ALGO);

	LDT->sizes[ALGO.step-1] = ALGO.last_subsys_size;
	LDT->corr_cl[ALGO.step-1] = LDT->current_cluster+1;
	if (ALGO.last_subsys_size < P_ALGO.stop_cycles)
	  if (retry++ < P_ALGO.max_retry && ALGO.nb_couples > P_ALGO.stop_cycles)
	    /* erase last found subsystem & retry */
	    {
	      ALGO.retry_flag = 1;
	      ALGO.step--;
	      ALGO.num_extr_subsys--;
	      LDT->nb_retry[ALGO.step]++;
	    }
	  else continue_flag = 0;
	else { /* last subsystem is accepted */
	  display_last_subsyst_size (&ALGO);
	  ALGO.retry_flag = 0;
	  retry = 0;
	}
	if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys ) continue_flag = 0;
      }
      LDT->current_cluster++;
      continue_flag = 1;
    }
    while (LDT->current_cluster < LDT->cluster*LDT->cluster);
    break;

  case 5: /* ********** Conic detection */

    CDT->current_cluster = 0;
      for (int i=0; i<ALGO.nb_couples; i++) ALGO.equations[i]=i;

      retry = 0;
      ALGO.retry_flag = 0;

      while ((ALGO.nb_couples >= P_ALGO.stop_cycles) && (continue_flag == 1)){
	ALGO.num_extr_subsys++;
	search_biggest_subsystem_CDT(CDT, &P_ALGO, &ALGO); /* *** */

	/* update canvas if -graph option is enable */
	if (P_ALGO.graph) conic_detect_tk_update_canvas(CDT, &ALGO, &P_ALGO);

	display_last_subsyst_size (&ALGO);
	CDT->sizes[ALGO.step-1] = ALGO.last_subsys_size;
	CDT->corr_cl[ALGO.step-1] = CDT->current_cluster+1;
	if (ALGO.last_subsys_size < P_ALGO.stop_cycles)
	  if (retry++ < P_ALGO.max_retry && ALGO.nb_couples > P_ALGO.stop_cycles)
	    /* erase last found subsystem & retry */
	    {
	      ALGO.retry_flag = 1;
	      ALGO.step--;
	      ALGO.num_extr_subsys--;
	      CDT->nb_retry[ALGO.step]++;
	    }
	  else continue_flag = 0;
	else {
	  ALGO.retry_flag = 0;
	  retry = 0;
	}
	if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys ) continue_flag = 0;
      }
      CDT->current_cluster++;
      continue_flag = 1;

    break;

  case 6: /* ********** Conic_2 detection (circle)*/
    CDT->current_cluster = 0;
    do {
      if (CDT->cluster) {
	CDT->origin.x = 0;
	CDT->origin.y = 0;
	CDT->SYST = CDT->SYST_CLUSTERS[CDT->current_cluster];
	ALGO.nb0_couples = ALGO.nb_couples = CDT->SYST->nr;
	if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys ) continue_flag = 0;
	printf("\nCluster #%d (Nb_pts:%d)\n",CDT->current_cluster+1,
	       ALGO.nb_couples);
      }
      for (int i=0; i<ALGO.nb_couples; i++) ALGO.equations[i]=i;

      retry = 0;
      ALGO.retry_flag = 0;

      while ((ALGO.nb_couples >= P_ALGO.stop_cycles) && (continue_flag == 1)){
	ALGO.num_extr_subsys++;
	search_biggest_subsystem_CDT2(CDT, &P_ALGO, &ALGO); /* *** */
	/* update canvas if -graph option is enable */
	if (P_ALGO.graph) conic2_detect_tk_update_canvas(CDT, &ALGO, &P_ALGO);

	CDT->sizes[ALGO.step-1] = ALGO.last_subsys_size;
	CDT->corr_cl[ALGO.step-1] = CDT->current_cluster+1;
	if (!ALGO.last_subsystem_accepted) {
	  /* last subsystem is rejected */
	  if (P_ALGO.graph) display_last_subsystem_info_cdt (&ALGO,CDT);
	  if (retry++ < P_ALGO.max_retry && ALGO.nb_couples > P_ALGO.stop_cycles)
	    /* retry */
	    {
	      ALGO.retry_flag = 1;
	      ALGO.step--;
	      ALGO.num_extr_subsys--;
	      CDT->nb_retry[ALGO.step]++;
	    }
	  else continue_flag = 0; /* stop */
	}
	else { /* last subsystem is accepted */
	  display_last_subsystem_info_cdt (&ALGO,CDT);
	  ALGO.retry_flag = 0;
	  retry = 0;
	}
	if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys ) continue_flag = 0;
      }
      CDT->current_cluster++;
      continue_flag = 1;
    }
    while (CDT->current_cluster < CDT->cluster*CDT->cluster);

    break;

  case 7: /* ********** Conic_3 detection (ellipse) */
    CDT->current_cluster = 0;
    do {
      if (CDT->cluster) {
	CDT->origin.x = 0;
	CDT->origin.y = 0;
	CDT->SYST = CDT->SYST_CLUSTERS[CDT->current_cluster];
	ALGO.nb0_couples = ALGO.nb_couples = CDT->SYST->nr;
	if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys ) continue_flag = 0;
	printf("\nCluster #%d (Nb_pts:%d)\n",CDT->current_cluster+1,
	       ALGO.nb_couples);
      }
      for (int i=0; i<ALGO.nb_couples; i++) ALGO.equations[i]=i;

      retry = 0;
      ALGO.retry_flag = 0;

      while ((ALGO.nb_couples >= P_ALGO.stop_cycles) && (continue_flag == 1)){
	ALGO.num_extr_subsys++;
	search_biggest_subsystem_CDT3(CDT, &P_ALGO, &ALGO); /* *** */

	/* update canvas if -graph option is enable */
	if (P_ALGO.graph) conic3_detect_tk_update_canvas(CDT, &ALGO, &P_ALGO);

	CDT->sizes[ALGO.step-1] = ALGO.last_subsys_size;
	CDT->corr_cl[ALGO.step-1] = CDT->current_cluster+1;
	if (!ALGO.last_subsystem_accepted) {
	  if (P_ALGO.graph) display_last_subsystem_info_cdt (&ALGO,CDT);
	  if (retry++ < P_ALGO.max_retry && ALGO.nb_couples > P_ALGO.stop_cycles)
	    /* last subsystem rejected & retry */
	    {
	      ALGO.retry_flag = 1;
	      ALGO.step--;
	      ALGO.num_extr_subsys--;
	      CDT->nb_retry[ALGO.step]++;
	    }
	  else {
	    continue_flag = 0; /* last subsys rejected & research aborted */
	    ALGO.step--;
	    ALGO.num_extr_subsys--;
	  }
	}
	else { /* last subsystem is accepted */
	  display_last_subsystem_info_cdt (&ALGO,CDT);
	  ALGO.retry_flag = 0;
	  retry = 0;
	}
	if (ALGO.num_extr_subsys > P_ALGO.max_num_subsys ) continue_flag = 0;
      }
      CDT->current_cluster++;
      continue_flag = 1;
    }
    while (CDT->current_cluster < CDT->cluster*CDT->cluster);

    break;

  }


  P_ALGO.BENCH->time = get_process_time() - P_ALGO.BENCH->time;
  P_ALGO.BENCH->postprocessing_time =
    P_ALGO.BENCH->time - P_ALGO.BENCH->computation_time;


/* ********** Final results ouput ******************************************* */

  switch (data_type) {
  case 1: /* ********** AR model */
    /* print AR error values on screen */
    AR->total_av_abs_err /= ALGO.nb0_couples - ALGO.nb_couples;
    AR->total_av_quadr_err /= ALGO.nb0_couples - ALGO.nb_couples;
    print_AR_error(AR);

    print_AR_final_results(ALGO.num_extr_subsys, AR);

    printf("\nCalculation time: %4.2fs\n",P_ALGO.BENCH->time);
    printf("Remaining nb of eq: %d (%2.1f%% classified)\n\n",ALGO.nb_couples,
	   (1-(float)ALGO.nb_couples/(float)ALGO.nb0_couples)*100);

    /* make both out_file & mapping_file */
    if (AR->out_name[0]!='\0') write_matlab_AR_file(&P_ALGO, &ALGO, AR);

    /* add info to result file */
    if (P_ALGO.BENCH->result_file_name[0]) {
      FILE *result_file = fopen_a(P_ALGO.BENCH->result_file_name);
      fprintf(result_file,"%d %f %f\n",
	      P_ALGO.nb0_cycles,AR->total_av_quadr_err,
	      P_ALGO.BENCH->time);
      fclose(result_file);
    }

    break;

  case 2: /* ********** Optical flow */
    printf("\n\n");
    for (int i=0; i < ALGO.num_extr_subsys; i++)
      printf("%+12.8e %+12.8e %+12.8e   size-> %3d\n",
	     OPT->solutions[i][0], OPT->solutions[i][1], OPT->solutions[i][2],
	     OPT->sizes[i]);
    printf("Calculation time: %4.2fs\n",P_ALGO.BENCH->time);
    break;

  case 4: /* ********** Line detection */

    /* update graph with non classified points (crosses) */
    if (P_ALGO.graph && P_ALGO.mon_well_classified)
      line_detect_tk_flag_non_classified(LDT, &ALGO);

    printf("\n\n");
    for (int i=0; i < ALGO.num_extr_subsys; i++)
      printf("%2d | %4d #%-2d C%d | %+f %+f %+f sq:%4.1f m:%4.1f\n", i+1,
	     LDT->sizes[i], LDT->nb_retry[i], LDT->corr_cl[i],
	     LDT->solutions[i][0], LDT->solutions[i][1], LDT->solutions[i][2],
	     LDT->sq_error[i]/LDT->sizes[i], LDT->mean[i]/LDT->sizes[i]);

    /* create output bmp file */
    if (LDT->bmp_flag && LDT->bmp_out_flag)
      create_ldetect_output_bmp(LDT, &ALGO, &P_ALGO);

    printf("Calculation time: %4.2fs\n",P_ALGO.BENCH->time);
    printf("Total nb. of correction: %1.2e\n",(float)ALGO.nb_of_correction);
    if (P_ALGO.BENCH->nb_subsys) {
      calculate_ldetect_bench_mark(LDT, &P_ALGO, &ALGO);
      printf("Bench mark /%-3d : %5.2f\n\n",
	   P_ALGO.BENCH->nb_subsys, P_ALGO.BENCH->result);
    }

    /* add info to bench_noise file */
//     FILE* noise_bench_file;
//     noise_bench_file = fopen_a("noise_bench.txt");
//     int index_max_size;
//     index_max_size = index_max_value(LDT->sizes, ALGO.num_extr_subsys);
// //    fprintf(noise_bench_file,"%1.2f  %1.4f %1.4f %4.3f %5d\n",
// //	P_ALGO.partial, LDT->sq_error[index_max_size]/LDT->sizes[index_max_size],
// //	LDT->mean[index_max_size]/LDT->sizes[index_max_size], P_ALGO.BENCH->time,
// //	round_to_int(P_ALGO.BENCH->result*100));
//     float pente;
//     pente = LDT->solutions[index_max_size][1]/LDT->solutions[index_max_size][0];
//     calculate_lms_param(LDT);
//     fprintf(noise_bench_file,"%f %f %5d %f %1.1f\n",
//        pente, pente*pente,
//        round_to_int(P_ALGO.BENCH->result*100), LDT->a_lms, LDT->sum_xi);
//     fclose (noise_bench_file);

    /* close Tk graphical window */
    if (P_ALGO.graph) close_TclTk(LDT->TK->interp);

    break;

  case 5: /* ********** Conic detection */

float R;
    printf("\n\n");
    for (int i=0; i < ALGO.num_extr_subsys; i++) {
      vector_to_screen(CDT->solutions[i],CDT->SYST->nc);
R = sqrt(-CDT->solutions[i][5]+CDT->solutions[i][3]*CDT->solutions[i][3]/4+CDT->solutions[i][4]*CDT->solutions[i][4]/4);
      printf("   %3d #%-2d C%d x0:%f y0:%f R:%f\n",
	     CDT->sizes[i], CDT->nb_retry[i], CDT->corr_cl[i],
	     -CDT->solutions[i][3]/2, -CDT->solutions[i][4]/2, R);
    }

    if (CDT->bmp_out_flag) create_cdetect_output_bmp(CDT, &ALGO, &P_ALGO);   
    printf("Calculation time: %4.2fs\n",P_ALGO.BENCH->time);

    /* close Tk graphical window */
    if (P_ALGO.graph) close_TclTk(CDT->TK->interp);

    break;

  case 6: /* ********** Conic_2 detection (circle) */

    /* update graph with non classified points (crosses) */
    if (P_ALGO.graph && P_ALGO.mon_well_classified)
      conic_detect_tk_flag_non_classified(CDT, &ALGO);

    printf("\n\n");
    for (int i=0; i < ALGO.num_extr_subsys; i++)
      printf("%2d | %4d #%-2d C%d | %4.0f %4.0f %4.0f\n", i+1,
	     CDT->sizes[i], CDT->nb_retry[i], CDT->corr_cl[i],
	     CDT->solutions[i][0], CDT->solutions[i][1], CDT->solutions[i][2]);
    printf("\n");

    if (CDT->bmp_out_flag) create_c23detect_output_bmp(CDT, &ALGO, &P_ALGO);
    
    display_time_informations(&P_ALGO);

    if (P_ALGO.BENCH->nb_subsys) {
      calculate_cdetect_bench_mark(CDT, &P_ALGO, &ALGO);
      printf("Bench mark /%-3d : %5.2f\n\n",
	     P_ALGO.BENCH->nb_subsys, P_ALGO.BENCH->result);
    }

    /* close Tk graphical window */
    if (P_ALGO.graph) close_TclTk(CDT->TK->interp);

    break;

  case 7: /* ********** Conic_3 detection (ellipse) */

    /* update graph with non classified points (crosses) */
    if (P_ALGO.graph && P_ALGO.mon_well_classified)
      conic_detect_tk_flag_non_classified(CDT, &ALGO);

    printf("\n\n");
    for (int i=0; i < ALGO.num_extr_subsys; i++) {
      printf("%2d | %4d #%-2d C%d | ", i+1,
	     CDT->sizes[i], CDT->nb_retry[i], CDT->corr_cl[i]);
      vector_to_screen_int(CDT->solutions[i],CDT->SYST->nc);
    }
    if (CDT->bmp_out_flag) create_c23detect_output_bmp(CDT, &ALGO, &P_ALGO);   
    printf("Calculation time: %4.2fs\n",P_ALGO.BENCH->time);

    if (P_ALGO.BENCH->nb_subsys) {
      calculate_cdetect_bench_mark(CDT, &P_ALGO, &ALGO);
      printf("Bench mark /%-3d : %5.2f\n\n",
	     P_ALGO.BENCH->nb_subsys, P_ALGO.BENCH->result);
    }

    /* *** temp */
    FILE *bench;
    if (P_ALGO.BENCH->result_file_name[0]!='\0') {
      bench = fopen_a(P_ALGO.BENCH->result_file_name);
      fprintf(bench,"%s %4.2f %5.2f %d",CDT->image_file_name,P_ALGO.BENCH->time,P_ALGO.BENCH->result,(int)P_ALGO.T);
      int aux_temp=0;
      for (int i=0; i<ALGO.num_extr_subsys; i++) {
	aux_temp += CDT->nb_retry[i];
      }
      fprintf(bench," %f %4.2f\n",(float)(aux_temp-P_ALGO.max_retry)/(float)(ALGO.num_extr_subsys-1),P_ALGO.T_param);
      fclose(bench);
    }
    /* *** temp */

    /* close Tk graphical window */
    if (P_ALGO.graph) close_TclTk(CDT->TK->interp);

    break;
  }

  /* close monitoring files */
  if (P_ALGO.mon_t) fclose(P_ALGO.mon_t_file);

  return (1);
}
