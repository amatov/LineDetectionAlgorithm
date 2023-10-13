#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "math_functions.h"
#include "struct.h"

/***********************************************************************/
/* ERRORS							       */
/***********************************************************************/
void exit_with_error(char error_msg[])
{
  fprintf(stderr,"\nExited abnormally with error\n===> %s\n\n",error_msg);
  exit(0);
}

void exit_with_error(char error_msg[],char *str)
{
  fprintf(stderr,"\nExited abnormally with error\n===> %s\n(%s)\n\n",
	  error_msg,str);
  exit(0);
}

/***********************************************************************/
/* Copy two float vectors                                              */
/***********************************************************************/
void vector_copy (float *target, float *data, int size)
{
  for (int i=0; i<size; i++) target[i] = data[i];
}

/***********************************************************************/
/* read an integer from an ascii file with one or more space, carriage */
/* return or tabulation as separator                                   */ 
/***********************************************************************/
int read_int_from_file (FILE *f)
{
  char chaine[50];
  char c;

  chaine[0]='\0';
  do
    c = fgetc(f);
  while ((c ==' ' || c =='\n' || c =='\t') && !feof(f));
  while (c != ' ' && c != '\n' && c !='\t' && !feof(f)) {
    sprintf(chaine, "%s%c", chaine, c);
    c = fgetc(f);
  }
  return (atoi(chaine));
}
 
/***********************************************************************/
/* read a float from an ascii file with one or more space, carriage    */
/* return or tabulation as separator                                   */ 
/***********************************************************************/
float read_float_from_file (FILE *f)
{
  char chaine[50];
  char c;

  chaine[0]='\0';
  do
    c = fgetc(f);
  while ((c ==' ' || c =='\n' || c =='\t') && !feof(f));
  while (c != ' ' && c != '\n' && c !='\t' && !feof(f)){
    sprintf(chaine, "%s%c", chaine, c);
    c = fgetc(f);
  }
  return (atof(chaine));
}

/***********************************************************************/
/* read a string from an ascii file with one or more space, carriage   */
/* return or tabulation as separator.                                  */
/* (anythink enclosed by '//' & '\n' is ignored)                       */
/***********************************************************************/
int read_string_from_file (char *chaine, int max_char, FILE *f)
{
  char c,last_c='\0';
  int i=0;

  chaine[0]='\0';
  do
    c = fgetc(f);
  while ((c ==' ' || c =='\n' || c =='\t') && !feof(f));
  while (c !=' ' && c !='\n' && c !='\t' && !feof(f)) {
    /* Remove comments */
    if ((c =='/') && (last_c =='/')) {
      chaine[strlen(chaine)-1]='\0';
      while (c !='\n' && !feof(f)) c=fgetc(f);
      if (chaine[0]!='\0') return i;
    }
    else /* update string */
      if (++i < max_char) sprintf(chaine, "%s%c", chaine, c);

    last_c = c;
    c = fgetc(f);
  }
  return i; /* return length of string */
}

/***********************************************************************/
/* Create new ascii file and check for errors.			       */
/***********************************************************************/
FILE* fopen_w(char *file_name)
{
  FILE *f;
  char err[100];
  f=fopen(file_name,"w");
  if (!f) {
    sprintf(err, "Can't create file '%s'", file_name);
    exit_with_error(err);
  }
  return f;
}

/***********************************************************************/
/* Open an existing ascii file in append mode and check for errors.    */
/***********************************************************************/
FILE* fopen_a(char *file_name)
{
  FILE *f;
  char err[100];
  f=fopen(file_name,"a");
  if (!f) {
    sprintf(err, "Can't open file '%s'", file_name);
    exit_with_error(err);
  }
  return f;
}

/***********************************************************************/
/* Open existing ascii file in read mode and check for errors.	       */
/***********************************************************************/
FILE* fopen_r(char *file_name)
{
  FILE *f;
  char err[100];
  f=fopen(file_name,"r");
  if (!f) {
    sprintf(err, "Can't open file '%s'", file_name);
    exit_with_error(err);
  }
  return f;
}

/***********************************************************************/
/* Display matrix in std out					       */
/***********************************************************************/
/* float matrix */
void matrix_to_screen(float **M, int nr, int nc)
{
  int i,j;
  for (i=0;i<nr;i++){
    printf ("\n");
    for (j=0;j<nc;j++) printf ("%f ", M[i][j]);
  }
  printf ("\n");
}

/* int matrix */
void matrix_to_screen(int **M, int nr, int nc)
{
  int i,j;
  for (i=0;i<nr;i++){
    printf ("\n");
    for (j=0;j<nc;j++) printf ("%d ", M[i][j]);
  }
  printf ("\n");
}

void matrix_to_screen_out(int **M, int nr, int nc)
{
  int i,j;
  for (i=0;i<nr;i++){
    printf ("\n");
    for (j=0;j<nc;j++)
      if (M[i][j] > 0) printf ("%d", M[i][j]);
      else
	if (M[i][j] < 0) printf("+");
	else printf (" ");
  }
  printf ("\n");
}

