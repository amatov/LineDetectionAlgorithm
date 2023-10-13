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
/* Conic detection	       					            */
/****************************************************************************/

/****************************************************************************/
/* Initialisation of solution for conic detection	       		    */
/****************************************************************************/
void solution_initialisation_CDT(struct_cdetect *CDT, struct_algo *ALGO,
				 float *x)
{
  /* initialisation of solutions */
  if (ALGO->retry_flag != 0)
    printf("Retry #%d\n", CDT->nb_retry[ALGO->step]);
  
  x[0] = 1;
  x[1] = 0;
  x[2] = 1;
  x[3] = -256;
  x[4] = -256;
  x[5] = 32370;
  
  float norm_x = sqrt(SQR(x[0])+SQR(x[1])+SQR(x[2])
		      +SQR(x[3])+SQR(x[4])+SQR(x[5]));
  x[0] /= norm_x;
  x[1] /= norm_x;
  x[2] /= norm_x;
  x[3] /= norm_x;
  x[4] /= norm_x;
  x[5] /= norm_x;
  vector_copy(CDT->last_cycle_starting_point, x, CDT->SYST->nc);
}

/* C_GEO_DIST = sqrt((2ax+by+d)^2+(2cy+bx+e)^2) */
#define C_GEO_DIST sqrt((2*x[0]*A[i][3]+x[1]*A[i][4]+x[3])*(2*x[0]*A[i][3]+x[1]*A[i][4]+x[3])+(2*x[2]*A[i][4]+x[1]*A[i][3]+x[4])*(2*x[2]*A[i][4]+x[1]*A[i][3]+x[4]))
/* C_REC_DIST = sqrt(a^2+b^2+c^2+d^2+e^2) */
#define C_REC_DIST sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]+x[3]*x[3]+x[4]*x[4])
/* NORM_A2 = || x^2, xy, y^2, x, y, 1 ||^2 */
#define NORM_A2 (A[i][0]*A[i][0]+A[i][1]*A[i][1]+A[i][2]*A[i][2]+A[i][3]*A[i][3]+A[i][4]*A[i][4]+A[i][5]*A[i][5])

/****************************************************************************/
/* Core for conic detection	       					    */
/****************************************************************************/

void update_winnow_current_solution_conic(float *x,float t0,float ti,
					  int nc, float dist,
					  struct_eq_system *SYST,
					  struct_algo_param *P_ALGO,
					  struct_algo *ALGO,
					  int i_eq)
{
  float alpha = 1.05;
vector_to_screen(x,6);

  float norm = sqrt(SYST->A[i_eq][0]+SYST->A[i_eq][2]);
  if (dist > 0) {
    x[0] *= pow(alpha,-SYST->A[i_eq][0]/norm/norm);
    x[1] *= pow(alpha,-SYST->A[i_eq][1]/norm/norm);
    x[2] *= pow(alpha,-SYST->A[i_eq][2]/norm/norm);
    x[3] *= pow(alpha,-SYST->A[i_eq][3]/norm);
    x[4] *= pow(alpha,-SYST->A[i_eq][4]/norm);
    x[5] *= pow(alpha,-1);
  }
  else {
    x[0] *= pow(alpha,SYST->A[i_eq][0]/norm/norm);
    x[1] *= pow(alpha,SYST->A[i_eq][1]/norm/norm);
    x[2] *= pow(alpha,SYST->A[i_eq][2]/norm/norm);
    x[3] *= pow(alpha,SYST->A[i_eq][3]/norm);
    x[4] *= pow(alpha,SYST->A[i_eq][4]/norm);
    x[5] *= pow(alpha,1);
  }

  /*  float **A = SYST->A;
      int i = i_eq;
      new_dist = qprod_scal(x, A[i], 6) / C_GEO_DIST;

      if (ABS(new_dist) > ABS(dist)) {
      if (dist < 0) {
      x[0] *= pow(alpha,-SYST->A[i_eq][0]/norm/norm);
      x[1] *= pow(alpha,-SYST->A[i_eq][1]/norm/norm);
      x[2] *= pow(alpha,-SYST->A[i_eq][2]/norm/norm);
      x[3] *= pow(alpha,-SYST->A[i_eq][3]/norm);
      x[4] *= pow(alpha,-SYST->A[i_eq][4]/norm);
      x[5] *= pow(alpha,-1);
      }
      else {
      x[0] *= pow(alpha,SYST->A[i_eq][0]/norm/norm);
      x[1] *= pow(alpha,SYST->A[i_eq][1]/norm/norm);
      x[2] *= pow(alpha,SYST->A[i_eq][2]/norm/norm);
      x[3] *= pow(alpha,SYST->A[i_eq][3]/norm);
      x[4] *= pow(alpha,SYST->A[i_eq][4]/norm);
      x[5] *= pow(alpha,1);
      }
      }
  */
}

