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

#include "math_functions.h"
#include "struct.h"
#include "memory_alloc.h"
#include "data_output.h"

#include "inline_functions.cc"

/* temp */
#include "utils.h"

/****************************************************************************/
/* Optical flow							            */
/****************************************************************************/
void search_biggest_subsystem_OPT(struct_optical *OPT,
				  struct_algo_param *P_ALGO,
				  struct_algo *ALGO){

    float dist; /* distances between current point and eq hyperplane  */
    int i,j,k,iteration,index;
    float t0,ti=0;     /* Initial, decrease and current temp */
    float avg;
    int new_nb_couples,non_choisies = 0;
    float *x1;	       /* Vector of current subsystem solution */
    int *ligne;

    /* the following variables are defined to simplify expressions */
    float **A = OPT->SYST->A;
    int nc = OPT->SYST->nc;
    float  *b = OPT->SYST->b;
    int   *eq = ALGO->equations;
    /* ****** */
    ALGO->step++;
    ti = P_ALGO->T;
    /* initialisation of solutions for optical flow model */
    x1=(float *) vector_alloc(nc, sizeof(float));
    for (j=0; j<nc;j++) x1[j]=0.1;

    /* array ligne for indirect access to equations, which is necessary to   */
    /* increase performance. In the beginning every index points to the      */
    /* equation itself.                                                      */

    ligne= (int *) vector_alloc(ALGO->nb_couples, sizeof(int));
    for (i = 0; i < ALGO->nb_couples; i++) ligne[i] = i;

    /* set initial temp if auto_temp_set is enable */
    if (P_ALGO->auto_temp_set) {
      calculate_v(&avg, x1, OPT->SYST, ALGO);
      avg /= sqrt(1 + x1[1]*x1[1] + x1[2]*x1[2]);
      t0 = P_ALGO->tzero * avg;
      printf("Initial temperature: %f\n",t0);
    }
    /* Here begins the outer loop, counting the number of cycles, i.e. the   */
    /* number of times, all equations of the equation system have been chosen*/
    /* to adapt the solution.                                                */

//    ALGO->nb_cycles = P_ALGO->nb0_cycles * ALGO->nb0_couples / ALGO->nb_couples;
    ALGO->nb_cycles = P_ALGO->nb0_cycles;
    for (ALGO->index_cycle = 0;
	 ALGO->index_cycle < ALGO->nb_cycles;
	 ALGO->index_cycle++) {

      /* calculate current temperature */
      calculate_v(&avg, x1, OPT->SYST, ALGO);
      avg /= sqrt(1 + x1[1]*x1[1] + x1[2]*x1[2]);
      ti = update_temp(&t0, ti, avg, P_ALGO, ALGO);

      /* stop algorithm if avg/ti is upper than stop_temp */
      if (avg/ti > P_ALGO->stop_temp){
	printf("\nStopped at cycle %d %d\n", ALGO->index_cycle, ALGO->nb_cycles);
	ALGO->index_cycle = ALGO->nb_cycles;
      }

      /* Here begins the inner loop, each couple of equations of the         */
      /* equation system is chosen exactly once, but in random order.        */
    
      for (iteration = 0; iteration < ALGO->nb_couples*P_ALGO->partial; iteration++){
   
	/* choose one couple of equations */
	i = choose_line(ligne, &non_choisies, ALGO->nb_couples);
	i = eq[i]; /* indirect access to equations */
	/* calculate distance between current point and equation hyperplane  */
	dist = (qprod_scal(A[i],x1,nc) - b[i]);
//	  / sqrt(1 + x1[1]*x1[1] + x1[2]*x1[2]);

	ALGO->weight[0] = ABS(qprod_scal(A[i],x1,nc) - b[i])
				/ (A[i][1]*A[i][1] + A[i][2]*A[i][2] + 1);
	ALGO->weight[2] = ALGO->weight[1] = ALGO->weight[0];

	/* apply correction to current solutions */
	if (P_ALGO->no_corrections == 0 || ABS(dist) > P_ALGO->epsilon) {
	  update_current_solution(x1, t0, ti, nc, dist, OPT->SYST,
				  P_ALGO, ALGO, i);
	}
//printf("%f",dist);
//printf(" / %f / ",ti);
      }

      /* end of inner loop */

printf("%d %f  ",ALGO->index_cycle,ti);
vector_to_screen(x1,nc);

      /* temp ***********/
      if (P_ALGO->mon > 1) {
	if ((ALGO->index_cycle % (int)(ALGO->nb_cycles/5)) == 0){
	  if (!P_ALGO->auto_temp_set) {
	    calculate_v(&avg, x1, OPT->SYST, ALGO);
	    avg /= sqrt(1 + x1[1]*x1[1] + x1[2]*x1[2]);
	  }

	  for (index = 0; index < ALGO->nb0_couples; index++) {
	    if (ABS((qprod_scal(x1, A[index], nc) - b[index])
		     / sqrt(1 + x1[1]*x1[1] + x1[2]*x1[2]) / b[index])
		< P_ALGO->epsilon)
	      OPT->out0[index/OPT->dim_x][index%OPT->dim_x] = '#';
	    else
	      OPT->out0[index/OPT->dim_x][index%OPT->dim_x] = '.';
	  }
	  matrix_to_screen(OPT->out0, OPT->dim_y, OPT->dim_x);

	  printf(" %4d %f avg: %g\n",ALGO->index_cycle, ti, avg);
	  printf("          x1 ==> ");
	  vector_to_screen(x1,nc);
	}
      }
      /* ****************/
    }

    new_nb_couples = ALGO->nb_couples;
    j = 0;
    for (i = 0; i < ALGO->nb_couples; i++) {
      k=eq[i];
      if (ABS((qprod_scal(x1, A[k], nc) - b[k])
	       / sqrt(1 + x1[1]*x1[1] + x1[2]*x1[2]) / b[k])
	       < P_ALGO->epsilon)
	{
	  new_nb_couples--;
	  OPT->out[eq[i]/OPT->dim_x][eq[i]%OPT->dim_x] = ALGO->step;
	}
      else {
eq[j++] = eq[i];
//printf("%d  %f %f %f\n",eq[i],A[eq[i]][0],A[eq[i]][1],A[eq[i]][2]);
      }
    }

    /* update the nb of couples left considering the current subsystem */
    ALGO->last_subsys_size = ALGO->nb_couples-new_nb_couples;
    ALGO->nb_couples = new_nb_couples;

    /* add current solution to solutions matrix */
    for (i=0; i < nc; i++)
      OPT->solutions[ALGO->step-1][i] = x1[i];

    free(x1);
    free(ligne);
}