/* char matrix */
void matrix_to_screen(char **M, int nr, int nc)
{
  int i,j;
  for (i=0;i<nr;i++){
    printf ("\n");
    for (j=0;j<nc;j++) printf ("%c ", M[i][j]);
  }
  printf ("\n");
}

void matrix_to_screen_out(char **M, int nr, int nc)
{
  int i,j;
  for (i=0;i<nr;i++){
    printf ("\n");
    for (j=0;j<nc;j++)
      if (M[i][j]!='.') printf ("%c", M[i][j]);
      else printf ("%c", M[i][j]);
  }
  printf ("\n");
}

/***********************************************************************/
/* Display vector in std out					       */
/***********************************************************************/
/* float vector */
void vector_to_screen(float *V, int size)
{
  for (int i=0;i<size;i++) printf ("%f ", V[i]);
  printf ("\n");
}

void vector_to_screen_int(float *V, int size)
{
  for (int i=0;i<size;i++) printf ("%d ", round_to_int(V[i]));
  printf ("\n");
}

/* int vector */
void vector_to_screen(int *V, int size)
{
  for (int i=0;i<size;i++) printf ("%d ", V[i]);
  printf ("\n");
}

/* unsigned char vector */
void vector_to_screen(unsigned char *V, int size)
{
  for (int i=0;i<size;i++) printf ("%d ", (int)V[i]);
  printf ("\n");
}

/* string vector */
void vector_to_screen(char **V, int size)
{
  for (int i=0;i<size;i++) printf("%s ", V[i]);
  printf ("\n");
}

/***********************************************************************/
/* Write a vector to an ascii file (with spaces as separator)  	       */
/***********************************************************************/
void vector_to_file(FILE * file, float *V, int size, char *str)
{
  int i;
  for (i=0;i<size;i++) fprintf (file, "%f ", V[i]);
  fprintf (file, str);
}

void vector_to_file(FILE * file, int *V, int size, char *str)
{
  int i;
  for (i=0;i<size;i++) fprintf (file, "%d ", V[i]);
  fprintf (file, str);
}

/***********************************************************************/
/* Create segment in an 8-bits image				       */
/***********************************************************************/
void put_segment_in_image(unsigned char **im, point_2d_int dim, float x1,
			  float y1, float x2, float y2, unsigned char color)
{
  point_2d_float u;
  float norm_u;
  float x,y;

  u.x = x2 - x1;
  u.y = y2 - y1;
  norm_u = sqrt(u.x*u.x + u.y*u.y);
  u.x /= norm_u;
  u.y /= norm_u;

  for (int i=0; i < (int)(norm_u+1); i++) {
    x = x1 + i*u.x;
    y = y1 + i*u.y;
    if ((x>=0) && (y>=0) && (x<=dim.x-1) && (y<=dim.y-1))
      im[round_to_int(y)][round_to_int(x)] = color;
  }
}

/***********************************************************************/
/* Create line in an 8-bits image				       */
/***********************************************************************/
void put_line_in_image(unsigned char **im, point_2d_int dim,
		       float x1, float y1, float x2, float y2, unsigned char color)
{
  float aux,x,y,k_min,k_max,k,size;

  if (x1>x2) { /* x1<->x2 & y1<->y2 */
    aux=x2;
    x2=x1;
    x1=aux;
    aux=y2;
    y2=y1;
    y1=aux;
  }

  if (x1==x2) /* horizontal line */
    for (k=0; k<dim.y; k++) im[round_to_int(k)][round_to_int(x1)]=color;
  else
    if (y1==y2) /* vertical line */
      for (k=0; k<dim.x; k++) im[round_to_int(y1)][round_to_int(k)]=color;
    else {
      if (y2>y1) { /* neither vertical nor horizontal */
	k_min = max_value(x1/(x1-x2), y1/(y1-y2));
	k_max = min_value((dim.x-x1)/(x2-x1), (dim.y-y1)/(y2-y1));
      }
      else {
	k_min = max_value(x1/(x1-x2), (dim.y-y1)/(y2-y1));
	k_max = min_value((dim.x-x1)/(x2-x1), y1/(y1-y2));
      }
      size = (k_max-k_min) * sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
      for (k=k_min; k<k_max; k+=(k_max-k_min)/size) {
	x = x1 + k*(x2-x1);
	y = y1 + k*(y2-y1);
	if ((x>=0) && (y>=0) && (x<=dim.x-1) && (y<=dim.y-1))
	  im[round_to_int(y)][round_to_int(x)] = color;
      }
    }
}

/***********************************************************************/
/* Create circle in an 8-bits image				       */
/***********************************************************************/
void put_circle_in_image(unsigned char **im, point_2d_int dim,
			 float c_x, float c_y, float rayon, unsigned char color)
{
float teta;
int x,y;

for (teta=0; teta<=TWO_PI; teta+=1/rayon)
  {
    x = (int)(c_x + rayon*cos(teta));
    y = (int)(c_y + rayon*sin(teta));
    if ((x<dim.x) && (x>=0) && (y<dim.y) && (y>=0)) {
      im[(int)floor(y)][(int)floor(x)] = color;
    }
  }
}

