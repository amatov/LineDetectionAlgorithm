#include <stdlib.h>
#include <stdio.h>
#include "struct.h"
#include "bitmap.h"
#include "memory_alloc.h"
#include "utils.h"

/***********************************************************************/
/* Get & put byte, word & dword in binary file			       */
/***********************************************************************/
void putbyte(FILE* file, unsigned char byte) {
  fwrite(&byte,sizeof(unsigned char),1,file);
}

void putword(FILE* file, unsigned short word) {
  putbyte(file, (BYTE)((word & 0x0ff)));     /* LSB */
  putbyte(file, (BYTE)((word>>8) & 0x0ff));  /* MSB */
}

void putdword(FILE* file, unsigned long dword) {
  putbyte(file, (BYTE)((dword)     & 0x0ff)); /* LSB */
  putbyte(file, (BYTE)((dword>>8)  & 0x0ff));
  putbyte(file, (BYTE)((dword>>16) & 0x0ff));
  putbyte(file, (BYTE)((dword>>24) & 0x0ff)); /* MSB */
}

unsigned char getbyte(FILE* file) {
  unsigned char C;
  fread(&C,sizeof(unsigned char),1,file);
  return C;
}

unsigned short getword(FILE* file) {
  return ((unsigned short)getbyte(file)
	  | (((unsigned short)getbyte(file)<<8) & 0xff00));
}

unsigned long getdword(FILE* file) {
  return ((unsigned long)getbyte(file)
	  | (((unsigned long)getbyte(file)<<8)  & 0x0000ff00)
	  | (((unsigned long)getbyte(file)<<16) & 0x00ff0000)   
	  | (((unsigned long)getbyte(file)<<24) & 0xff000000));
}

/***********************************************************************/
/* write bmp file header (8 bits grey scale plalette)		       */
/***********************************************************************/
void write_header_8b(FILE *fd, point_2d_int size) {
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;

  /* Fichier d'en-tête d'un Bitmap 24 bits */
  putword(fd,bmfh.bfType=19778);
  putdword(fd,bmfh.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
	   +sizeof(RGBQUAD)*256 + size.x*size.y);
  putword(fd,bmfh.bfReserved1=0);
  putword(fd,bmfh.bfReserved2=0);
  putdword(fd,bmfh.bfOffBits=sizeof(BITMAPFILEHEADER)
	   +sizeof(RGBQUAD)*256 + sizeof(BITMAPINFOHEADER)-2);

  /* En-tête d'un Bitmap 24 bits */
  putdword(fd,bmih.biSize=sizeof(BITMAPINFOHEADER));
  putdword(fd,bmih.biWidth=size.x);
  putdword(fd,bmih.biHeight=size.y);
  putword(fd,bmih.biPlanes=1);
  putword(fd,bmih.biBitCount=8);
  putdword(fd,bmih.biCompression=BI_RGB);
  putdword(fd,bmih.biSizeImage=0);
  putdword(fd,bmih.biXPelsPerMeter=0);
  putdword(fd,bmih.biYPelsPerMeter=0);
  putdword(fd,bmih.biClrUsed=256);
  putdword(fd,bmih.biClrImportant=0);

    /* palette 8 bits 256 couleurs */
    /* 256 niveaux de gris  reg = green = blue = Y */
    /* 0 = black */
    putbyte(fd,(BYTE)(0));  /* Blue */
    putbyte(fd,(BYTE)(0));  /* Green */
    putbyte(fd,(BYTE)(0));  /* Red */
    putbyte(fd,(BYTE)(0));  /* Reserved */
    /* 1 = red */
    putbyte(fd,(BYTE)(0));  /* Blue */
    putbyte(fd,(BYTE)(0));  /* Green */
    putbyte(fd,(BYTE)(255));  /* Red */
    putbyte(fd,(BYTE)(0));  /* Reserved */
    /* 2 = green */
    putbyte(fd,(BYTE)(0));  /* Blue */
    putbyte(fd,(BYTE)(255));  /* Green */
    putbyte(fd,(BYTE)(0));  /* Red */
    putbyte(fd,(BYTE)(0));  /* Reserved */

    for(int i=3;i<256;i++) {
      putbyte(fd,(BYTE)(i));  /* Blue */
      putbyte(fd,(BYTE)(i));  /* Green */
      putbyte(fd,(BYTE)(i));  /* Red */
      putbyte(fd,(BYTE)(0));  /* Reserved */
    }
}

