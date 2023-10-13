#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "struct.h"

void exit_with_error(char error_msg[]);
void exit_with_error(char error_msg[],char *);

void vector_copy(float *target, float *data, int size);

int   read_int_from_file (FILE *);
float read_float_from_file (FILE *);
int   read_string_from_file (char *, int, FILE *);
FILE* fopen_w(char *);
FILE* fopen_a(char *);
FILE* fopen_r(char *);

void matrix_to_screen(float **, int, int);
void matrix_to_screen(int **, int, int);
void matrix_to_screen(char **, int, int);
void matrix_to_screen_out(int **, int, int);
void matrix_to_screen_out(char **, int, int);

void vector_to_screen(float *, int);
void vector_to_screen_int(float *,int);
void vector_to_screen(int *, int);
void vector_to_screen(unsigned char *, int);
void vector_to_screen(char **, int);

void vector_to_file(FILE *,float *V, int size, char *str);
void vector_to_file(FILE *,int *V, int size, char *str);

void put_segment_in_image(unsigned char **im, point_2d_int dim,
			  float x1, float y1, float x2, float y2,
			  unsigned char color);
void put_line_in_image(unsigned char **im, point_2d_int dim,
		       float x1, float y1, float x2, float y2,
		       unsigned char color);
void put_circle_in_image(unsigned char **im, point_2d_int dim,
			 float c_x, float c_y, float rayon, unsigned char color);
void put_conic_in_image(unsigned char **im, point_2d_int dim,
			float *coeff, unsigned char color);
void put_ellipse_in_image(unsigned char **im, point_2d_int dim,
			  float *coeff, unsigned char color);

float get_process_time();

void done(int);
void wait_debug();

#endif

