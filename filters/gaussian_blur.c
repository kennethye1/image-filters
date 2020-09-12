#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Main filter loop.
 * Does the gaussian blur filter
 */
void gaussian_blur_filter(Bitmap *bmp) {
//Probably cannot do this if pixel width or height is less than 3?
    if(bmp->height < 3 || bmp->width < 3){
        fprintf(stderr, "Cannot perform gaussian blur with less than 3 height or width");
        exit(1);
    }
    int width = bmp->width;
    int height = bmp->height;

    int i;
    //allocates memory for 3 rows
    Pixel **pixel_rows = malloc(sizeof(Pixel *) * 3);
    //first get the first three rows
    for(i = 0; i < 3; i++){
        pixel_rows[i] = malloc(sizeof(Pixel) * width);
        for(int j = 0; j < width; j++){
            if(fread(&pixel_rows[i][j], 1, sizeof(Pixel), stdin) != sizeof(Pixel)){
                perror("fread");
                exit(1);
            }
        }
    }
    //write out the first two rows
    for(int k = 0; k < 2; k++){
        for(int l = 0; l < width; l++){
            int row = k;
            int column = l;
            if(row == 0){
                row++;
            }
            if(column == 0){
                column++;
            } else if(column == width - 1){
                column--; 
            }

            Pixel grid[3][3];
            for(int a = 0; a < 3; a++){
                for(int b = 0; b < 3; b++){
                    grid[a][b] = pixel_rows[row + a - 1][column + b - 1];
                }
            }
            Pixel gauss = apply_gaussian_kernel(grid[0], grid[1], grid[2]);
            if(fwrite(&gauss, 1, sizeof(Pixel), stdout) != sizeof(Pixel)){
                perror("fwrite");
                exit(1);
            }
        }
    }
    //in between
    while(i < height) {
        //read in new row and shifting one up 
        for(int l = 0; l < width; l++){
            pixel_rows[0][l] = pixel_rows[1][l];
            pixel_rows[1][l] = pixel_rows[2][l];
            if(fread(&pixel_rows[2][l], 1, sizeof(Pixel), stdin) != sizeof(Pixel)){
                perror("fread");
                exit(1);
            }
        }
        for(int c = 0; c < width; c++){
            int column = c;
            if(column == 0){
                column++;
            } else if(column == width-1){
                column--;
            }
            Pixel grid[3][3];
            for(int a = 0; a < 3; a++){
                for(int b = 0; b < 3; b++){
                    grid[a][b] = pixel_rows[a][column + b - 1];
                }
            }
            Pixel gauss = apply_gaussian_kernel(grid[0], grid[1], grid[2]);
            if(fwrite(&gauss, 1, sizeof(Pixel), stdout) != sizeof(Pixel)){
                perror("fwrite");
                exit(1);
            }

        }
        i++;
    }

    //Case: Last row
    //Note that the changed variables will be the same
   for(int x = 0; x < width; x++){
        int column = x;
        if(column == 0){
            column++;
        }else if(column == width-1){
            column--;
        }
        Pixel grid[3][3];
        for(int a = 0; a < 3; a++){
            for(int b = 0; b < 3; b++){
                grid[a][b] = pixel_rows[a][column + b - 1];    
            }
        }
        Pixel gauss = apply_gaussian_kernel(grid[0], grid[1], grid[2]);
        if(fwrite(&gauss, 1, sizeof(Pixel), stdout) != sizeof(Pixel)){
            perror("fwrite");
            exit(1);
        }
    }

    for(int m = 0; m < 3; m++){
        free(pixel_rows[m]);
    }
    free(pixel_rows);
}

int main() {
    // Run the filter program with gaussian_blur to process the pixels.
    run_filter(gaussian_blur_filter, 1);
    return 0;
}
