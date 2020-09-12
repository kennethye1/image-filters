#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Main filter loop.
 * This function is responsible for doing the following:
 *   1. Read in pixels one at a time (because copy is a pixel-by-pixel transformation).
 *   2. Change pixels to the average.
     3. Write out pixels right after.

 */
void greyscale_filter(Bitmap *bmp) {
    for(int i = 0; i < bmp->height * bmp->width; i++){
    	Pixel pixel;
    	if(fread(&pixel, 1, sizeof(Pixel), stdin) != sizeof(Pixel)){
    		perror("fread");
    		exit(1);
    	}
        int avg = (pixel.red + pixel.blue + pixel.green) / 3;
        pixel.red = avg;
        pixel.blue = avg;
        pixel.green = avg;

    	if(fwrite(&pixel, 1, sizeof(Pixel), stdout) != sizeof(Pixel)){
    		perror("fwrite");
    		exit(1);
    	}	
    }
    return;
}

int main() {
    run_filter(greyscale_filter, 1);
    return 0;
}
