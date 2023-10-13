#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#define TWO_PI 2*M_PI
#define ABS(x) ((x)<0 ? (-(x)) : (x))
#define FL2INT(x) (((x)-floor(x)) >= 0.5 ? (int)ceil(x) : (int)floor(x))
#define SQR(x) ((x)*(x))
#define RAND ((float)rand()/32767)

#define MAX_EXP_NEG 300
//#define EXP_TAB(x) _TAB_EXP[MAX_EXP_NEG+(int)(x)-1]
#define EXP_TAB(x) _TAB_EXP[300+(int)(x)]
extern float *_TAB_EXP; /* global ptr to access the exp array */

float qprod_scal(float *v1, float *v2, int dim);
float norm_euclid(float x, float y);
double norm_euclid(double *x, int size);
float sq_norm_euclid(float x, float y);
int round_to_int(float x);

int index_max_value(int *V, int size);
void sort_value(float *v1, float *v2);
void sort_value(int *v1, int *v2);
float max_value(float v1, float v2);
int max_value(int v1, int v2);
float min_value(float v1, float v2);
int min_value(int v1, int v2);
float min_abs_value(float v1, float v2);

float gasdev(int *idum);

float ellipse_length(float *coeff);

void init_exp_array(float **);

#endif