/***********************************************************************/
/* write bmp file header (1 bit)				       */
/***********************************************************************/
void write_header_1b(FILE *fd, point_2d_int size) {
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;

  /* Fichier d'en-tête d'un Bitmap 24 bits */
  putword(fd,bmfh.bfType=19778);
  putdword(fd,bmfh.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
	   +sizeof(RGBQUAD)*2 + size.x*size.y);
  putword(fd,bmfh.bfReserved1=0);
  putword(fd,bmfh.bfReserved2=0);
  putdword(fd,bmfh.bfOffBits=sizeof(BITMAPFILEHEADER)
	   +sizeof(RGBQUAD)*2 + sizeof(BITMAPINFOHEADER)-2);

  /* En-tête d'un Bitmap 24 bits */
  putdword(fd,bmih.biSize=sizeof(BITMAPINFOHEADER));
  putdword(fd,bmih.biWidth=size.x);
  putdword(fd,bmih.biHeight=size.y);
  putword(fd,bmih.biPlanes=1);
  putword(fd,bmih.biBitCount=1);
  putdword(fd,bmih.biCompression=BI_RGB);
  putdword(fd,bmih.biSizeImage=0);
  putdword(fd,bmih.biXPelsPerMeter=0);
  putdword(fd,bmih.biYPelsPerMeter=0);
  putdword(fd,bmih.biClrUsed=2);
  putdword(fd,bmih.biClrImportant=0);

    /* palette 1 bit */
    /* 0 = black */
    putbyte(fd,(BYTE)(0));  /* Blue */
    putbyte(fd,(BYTE)(0));  /* Green */
    putbyte(fd,(BYTE)(0));  /* Red */
    putbyte(fd,(BYTE)(0));  /* Reserved */
    /* 1 = white */
    putbyte(fd,(BYTE)(255));  /* Blue */
    putbyte(fd,(BYTE)(255));  /* Green */
    putbyte(fd,(BYTE)(255));  /* Red */
    putbyte(fd,(BYTE)(0));  /* Reserved */

}

/***********************************************************************/
/* Read bmp file header (8 bits grey scale palette)		       */
/***********************************************************************/
void read_header(FILE *fd, BITMAPFILEHEADER *bmfh, BITMAPINFOHEADER *bmih,
		 RGBQUAD *bmppal) {
  /* Fichier d'en-tête d'un Bitmap 24 bits */
  bmfh->bfType = getword(fd);
  bmfh->bfSize = getdword(fd);
  bmfh->bfReserved1 = getword(fd);
  bmfh->bfReserved2 = getword(fd);
  bmfh->bfOffBits = getdword(fd);

  /* En-tête d'un Bitmap 24 bits */
  bmih->biSize = getdword(fd);
  bmih->biWidth = getdword(fd);
  bmih->biHeight = getdword(fd);
  bmih->biPlanes = getword(fd);
  bmih->biBitCount = getword(fd);
  bmih->biCompression = getdword(fd);
  bmih->biSizeImage = getdword(fd);
  bmih->biXPelsPerMeter = getdword(fd);
  bmih->biYPelsPerMeter = getdword(fd);
  bmih->biClrUsed = getdword(fd);
  bmih->biClrImportant = getdword(fd);

  /* lecture de la palette */
  // if (bmih->biClrUsed == 0) bmih->biClrUsed = 256;
  for(int i=0;i<(int)bmih->biClrUsed;i++) {
    bmppal[i].rgbRed   = getbyte(fd);  /* Red   */
    bmppal[i].rgbGreen = getbyte(fd);  /* Green */
    bmppal[i].rgbBlue  = getbyte(fd);  /* Blue  */
    getbyte(fd);  /* Reserved */
    bmppal[i].rgbReserved = (bmppal[i].rgbRed + bmppal[i].rgbGreen
			     + bmppal[i].rgbBlue)/3; /* corresponding grey */
  }
  fseek(fd, (long)bmfh->bfOffBits, SEEK_SET);
}

/***********************************************************************/
/* Create bmp file given image in a 1D unsigned byte vector	       */
/***********************************************************************/
void save_bmp_8b(char *filename,unsigned char *im, point_2d_int size)
{
  FILE *fd;
  int i;

  if ( (fd=fopen(filename,"wb"))==NULL)
    {
      sprintf(filename,"Couldn't open %s\n",filename);
      exit_with_error(filename);
    }
  /* Ecriture le l'entete */
  write_header_8b(fd, size);
  /* Ecriture des données images dans le fichier */
  for(i=size.y-1;i>=0;i--)
    fwrite(im+(size.x)*i,sizeof(BYTE),size.x,fd);
  fclose(fd);
}

