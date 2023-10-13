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

#include "line_detect_tk.h"

#include "inline_functions.cc"

/* temp */
#include "utils.h"

/****************************************************************************/
/* Initialisation of solution for line detection	       		    */
/****************************************************************************/
void solution_initialisation_LDT(struct_ldetect *LDT, struct_algo *ALGO,
				 float *x)
{
  /* restart with last found solution */
  if (LDT->last_as_start == 1 && ALGO->step) {
    x[0] = LDT->solutions[ALGO->step-1][0];
    x[1] = LDT->solutions[ALGO->step-1][1];
    x[2] = LDT->solutions[ALGO->step-1][2];	
  }
  else {
    /* random starting point */
    int i = ALGO->equations[int(RAND*ALGO->nb_couples)];
    float x1 = LDT->SYST->A[i][0];
    float y1 = LDT->SYST->A[i][1];
    i = ALGO->equations[int(RAND*ALGO->nb_couples)];
    float x2 = LDT->SYST->A[i][0];
    float y2 = LDT->SYST->A[i][1];
    x[0] = (y2-y1)/(x2*y1-x1*y2);
    x[1] = (-1-x1*x[0])/y1;
    x[2] = 1;

    float norm_abc;
    norm_abc = sqrt(x[0]*x[0]+x[1]*x[1]);
    x[0] /= norm_abc;
    x[1] /= norm_abc;
    x[2] /= norm_abc;
  }

  /* last search was a failure */
  if (ALGO->retry_flag != 0) {
    if (LDT->nb_retry[ALGO->step]==1)
      printf("Retry #%3d", LDT->nb_retry[ALGO->step]);
    else
      printf("\b\b\b%3d", LDT->nb_retry[ALGO->step]);
    fflush(stdout);
  }

  vector_copy(LDT->last_cycle_starting_point, x, LDT->SYST->nc);
}