void search_biggest_subsystem_CDT(struct_cdetect *CDT,
				  struct_algo_param *P_ALGO,
				  struct_algo *ALGO){

    float geo_dist; /* geometric distance between current point and solution */
    int i,j,k,iteration;
    float t0,ti=0; /* Initial, decrease and current temp */
    float aux, prod_scal, avg;
    int new_nb_couples,non_choisies = 0;
    static float x[6]; /* Vectors of current subsystem solution */
    int *ligne;

    ALGO->nb_cycles = P_ALGO->nb0_cycles;
    ALGO->last_subsystem_nb_cycles = ALGO->nb_cycles;

    /* the following variables are defined to simplify expressions */
    float **A = CDT->SYST->A;
    int nc = CDT->SYST->nc;
    /* ****** */

    /* solution monitoring in reciprocal space (-mon_ab option) */
    FILE *mon_ab;
    if (P_ALGO->mon_ab) {
      char mon_ab_name[100];
      sprintf(mon_ab_name,"MONITOR/mon_ab_%d",ALGO->step+1);
      mon_ab = fopen(mon_ab_name,"w");
    }

    ti = P_ALGO->T;

    /* intialisation of solution */
    solution_initialisation_CDT(CDT, ALGO, x);
    if (P_ALGO->graph) conic_detect_tk_put_start(CDT);

    ALGO->step++;

    /* place origin randomly (-rnd_origin) */
    if (CDT->rnd_origin) {
      point_2d_int old_origin = CDT->origin;
      point_2d_int delta, o_cl;

      if (CDT->cluster) {
	int cc = CDT->current_cluster;
	delta.x = CDT->cluster_edge[cc][1] - CDT->cluster_edge[cc][0];
	delta.y = CDT->cluster_edge[cc][3] - CDT->cluster_edge[cc][2];
	o_cl.x = CDT->image.dim.x/2 - CDT->cluster_edge[cc][0];
	o_cl.y = CDT->image.dim.y/2 - CDT->cluster_edge[cc][2];
      }
      else {
	delta.x = CDT->image.dim.x;
	delta.y = CDT->image.dim.y;
	o_cl.x = CDT->image.dim.x / 2;
	o_cl.y = CDT->image.dim.y / 2;
      }
      CDT->origin.x = (int)floor((float)rand()/32767 * delta.x - o_cl.x);
      CDT->origin.y = (int)floor((float)rand()/32767 * delta.y - o_cl.y);
      for (int q=0; q<ALGO->nb_couples; q++) {
	A[EQ[q]][0] += CDT->origin.x - old_origin.x;
	A[EQ[q]][1] += CDT->origin.y - old_origin.y;
      }
    }

    /* In the beginning every index points to the equation itself */
    ligne= (int *) vector_alloc(ALGO->nb_couples, sizeof(int));
    for (i = 0; i < ALGO->nb_couples; i++) ligne[i] = i;

    /* set initial temp if auto_temp_set is enable */
    if (P_ALGO->auto_temp_set) {
//      calculate_v(&avg, x, CDT->SYST, ALGO);
//      t0 = P_ALGO->tzero * avg;
      
    }
    else {
      ti = t0 = P_ALGO->T;
    }

   /* Here begins the outer loop, counting the number of cycles, i.e. the   */
   /* number of times, all equations of the equation system have been chosen*/
   /* to adapt the solution.                                                */
//    ALGO->nb_cycles = P_ALGO->nb0_cycles * ALGO->nb0_couples / ALGO->nb_couples;
    for (ALGO->index_cycle = 0;
	 ALGO->index_cycle < ALGO->nb_cycles;
	 ALGO->index_cycle++) {

      /* calculate current temperature */
//      calculate_v(&avg, x, CDT->SYST, ALGO);
//      avg /= norm_euclid(x[0], x[1]);
      if (P_ALGO->auto_temp_set)
	ti = update_temp(&t0, ti, avg, P_ALGO, ALGO);
      else {
	float aux = (float)ALGO->index_cycle / (float)ALGO->nb_cycles;
	ti = (P_ALGO->T * ((1-P_ALGO->T_param)*exp(-40*aux/(2-aux))
			   + P_ALGO->T_param*exp(-5*aux)));
      }

      /* temp monitoring */
      if (P_ALGO->mon_t) fprintf(P_ALGO->mon_t_file,"%g %g\n",avg,ti);

      /* stop algorithm if avg/ti is upper than stop_temp */
//      if (avg/ti > P_ALGO->stop_temp && P_ALGO->auto_temp_set) {
//	ALGO->last_subsystem_nb_cycles = ALGO->index_cycle;
//	ALGO->index_cycle = ALGO->nb_cycles;
//      }

      /* enable conditionned correction if avg/ti is upper than no_corr */
//      if (avg/ti > P_ALGO->no_corrections) P_ALGO->no_corrections = 0;

/************************************************************************/
/* Here begins the inner loop, each couple of equations of the equation */
/* system is chosen in random order.                                    */
    
      for (iteration = 0; iteration < (int)(ALGO->nb_couples*P_ALGO->partial)
	     ; iteration++){

	/* choose one couple of equations */
	i = choose_line(ligne, &non_choisies, ALGO->nb_couples);
	i = EQ[i]; /* indirect access to equations */

 	/* calculate distances between current point and solution */
	prod_scal = qprod_scal(A[i],x,nc);
	float dist = prod_scal/C_GEO_DIST; /* geometrical distance */

	/* apply correction to current solutions */
//	if (ABS(dist/ti) < P_ALGO->stop_temp) {
	if (1) {
	  float weight = ti/t0 * ABS(prod_scal) / NORM_A2
	    * EXP_TAB(-ABS(dist)/ti);
	  if (P_ALGO->no_corrections == 0 || ABS(dist) > P_ALGO->epsilon) {
	    if (dist < 0) {
	      x[0] += A[i][0] * weight;
	      x[1] += A[i][1] * weight;
	      x[2] += A[i][2] * weight;
	      x[3] += A[i][3] * weight;
	      x[4] += A[i][4] * weight;
	      x[5] += A[i][5] * weight;
	    }
	    else {
	      x[0] -= A[i][0] * weight;
	      x[1] -= A[i][1] * weight;
	      x[2] -= A[i][2] * weight;
	      x[3] -= A[i][3] * weight;
	      x[4] -= A[i][4] * weight;
	      x[5] -= A[i][5] * weight;
	    }
	  }

	  /* projection on the hypersphere ||x||=1 */
	  float norm_x = sqrt(SQR(x[0])+SQR(x[1])+SQR(x[2])
			      +SQR(x[3])+SQR(x[4])+SQR(x[5]));
	  x[0] /= norm_x;
	  x[1] /= norm_x;
	  x[2] /= norm_x;
	  x[3] /= norm_x;
	  x[4] /= norm_x;
	  x[5] /= norm_x;

	  /* update canvas if -graph enable */
	  if (P_ALGO->graph && P_ALGO->mon==-1
	      && iteration%MONITORING_STEP==0) {
	    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	    printf("%5d ti=%5.2f",ALGO->index_cycle,ti);
	    fflush(stdout);
	    conic_detect_tk_update_mon_canvas(CDT, x);
	  }

	  /* solution monitoring */
	  if (P_ALGO->mon_ab) fprintf(mon_ab,"%8.3e %8.3e %8.3e %3.1f\n",
				      x[3],x[4],x[5],ti);
	}
      }

   /* End of inner loop							*/
   /************************************************************************/

      /* print partial results every 5 cycles */
      if (P_ALGO->mon > 1)
	if ((ALGO->index_cycle % (int)(ALGO->nb_cycles/5)) == 0) {
	  printf("ti %f ",ti);
	  vector_to_screen(x,6);
	}
      if (P_ALGO->graph && P_ALGO->mon>2)
	conic_detect_tk_update_mon_canvas(CDT, x);
    }

    /* add current solution to the matrix of solutions */
    for (i=0; i < nc; i++)
      CDT->solutions[ALGO->step-1][i] = x[i];

    /* calculate subsystem size */
    ALGO->last_subsys_size = 0;
    for (i = 0; i < ALGO->nb_couples; i++) {
      if (ABS(qprod_scal(x, A[EQ[i]], nc)/C_GEO_DIST) < P_ALGO->epsilon)
	ALGO->last_subsys_size++;
    }

    /* update the equation list eq[] */
    if (ALGO->last_subsys_size >= P_ALGO->stop_cycles) {
      /* last found subsystem is accepted (by first postprocessing) */
      cdetect_update_eq(P_ALGO, ALGO, CDT, P_ALGO->epsilon);
    }

    vector_to_screen(x,CDT->SYST->nc);

    free(ligne);

    /*remove starting point in TK window */
    if (P_ALGO->graph) conic_detect_tk_remove_start(CDT);

    /* close monitoring files */
    if (P_ALGO->mon_ab) fclose(mon_ab);
}

#undef C_GEO_DIST
#undef C_REC_DIST
#undef NORM_A2

#undef EQ