/***********************************************************************/
/* Create 8bits bmp file given image in a 2D unsigned byte matrix      */
/***********************************************************************/
void save_bmp_8b(char *filename, unsigned char **im, point_2d_int size)
{
  FILE *fd;
  int i,aux;
  unsigned char *zero_padd;
  zero_padd = (unsigned char *)calloc(4,sizeof(unsigned char));

  if ((fd=fopen(filename,"wb"))==NULL)
    {
      sprintf(filename,"Couldn't open %s\n",filename);
      exit_with_error(filename);
    }
  /* Ecriture de l'entete */
  write_header_8b(fd, size);
  /* Ecriture de l'image */
  if (size.x%4) aux = 4-size.x%4;
  else aux = 0;
  for(i=size.y-1;i>=0;i--) {
    fwrite(im[i],sizeof(BYTE),size.x,fd);
    /* each scan line must be zero-padded to end on a long boundary */
    fwrite(zero_padd,sizeof(BYTE),aux,fd);
  }
  fclose(fd);
}

/***********************************************************************/
/* Create 1bit bmp file given image in a 2D unsigned byte matrix       */
/***********************************************************************/
void save_bmp_1b(char *filename, unsigned char **im, point_2d_int size)
{
  FILE *fd;
  int i,j,offset;
  char aux = 0;

  if ((fd=fopen(filename,"wb"))==NULL)
    {
      sprintf(filename,"Couldn't open %s\n",filename);
      exit_with_error(filename);
    }
  /* Ecriture de l'entete */
  write_header_1b(fd, size);
  /* Ecriture de l'image */
  for(i=size.y-1;i>=0;i--) {
    for(j=0;j<size.x;j++) {
      if (im[i][j]) aux += 0x01<<7-j%8;
      if (j%8==7) {
	putbyte(fd, aux);
	aux = 0;
      }
    }
    /* each scan line must be zero-padded to end on a long boundary */
    if (size.x%8) {
      putbyte(fd, aux);
      for (j=0;j<(4-(size.x/8+1)%4);j++) putbyte(fd,0);
    }
    else
      if (size.x%4)
	for (j=0;j<(4-(size.x/8)%4);j++) putbyte(fd,0);
  }

  fclose(fd);
}

/***********************************************************************/
/* Read bmp file & return a 2D unsigned char matrix		       */
/* (only 8bit or 1bit per pixel)				       */
/***********************************************************************/
unsigned char** load_bmp_8b(char *filename, point_2d_int *dim) {
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;
  RGBQUAD *bmppal;
  unsigned char **im;
  unsigned char aux;
  FILE *fd;
  int i,j,dim_x,dim_y;
  
  if ((fd=fopen(filename,"rb"))==NULL)
    {
      sprintf(filename,"Couldn't open %s\n",filename);
      exit_with_error(filename);
    }
  /* lecture de l'entete */
  bmppal = (RGBQUAD*) malloc(256 * sizeof(RGBQUAD));
  read_header(fd, &bmfh, &bmih, bmppal);
  dim->x = dim_x = (int) bmih.biWidth;
  dim->y = dim_y = (int) bmih.biHeight;
  im =(unsigned char **) matrix_alloc(dim_y, dim_x, sizeof(unsigned char));
  if (bmih.biBitCount == 8) /* image 8 bits/pixel */
    for(i=(dim_y)-1; i>=0; i--)
      fread(im[i],sizeof(BYTE),dim_x,fd);
  else
    if (bmih.biBitCount == 1) { /* image 1 bit/pixel */
      i = 0;
      int nb_pts;
      nb_pts = dim_x * dim_y;
      /* image=0 */
      for(i=0; i<dim_y; i++)
	for(j=0; j<dim_x; j++) im[i][j] = bmppal[0].rgbReserved;
      /* lecture + conversion 1bit->8bits */
      j=-1;
      for(i=0; i<nb_pts; i++) {
	if (i%8==0) {
	  fread(&aux,sizeof(BYTE),1,fd);
	  j +=8;
	}
	  if((aux>>(i%8) & 0x01)==0x01)
	    im[dim_y-1-i/dim_x][(j-(i%8))%dim_x] = bmppal[1].rgbReserved;
      }
    }
    else exit_with_error("Can't read bmp file (only 1 or 8 bits/pixel)");
  fclose(fd);
  return im;
}











