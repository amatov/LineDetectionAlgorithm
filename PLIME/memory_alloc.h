#ifndef MEMORY_ALLOC_H
#define MEMORY_ALLOC_H

/* memory allocation for a float nr*nc matrix */
void **matrix_alloc(int nr, int nc, unsigned int);

/* free memory previously allocated by matrix_alloc */
void matrix_free(void **, int nr);

/* memory allocation for a float vector */
void *vector_alloc(int size, unsigned int);

#endif
