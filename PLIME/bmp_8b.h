#ifndef BMP_8B_H
#define BMP_8B_H

void save_bmp_8b(char *filename,unsigned char *im, point_2d_int size);
void save_bmp_8b(char *filename,unsigned char **im, point_2d_int size);
void save_bmp_1b(char *filename,unsigned char **im, point_2d_int size);
unsigned char** load_bmp_8b(char *filename, point_2d_int *dim);

#endif
