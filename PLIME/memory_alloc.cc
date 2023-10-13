#include <stdlib.h>
#include "utils.h"

/* memory allocation for nr*nc matrix of objects of size obj_size */
void **matrix_alloc(int nr, int nc, unsigned int obj_size)
{
  int i;
  void **ptr;

//static int tot = 0;
//tot += nr*nc*obj_size;
//printf("tot matix: %d\n",tot);

  ptr=(void **) malloc(nr * sizeof(void*));
  if (!ptr) exit_with_error("matrix allocation failure.");

  for(i=0;i<nr;i++){
    ptr[i]=(void *) malloc(nc * obj_size);
    if (!ptr[i]) exit_with_error("matrix allocation failure.");
  }
//printf("### Matrix_alloc %d * %d * %d\n",nr,nc,obj_size);
  return ptr;
}

/* free memory previously allocated by matrix_alloc */
void matrix_free(void **ptr, int nr)
{
  for(int i=0;i<nr;i++) {
    fflush(stdout);
    free(ptr[i]);
  }
  free (ptr);
}

/* memory allocation for a vector of objects of size obj_size*/
void *vector_alloc(int size, unsigned int obj_size)
{
  void *ptr;

//static int tot = 0;
//tot += size*obj_size;
//printf("tot vector: %d\n",tot);

  ptr=(void *) malloc(size * obj_size);
//printf("### Vector_alloc %d * %d\n",size,obj_size);
  if (!ptr) exit_with_error("vector allocation failure.");

return ptr;
}