/***********************************************************************/
/* Create conic in an 8-bits image with the 6 implicite eq parameters  */
/* A*x^2 + B*xy + C*y^2 + D*x + E*y + F = 0			       */
/***********************************************************************/
void put_conic_in_image(unsigned char **im, point_2d_int dim,
			float *coeff, unsigned char color)
#define A coeff[0]
#define B coeff[1]
#define C coeff[2]
#define D coeff[3]
#define E coeff[4]
#define F coeff[5]
{
  int x1,x2,y1,y2;
  float aux;
  for (int x=0; x<dim.x; x++) {
    aux = (E + B*x)*(E + B*x) - 4*C*(F + x*(D + A*x));
    if (aux >= 0) {
      aux = sqrt(aux);
      y1 = round_to_int(-(E + B*x + aux)/2/C);
      y2 = round_to_int(-(E + B*x - aux)/2/C);
      if (y1>=0 && y1<dim.y) im[y1][x]=color;
      if (y2>=0 && y2<dim.y) im[y2][x]=color;
      if (y1>=1 && y1<dim.y) im[y1-1][x]=color;
      if (y2>=1 && y2<dim.y) im[y2-1][x]=color;
      if (y1>=0 && y1<dim.y-1) im[y1+1][x]=color;
      if (y2>=0 && y2<dim.y-1) im[y2+1][x]=color;
    }
  }
  for (int y=0; y<dim.y; y++) {
    aux = (D + B*y)*(D + B*y) - 4*A*(F + y*(E + C*y));
    if (aux >= 0) {
      aux = sqrt(aux);
      x1 = round_to_int(-(D + B*y + aux)/2/A);
      x2 = round_to_int(-(D + B*y - aux)/2/A);
      if (x1>=0 && x1<dim.x) im[y][x1]=color;
      if (x2>=0 && x2<dim.x) im[y][x2]=color;
      if (x1>=1 && x1<dim.x) im[y][x1-1]=color;
      if (x2>=1 && x2<dim.x) im[y][x2-1]=color;
      if (x1>=0 && x1<dim.x-1) im[y][x1+1]=color;
      if (x2>=0 && x2<dim.x-1) im[y][x2+1]=color;
    }
  }
}
#undef A
#undef B
#undef C
#undef D
#undef E
#undef F

/***********************************************************************/
/* Create ellipse in an 8-bits image with the two foyers & a	       */
/***********************************************************************/
void put_ellipse_in_image(unsigned char **im, point_2d_int dim,
			  float *coeff, unsigned char color)
#define X1 coeff[0]
#define Y1 coeff[1]
#define X2 coeff[2]
#define Y2 coeff[3]
#define AA coeff[4]
{
  float impl_coeff[6];
  float xo,yo,a2,b2,c2;
  double SIN,COS,SIN2,COS2,SINCOS;

  xo = (X1+X2)/2;
  yo = (Y1+Y2)/2;
  c2 = ((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1))/4;
  a2 = AA*AA/4;
  b2 = a2-c2;
  COS = (X2-X1) / (2*sqrt(c2));
  SIN = -(Y1-Y2) / (2*sqrt(c2)); /* indirect base */
  SINCOS = SIN*COS;
  COS2 = COS*COS;
  SIN2 = SIN*SIN;

  if (b2>0) {
    impl_coeff[0] = b2*COS2+a2*SIN2;		/* x^2 */
    impl_coeff[1] = 2*SINCOS*(b2-a2);		/* xy  */
    impl_coeff[2] = a2*COS2+b2*SIN2;		/* y^2 */
    impl_coeff[3] = -2*(SINCOS*yo*(b2-a2) + b2*xo*COS2 + a2*xo*SIN2);	/* x */
    impl_coeff[4] = -2*(SINCOS*xo*(b2-a2) + a2*yo*COS2 + b2*yo*SIN2);	/* y */
    impl_coeff[5] = COS2*(b2*xo*xo + a2*yo*yo) + SIN2*(a2*xo*xo + b2*yo*yo) 
      + 2*xo*yo*SINCOS*(b2-a2) - a2*b2;		/* cst */

    put_conic_in_image(im, dim, impl_coeff, color);
  }
}
#undef X1
#undef Y1
#undef X2
#undef Y2
#undef AA

/***********************************************************************/
/* Get CPU time from the begining of the process		       */
/***********************************************************************/
float get_process_time() {
   rusage rusage;
   float time;
   if (!getrusage(RUSAGE_SELF, &rusage)) {
     time = rusage.ru_utime.tv_sec + (float)rusage.ru_utime.tv_usec * 1E-6;
   }
   else time = 0;
   return time;
}

/* ****** DEBUG ****** */
void done(int n) {
printf("\n\n********* done %d ***********\n\n",n);
}

void wait_debug() {
  char x;
  scanf (&x,"wait_debug %s");
}
