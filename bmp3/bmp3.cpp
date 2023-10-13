// bmp3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <il/il.h>
#include <il/ilu.h>
#include <windows.h>
#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

void ShowBMP(char *file, SDL_Surface *screen, int x, int y)
{
	SDL_Surface *image;
    SDL_Rect dest;                             
	/* Load the BMP file into a surface */
    image = SDL_LoadBMP(file);                             
    if ( image == NULL ) 
	{
		fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());                                 
        return;                             
    }                                                          
	/* Blit onto the screen surface.
    The surfaces should not be locked at this point.*/                                                                 										 
    dest.x = x;                             
    dest.y = y;                             
    dest.w = image->w;                             
    dest.h = image->h;                             
    SDL_BlitSurface(image, NULL, screen, &dest);
                             
    /* Update the changed portion of the screen */                             
    SDL_UpdateRects(screen, 1, &dest);                             
	SDL_FreeSurface(image);
}     

main(int argc, char *argv[])
{
	unsigned char* p_inputImg;
	unsigned char* p_outputImg;
	int imgWidth, imgHeight;

	unsigned int ImgId;
	char imageName[200]; //file path

	// Load input image	with DevIL
	ilInit();//Initialize DevIL


	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
	
	ilGenImages(1, &ImgId);//Generate the main image to use
	
	ilBindImage(ImgId);// Bind this image name.

	SDL_Surface *screen;
	screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE);

    if ( screen == NULL ) 
	{
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
        exit(1);
    }                            

	for(int imgIdx = 1; imgIdx < 2; imgIdx++)//choose images
	{
		//ilLoadImage function loads an image from a file.
		sprintf(imageName, "img%d.bmp", ImgId);
		//ilLoadImage(imageName);
		//imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
		//imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		//p_outputImg = new unsigned char [imgHeight*imgWidth];
		// ilGetData function returns a pointer to the current image's data
		//p_inputImg = ilGetData();
		ShowBMP(imageName, screen, 0, 0);
	}

	getchar ();
    return 0;
}

                            
                             
                                 