/****************************************************************************/
/* Line detection	       					            */
/****************************************************************************/
void search_biggest_subsystem_LDT(struct_ldetect *LDT,
				  struct_algo_param *P_ALGO,
				  struct_algo *ALGO){

    float dist; /* distance between current point and eq hyperplane */
    int i,j,k,iteration;
    int coord_x, coord_y;
    float t0,ti=0; /* Initial, decrease and current temp */
    float avg;
    int non_choisies = 0;
    static float x[3]; /* Vectors of current subsystem solution */
    int *ligne;
    int equation_index;
    float x0[3] = {0,0,0};

    /* the following variables are defined to simplify expressions */
    float **A = LDT->SYST->A;
    int nc = LDT->SYST->nc;
    int *eq = ALGO->equations;
    /* ****** */

    ALGO->last_subsys_size_threshold = 0; /* not implemented with lines */

    /* solution monitoring in reciprocal space (-mon_ab option)*/
    FILE *mon_ab;
    if (P_ALGO->mon_ab) {
      char mon_ab_name[100];
      sprintf(mon_ab_name,"MONITOR/mon_ab_%d_%d",
	      ALGO->step+1,LDT->nb_retry[ALGO->step]);
      mon_ab = fopen(mon_ab_name,"w");
    }

    ti = P_ALGO->T;

    /* Initialisation of the starting point */
    solution_initialisation_LDT(LDT, ALGO, x);
    if (P_ALGO->graph) line_detect_tk_put_start(LDT);

    ALGO->step++;

    /* In the beginning every index points to the equation itself */
    ligne= (int *) vector_alloc(ALGO->nb_couples, sizeof(int));
    for (i = 0; i < ALGO->nb_couples; i++) ligne[i] = i;

    /* set initial temp if auto_temp_set is enable */
    if (P_ALGO->auto_temp_set) {
      calculate_v(&avg, x, LDT->SYST, ALGO);
      t0 = P_ALGO->tzero * avg / norm_euclid(x[0], x[1]);
//      printf("Initial temperature: %f\n",t0);
    }
    else {
      calculate_v(&avg, x, LDT->SYST, ALGO);
      avg /= norm_euclid(x[0], x[1]);
      t0=P_ALGO->T;
    }

  /* Here begins the outer loop, counting the number of cycles, i.e. the    */
  /* number of times, all equations of the equation system have been chosen */
  /* to adapt the solution.                                                 */

    ALGO->nb_cycles = P_ALGO->nb0_cycles;
    for (ALGO->index_cycle = 0;
	 ALGO->index_cycle < ALGO->nb_cycles;
	 ALGO->index_cycle++) {

      /* calculate current temperature */
      if (P_ALGO->auto_temp_set) {
	calculate_v(&avg, x, LDT->SYST, ALGO);
	avg /= norm_euclid(x[0], x[1]);
	ti = update_temp(&t0, ti, avg, P_ALGO, ALGO);
      }
      else {
	float aux = (float)ALGO->index_cycle / (float)ALGO->nb_cycles;
	ti = (P_ALGO->T * ((1-P_ALGO->T_param)*exp(-40*aux/(2-aux))
			   + P_ALGO->T_param*exp(-5*aux)));
      }

      /* temp monitoring */
      if (P_ALGO->mon_t) fprintf(P_ALGO->mon_t_file,"%g %g\n",avg,ti);

      /* stop algorithm if avg/ti is upper than stop_temp */
//      if (avg/ti > P_ALGO->stop_temp){
//	ALGO->last_subsystem_nb_cycles = ALGO->index_cycle;
//	ALGO->index_cycle = ALGO->nb_cycles;
//      }

      /* stop algorithm if no correction in the last cycle */
//      if (ABS(x0[2]-x[2]) < 0.001 || ABS(x0[1]*x[1] + x0[0]*x[0]) > 0.99998) {
      if (ABS(x0[2]-x[2]) < LDT->delta_c_min) {
	ALGO->last_subsystem_nb_cycles = ALGO->index_cycle;
	ALGO->index_cycle = ALGO->nb_cycles;
      }
//printf("===> %f\n",ABS(x0[1]*x[1] + x0[0]*x[0]));
      x0[0] = x[0];
      x0[1] = x[1];
      x0[2] = x[2];

      /* enable conditionned correction if avg/ti is upper than no_corr */
      if (ti < P_ALGO->no_corrections) P_ALGO->no_corrections = 0;

/************************************************************************/
/* Here begins the inner loop, each couple of equations of the equation */
/* system is chosen in random order.                                    */

/* Change origin randomly */
      float delta_x = LDT->image.dim.x*(RAND-.5);
      float delta_y = LDT->image.dim.y*(RAND-.5);
/**************************/

      for (iteration = 0; iteration < (int)(ALGO->nb_couples*P_ALGO->partial)
							; iteration++){

	/* choose one couple of equations */
	equation_index = choose_line(ligne, &non_choisies, ALGO->nb_couples);
	i = eq[equation_index]; /* indirect access to equations */
 	/* calculate distance between current point and equation hyperplane */
	if (LDT->tan_estimation == 0) {  /* ****** AMS relaxation algorithm */
/* origin */
A[i][0] += delta_x;
A[i][1] += delta_y;
x[2] -= x[0]*delta_x + x[1]*delta_y;
/* ****** */
	  if ((A[i][0]!=0) || (A[i][1]!=0)) {
	    float prod_scal = A[i][0]*x[0] + A[i][1]*x[1] + A[i][2]*x[2];
	    dist = prod_scal / sqrt(x[0]*x[0] + x[1]*x[1]);
	    /* apply correction to current solutions */
//	    if (P_ALGO->auto_temp_set) {
	      /* calculate weights */
	      if (ABS(dist/ti) < P_ALGO->stop_temp) {
		float weight = ti/t0 * ABS(prod_scal)
		  / (A[i][0]*A[i][0] + A[i][1]*A[i][1] + 1)
		  * EXP_TAB(-ABS(dist)/ti);
		if (P_ALGO->no_corrections == 0 || ABS(dist) > P_ALGO->epsilon) {
		  if (dist < 0) {
		    x[0] += A[i][0] * weight;
		    x[1] += A[i][1] * weight;
		    x[2] += A[i][2] * weight;
		  }
		  else {
		    x[0] -= A[i][0] * weight;
		    x[1] -= A[i][1] * weight;
		    x[2] -= A[i][2] * weight;
		  }
/* *** plan */
float norm_abc;
//norm_abc = sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
norm_abc = sqrt(x[0]*x[0]+x[1]*x[1]);
x[0] /= norm_abc;
x[1] /= norm_abc;
x[2] /= norm_abc;
		  ALGO->nb_of_correction++;
		}
	      }
/* origin */
A[i][0] -= delta_x;
A[i][1] -= delta_y;
x[2] += x[0]*delta_x + x[1]*delta_y;
/* ****** */
	  }
	}
	else {  /* ****** Local LMS tangent estimation */
	  float current_pt[2];
	  current_pt[0] = (int)(LDT->SYST->A[i][0]-0.5+(float)LDT->image.dim.x/2);
	  current_pt[1] = (int)(LDT->SYST->A[i][1]-0.5+(float)LDT->image.dim.y/2);
	  if (find_tangente_of_edge(LDT->image, (float*)&current_pt,
				    LDT->tan_estimation,
				    LDT->remove_bad_pts, &(LDT->u))
	      > CORRELATION_THRESHOLD) {
	    if ((A[i][0]!=0) || (A[i][1]!=0)) {
	      float prod_scal = A[i][0]*x[0] + A[i][1]*x[1] + A[i][2]*x[2];
	      dist = prod_scal / sqrt(x[0]*x[0] + x[1]*x[1]);
	      /* apply correction to current solutions */
	      if (P_ALGO->auto_temp_set) {
		if (ABS(dist/ti) < P_ALGO->stop_temp) {
		  if (P_ALGO->no_corrections == 0 || ABS(dist) > P_ALGO->epsilon) {
		    if (float aux = LDT->u.x*A[i][1] - LDT->u.y*A[i][0]) {
		      x[0] += ti/t0 * exp(-ABS(dist/2)/ti)*(LDT->u.y/aux - x[0]);
		      x[1] += ti/t0 * exp(-ABS(dist/2)/ti)*(-LDT->u.x/aux - x[1]);
		    }
		  }
		}
	      }
	      else exit_with_error ("Manual temp not implemented");
	    }
	  }
	  else {
        /* the selected point is isolate and tan. can't be estimated or the */
        /* correlation coefficient is |r|<CORRELATION_THRESHOLD & the       */
        /* option -remove_bad_pts is enable. In both cases the pt is remove */
        /* from eq[]							    */

	    /* flag rejected pt in canvas */
//	    if (P_ALGO->graph && P_ALGO->mon_well_classified)
//	      conic_detect_tk_flag_rejected_point(CDT,i1);
	    /* remove point in eq list */
	    ALGO->equations[equation_index] = ALGO->equations[--(ALGO->nb_couples)];
	    iteration--;
	  }
	}

	/* solution monitoring */
	if (P_ALGO->mon_ab) fprintf(mon_ab,"%8.3e %8.3e %8.3e %3.1f\n",x[0],x[1],x[2],ti);

	/* update canvas if -graph enable */
	if (P_ALGO->graph && P_ALGO->mon==-1
	    && iteration%MONITORING_STEP==0) {
	  printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	  printf("%5d ti=%5.2f",ALGO->index_cycle,ti);
	  fflush(stdout);
	  line_detect_tk_update_mon_canvas(LDT, x);
	}
	
      }

/* End of inner loop							*/
/************************************************************************/

    }

    /* add current solution to the matrix of solutions */
    for (i=0; i < nc; i++)
      LDT->solutions[ALGO->step-1][i] = x[i];

    /* update the equation list eq[] */
    ALGO->last_subsys_size = 0;
    float norm = norm_euclid(x[0], x[1]);
    for (i = 0; i < ALGO->nb_couples; i++) {
      if (ABS(qprod_scal(x, A[eq[i]], nc) / norm) <= P_ALGO->epsilon)
	ALGO->last_subsys_size++;
    }
    if (ALGO->last_subsys_size >= P_ALGO->stop_cycles)
      line_detect_update_eq(P_ALGO, ALGO, LDT);

    free(ligne);

    /*remove starting point in TK window */
    if (P_ALGO->graph) line_detect_tk_remove_start(LDT);

    /* close monitoring files */
    if (P_ALGO->mon_ab) fclose(mon_ab);
}
