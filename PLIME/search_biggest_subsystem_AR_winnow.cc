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

#include "inline_functions.cc"

/* temp */
#include "utils.h"

/****************************************************************************/
/* AR model								    */
/****************************************************************************/
void search_biggest_subsystem_AR_winnow(struct_ARmodel *AR,
					struct_algo_param *P_ALGO,
					struct_algo *ALGO){
   
    float dist1;   /* distance between current point and eq hyperplane */
    float dist;
    float avg,avg_exp;
    int i,iteration;
    float t0,ti=0; /* Initial, decrease and current temp */
    float avg1;
    int new_nb_couples,non_choisies = 0;

    float *x1;     /* Vectors of current subsystem solution */
    double *w_plus,*w_moins;

    int *ligne;
    int no_corr;   /* Flag to enable no correction mode */
int nb_corr;

    /* the following variables are defined to simplify expressions */
    float **A = AR->SYST->A;
    int nc    = AR->SYST->nc;
    float  *b = AR->SYST->b;
    int   *eq = ALGO->equations;
    /* ****** */

    ALGO->step++;
    ti = P_ALGO->T;

    /* initialisation of solution for AR model */
    x1=(float *) vector_alloc(nc, sizeof(float));
    for (int j=0; j<nc; j++) x1[j]=0;
//     x1[0]= 0.555767;
//     x1[1]= -0.769790;
//     x1[2]= 0.006390;
//     x1[3]= 0.145330;
//     x1[4]= 0.024113;
//     x1[5]= 0.243512;
//     x1[6]= -0.018858;
//     x1[7]= -0.393261;
//     x1[8]= -0.247996;
//     x1[9]= 0.763865;
//     x1[10]= -0.349203;
//     x1[11]= -0.064614;
//     x1[12]= -0.723401;
//     x1[13]= 0.215811;
//     x1[14]= 1.192900;

    w_plus=(double *) vector_alloc(nc, sizeof(double));
    for (int j=0; j<nc; j++) w_plus[j]=x1[j]+1;
    w_moins=(double *) vector_alloc(nc, sizeof(double));
    for (int j=0; j<nc; j++) w_moins[j]=1;

    /* solution monitoring (-mon_ab option) */
    FILE *mon_ab;
    if (P_ALGO->mon_ab) {
      char mon_ab_name[100];
      sprintf(mon_ab_name,"MONITOR/mon_ab_%d",ALGO->step);
      mon_ab = fopen(mon_ab_name,"w");
    }

    /* array ligne for indirect access to equations, which is necessary to   */
    /* increase performance. In the beginning every index points to the      */
    /* equation itself.                                                      */

    ligne= (int *) malloc(ALGO->nb_couples * sizeof(int));
    for (i = 0; i < ALGO->nb_couples; i++)
      ligne[i] = i;

    /* set initial temp if auto_temp_set is enable */
    if (P_ALGO->auto_temp_set) {
      calculate_v(&avg1, x1, AR->SYST, ALGO);
      t0 = P_ALGO->tzero * avg1;
      printf("Initial temperature: %f \n",t0);
    }
    /*************************************************************************/
    /* Here begins the outer loop, counting the number of cycles, i.e. the   */
    /* number of times, all equations of the equation system have been chosen*/
    /* to adapt the solution.                                                */

    for (ALGO->index_cycle = 0;
	 ALGO->index_cycle < ALGO->nb_cycles;
	 ALGO->index_cycle++) {

      /* calculate current temperature */
//      calculate_v(&avg1, x1, AR->SYST, ALGO);
//      ti = update_temp(&t0, ti, avg1, P_ALGO, ALGO);
      if (P_ALGO->auto_temp_set) { /* auto temp */
	float gamma;
	if (P_ALGO->exponential) gamma = 1/(1+(float)ALGO->index_cycle);
	else gamma = 1-(float)ALGO->index_cycle/(float)ALGO->nb_cycles;
	t0 = P_ALGO->update_tz*(P_ALGO->alpha_tz*t0+(1-P_ALGO->alpha_tz)*avg1);
	ti = gamma * t0;
      }
      else { /* no auto temp */
	if (P_ALGO->exponential) {
	  float aux = (float)ALGO->index_cycle / (float)ALGO->nb_cycles;
	  ti = (P_ALGO->T * ((1-P_ALGO->T_param)*exp(-40*aux/(2-aux))
			     + P_ALGO->T_param*exp(-5*aux)));
	}
	else { /* linear manual temp */
	  ti = P_ALGO->T *
	    (1 - (float)ALGO->index_cycle / (float)ALGO->nb_cycles) / 2;
	}
      }

      /* stop algorithm if avg/ti is upper than stop_temp */
//       if (avg1/ti > P_ALGO->stop_temp){
// 	printf("Stopped at cycle %d of %d\n", ALGO->index_cycle, ALGO->nb_cycles);
// 	ALGO->index_cycle = ALGO->nb_cycles;
//       }

      /* enable conditionned correction if avg/ti is upper than no_corr */
      if (P_ALGO->no_corrections==0 || ti < P_ALGO->no_corrections) no_corr = 0;
      else no_corr = 1;
//printf("%d %f ",no_corr,avg1/ti);

/************************************************************************/
/* Here begins the inner loop, each couple of equations of the equation */
/* system is chosen in random order.                                    */
nb_corr=0;
avg_exp = 0;
avg = 0;
 for (iteration = 0; iteration < (int)(ALGO->nb_couples*P_ALGO->partial)
	; iteration++){
   float aux;
   /* choose one couple of equations */
   i = choose_line(ligne, &non_choisies, ALGO->nb_couples);
   i = eq[i]; /* indirect access to equations */
   
   /* distance between current pt & eq hyperplane */
   dist1 = qprod_scal(A[i], x1, nc) - b[i];
   dist = dist1;
   
   if (ABS(dist1) > EPSILON) {
     
     if (AR->norm_eq[i]!=0) {
       /* apply correction */
       float alpha_win;
       if ((aux = ABS(dist)/ti) <= P_ALGO->stop_temp) {
	 alpha_win = EXP_TAB(-aux) * P_ALGO->alpha
	   * (1.0 - (float)ALGO->index_cycle / (float)ALGO->nb_cycles);

//   printf("alpha: %e  exp: %e  aux: %f\n",
//	  alpha_win,EXP_TAB(-aux),aux);
	 //       alpha_win = 1.001;

	 if (alpha_win > 1e-9) {
	   if (dist1<0)
	     for (int j=0; j<nc; j++) {
	       w_plus[j] *= pow(1+alpha_win, AR->SYST->A[i][j]);
	       w_moins[j] *= pow(1+alpha_win, -AR->SYST->A[i][j]);
	       x1[j] = w_plus[j] - w_moins[j];
	     }
	   else 
	     for (int j=0; j<nc; j++) {
	       w_plus[j] *= pow(1+alpha_win, -AR->SYST->A[i][j]);
	       w_moins[j] *= pow(1+alpha_win, AR->SYST->A[i][j]);
	       x1[j] = w_plus[j] - w_moins[j];
	     }
	 nb_corr++;
	 }

// 	   if (((float)ALGO->index_cycle / (float)ALGO->nb_cycles > 0.5) &&
// 	       (alpha_win < 1e-8)) {
// 	     //	   alpha_win *= 1e6;
// 	     alpha_win = 0.0001;
// 	     if (dist1>0)
// 	       for (int j=0; j<nc; j++) {
// 		 w_plus[j] *= pow(1+alpha_win, AR->SYST->A[i][j]);
// 		 w_moins[j] *= pow(1+alpha_win, -AR->SYST->A[i][j]);
// 		 x1[j] = w_plus[j] - w_moins[j];
// 	       }
// 	     else 
// 	       for (int j=0; j<nc; j++) {
// 		 w_plus[j] *= pow(1+alpha_win, -AR->SYST->A[i][j]);
// 		 w_moins[j] *= pow(1+alpha_win, AR->SYST->A[i][j]);
// 		 x1[j] = w_plus[j] - w_moins[j];
// 	       }
// 	   }

       }
     }
   }

   /* solution monitoring */
   if (P_ALGO->mon_ab && iteration%P_ALGO->mon_ab==0) {
     vector_to_file(mon_ab, x1, AR->order, " ");
     fprintf(mon_ab,"\n");
   }
 }

//printf("nb_corr: %d / %d\n",nb_corr,(int)(ALGO->nb_couples*P_ALGO->partial));
//printf("w+: %f w-: %f\n",norm_euclid(w_plus,nc),norm_euclid(w_moins,nc));
  
/* End of inner loop							*/
/************************************************************************/

      if (ALGO->index_cycle % (ALGO->nb_cycles/10) == 0) {
	printf ("*");
	fflush(stdout);
      }

      /* partial results monitoring */
      if (P_ALGO->mon) {
	if (!P_ALGO->auto_temp_set) {
	  calculate_v(&avg1, x1, AR->SYST, ALGO);
	}
	if ((ALGO->index_cycle % (int)(ALGO->nb_cycles/10)) == 0){
	  printf(" %4d %g avg:%g  x1 ==> ",ALGO->index_cycle,
		 (double)ti,(double)avg1);
	  vector_to_screen(x1,nc);
	}
      }

      /* temp monitoring */
      if (P_ALGO->mon_t) fprintf(P_ALGO->mon_t_file,"%g %g %g %d\n",
				 avg/nb_corr,ti,avg_exp/nb_corr,nb_corr);

    }

    /* End of outer loop						    */
    /************************************************************************/

    /* add current subsystem to the matrix of solutions */
    for (i = 0; i < AR->order;i++)
      AR->solutions[ALGO->num_extr_subsys-1][i]=x1[i];

    AR_model_update_eq(P_ALGO, ALGO, AR, P_ALGO->epsilon);

    if (ALGO->last_subsys_size > 0) {
      AR->total_av_abs_err += AR->av_abs_err;
      AR->total_av_quadr_err += AR->av_quadr_err;
      AR->av_abs_err /= (float)ALGO->last_subsys_size;
      AR->av_quadr_err /= (float)ALGO->last_subsys_size;
    }

    free(x1);
    free(ligne);

    /* close monitoring files */
    if (P_ALGO->mon_ab) fclose(mon_ab);

}

