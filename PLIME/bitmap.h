#ifndef BITMAP_H
#define BITMAP_H

typedef long LONG;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

typedef struct tagBITMAPFILEHEADER {
   WORD  bfType;
   DWORD bfSize;
   WORD  bfReserved1, bfReserved2;
   DWORD bfOffBits;
} BITMAPFILEHEADER ;

typedef struct tagBITMAPINFOHEADER { 
  DWORD biSize;
  LONG  biWidth, biHeight;
  WORD  biPlanes, biBitCount;
  DWORD biCompression, biSizeImage;
  LONG  biXPelsPerMeter, biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER ;

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD ;

#define BI_RGB 0

#endif
