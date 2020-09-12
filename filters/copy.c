#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Main filter loop.
 * This function is responsible for doing the following:
 *   1. Read in pixels one at a time (because copy is a pixel-by-pixel transformation).
 *   2. Immediately write out each pixel.
 */
void copy_filter(Bitmap *bmp) {
    for(int i = 0; i < bmp->height * bmp->width; i++){
    	Pixel pixel;
    	if(fread(&pixel, 1, sizeof(Pixel), stdin) != sizeof(Pixel)){
    		perror("fread");
    		exit(1);
    	}
    	if(fwrite(&pixel, 1, sizeof(Pixel), stdout) != sizeof(Pixel)){
    		perror("fwrite");
    		exit(1);
    	}	
    }
    return;
}

int main() {
    run_filter(copy_filter, 1);
    return 0;
}
