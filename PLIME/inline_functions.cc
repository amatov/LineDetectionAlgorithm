/****************************************************************************/
/* The following functions are inluded in algorithm's core source files to  */
/* be compile inline (in files search_biggest_subsystem_###.cc)		    */
/****************************************************************************/

/* Find the tangente of the edge at point given by index_point by using the */
/* least-square methode in a small neighbourhood. The returned value is 1 if*/
/* ok end 0 if we can not estimate the tangent and it is the experimental   */
/* correlation coefficient if -remove_bad_pts is enable. (||sl||=1)         */
inline float find_tangente_of_edge(struct_image image, float *p,
			    int tan_estimation, int remove_bad_pts,
			    point_2d_float *sl)
{
  int num_pixels;
  int x, y, x_min, x_max, y_min, y_max;
  int sum1, sum2, sum3;
  float slope, theta;
  int p_x,p_y;

p_x = (int)p[0];
p_y = (int)p[1];

  x_min = p_x - tan_estimation;
  if (x_min < 0)
    x_min = 0;
  x_max = p_x + tan_estimation;
  if (x_max >= image.dim.x)
    x_max = image.dim.x-1;

  y_min = p_y - tan_estimation;
  if (y_min < 0)
    y_min = 0;
  y_max = p_y + tan_estimation;
  if (y_max >= image.dim.y)
    y_max = image.dim.y-1;

  num_pixels = 0;
  sum1 = sum2 = sum3 = 0;
  for (x=x_min; x <= x_max; x++)
    for (y=y_min; y <= y_max; y++)
      if (image.im[y][x]!=image.background_color)
	{
	  num_pixels++;
	  sum1 += (x-p_x)*(y-p_y);
	  sum2 += (x-p_x)*(x-p_x);
	  sum3 += (y-p_y)*(y-p_y);
	}
  /* sum1/sum2 is the slope of the tangente */
  if (num_pixels == 1) return 0;
  if (sum2==0) { /* vertical */
    sl->x = 0;
    sl->y = 1;
    return 1;
  }
  if (sum3==0) { /* horizontal */
    sl->x = 1;
    sl->y = 0;
    return 1;
  }
  if (sum2 > sum3) { 
    sl->x = (float)sum2;
    sl->y = (float)sum1;
  }
  else { 
    sl->x = (float)sum1;
    sl->y = (float)sum3;
  }
  /* experimental correlation coefficient */
  if (remove_bad_pts && sum1)
    return ABS(float(sum1)/sqrt(float(sum2)*float(sum3)));
  else return 1;
}

/* This function chooses one couple of equations of the equation system     */
/* at random. This is needed to have an "almost cyclic control", as         */
/* proposed in the algorithm.                                               */
   
inline int choose_line(int *l, int *nc, int nb_couples)
{
  int temp, result;

/* there's a counter (*nc) to indicate the number of couples not yet chosen */
/* When this counter is decreased to 0, it will be reset to the number of   */
/* couples which are altogether in the equation system.                     */
/* The lines are accessed indirectly in order to avoid scanning all         */
/* equations at every iteration.                                            */
 
  if (*nc == 0) *nc = nb_couples; 
//  temp = rand() % (*nc);
  temp = (int)((float)rand() / 32768 * (*nc));
  result = l[temp];
  (*nc)--;
  l[temp] = l[*nc];
  l[*nc] = *nc;
  return result;
}

/****************************************************************************/
/* Calculate_v | mean distance to current solution                          */
/****************************************************************************/
inline void calculate_v(float *res1,
		 float *x1,
		 struct_eq_system *SYST, struct_algo *ALGO)
{
  float sum1 = 0;
  int j;

  for (int i=0; i < ALGO->nb_couples; i++){
    j = ALGO->equations[i];
    sum1 += ABS(qprod_scal(SYST->A[j], x1, SYST->nc) - SYST->b[j]);
//      /sqrt(SYST->A[j][0]*SYST->A[j][0]+SYST->A[j][1]*SYST->A[j][1]);
  }
  
  *res1 = sum1 / (float)ALGO->nb_couples;
}

