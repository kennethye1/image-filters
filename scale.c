#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

/*
 * Main filter loop.
 * This function is responsible for doing the following:
 *   1. Read in pixels one at a time (because copy is a pixel-by-pixel transformation).
 *   2. Immediately write out each pixel.
 *
 * Note that this function should allocate space only for a single Pixel;
 * do *not* store more than one Pixel at a time, it isn't necessary here!
 */
void scale_filter(Bitmap *bmp) {
    Pixel *row = malloc(bmp->width * sizeof(Pixel));
    for(int k = 0; k < bmp->height; k++){
        for(int i = 0; i < bmp->width; i++){
    	   if(fread(&row[i], 1, sizeof(Pixel), stdin) != sizeof(Pixel)){
    		  perror("fread");
    		  exit(1);
    	   }

        }
        for(int j = 0; j < bmp->scale_factor; j++){
            for(int a = 0; a <  bmp->width; a++){
                for(int b = 0; b < bmp->scale_factor; b++){
                    if(fwrite(&row[a], sizeof(Pixel), 1, stdout) != 1){
                        perror("fwrite");
                        exit(1);
                    }

                }
            }
        }
    }
    free(row);
}

int main(int argc, char** argv) {
    if(argc != 2){
        fprintf(stderr, "Needs scale factor input");
        return 1;
    }
    int scale = strtol(argv[1], NULL, 10);
    run_filter(scale_filter, scale);
    return 0;
}