/****************************************************************************/
/* Update_temp (return ti & update t0)                                      */
/****************************************************************************/
inline float update_temp(float *t0, float ti, float avg,
		  struct_algo_param *P_ALGO,
		  struct_algo *ALGO)
{
  float gamma;

  if (P_ALGO->auto_temp_set) { /* ****** auto temp enable */
    if (ALGO->index_cycle)
      *t0 = P_ALGO->update_tz*(P_ALGO->alpha_tz*(*t0) + (1-P_ALGO->alpha_tz)*avg);
    
    if (P_ALGO->exponential) { /* *** exponential scheme */
      gamma = 1/(1+0.5*(float)ALGO->index_cycle);
    }
    else {  /* *** linear scheme */
      gamma = 1.0 - (float)ALGO->index_cycle / (float)ALGO->nb_cycles;
    }
    return (gamma * (*t0));
  }
  else { /* ****** auto temp disable */
    /* exponential scheme for calculating current temperature. */
    if (P_ALGO->exponential) {
//       return (P_ALGO->T * 2.5 * (exp(-1/(1.2-(float)ALGO->index_cycle /
// 	   (float)ALGO->nb_cycles))));
      float aux = (float)ALGO->index_cycle / (float)ALGO->nb_cycles;
      return (P_ALGO->T * ((1-P_ALGO->T_param)*exp(-40*aux/(2-aux))
			   + P_ALGO->T_param*exp(-2.5*aux)));
    }

    /* linear scheme for calculating current temperature. */
    else
      return (P_ALGO->T * (1.0 - (float)ALGO->index_cycle / (float)ALGO->nb_cycles));
  }
}

/****************************************************************************/
/* Apply correction to current solution                                     */
/****************************************************************************/
inline float inline_exp(float x)
{
   if (x==0) return 1;
   float res = 1;
   if (x>0) {
     for (int i=0; i<(int)(x*1.442695); i++) res *= 2;
     return res;
   }
   else {
     for (int i=0; i<(int)(-x*1.442695); i++) res /= 2;
     return res;
   }
}

inline void update_current_solution(float *x,float t0,float ti, int nc,
				    float dist, struct_eq_system *SYST,
				    struct_algo_param *P_ALGO,
				    struct_algo *ALGO, int i_eq)
{
  int j;

  if (P_ALGO->auto_temp_set){
    if (dist < 0 ) /* dist<0 & auto_temp */
      for (j=0; j<nc; j++)
	x[j]+= (ti/t0) * SYST->A[i_eq][j] * ALGO->weight[j] * 
	  exp(- ABS(dist)/ti); 
    else           /* dist>0 & auto_temp */
      for (j=0; j<nc; j++)
	x[j]-= (ti/t0) * SYST->A[i_eq][j] * ALGO->weight[j] *
	  exp(- ABS(dist)/ti);
  }
  else {
    float norm = 0;
    for (int i=0; i<nc; i++) norm += SYST->A[i_eq][i]*SYST->A[i_eq][i];
    norm = sqrt(norm);

    float r_dist;
    r_dist = (qprod_scal(SYST->A[i_eq],x,nc)-SYST->b[i_eq])/norm;

//    if (dist < 0) /* dist<0 & no auto_temp */
      for (j=0; j<nc; j++)
//  	x[j]+=P_ALGO->alpha * SYST->A[i_eq][j] * ALGO->weight[j] *
//  	  (1.0 - (float)ALGO->index_cycle / (float)ALGO->nb_cycles)
//  	  * inline_exp(- ABS(dist)/ti); 
  	x[j]-= SYST->A[i_eq][j] * r_dist / norm; 
//    else           /* dist>0 & no auto_temp */
//      for (j=0; j<nc; j++)
//  	x[j]-=P_ALGO->alpha * SYST->A[i_eq][j] * ALGO->weight[j] *
//  	  (1.0 - (float)ALGO->index_cycle / (float)ALGO->nb_cycles)
//  	  * inline_exp(- ABS(dist)/ti);
//  	x[j]-= SYST->A[i_eq][j] * r_dist / norm;

//  float r_dist2 = 0;
//  r_dist2 = (qprod_scal(SYST->A[i_eq],x,nc)-SYST->b[i_eq]) / norm;
//  printf("%f %f\n",r_dist,r_dist2);
 
  }
}

