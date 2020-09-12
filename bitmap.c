#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"


/*
 * Read in bitmap header data from stdin, and return a pointer to
 * a new Bitmap struct containing the important metadata for the image file.
 *
 * TODO: complete this function.
 *
 * Notes:
 *   1. Store header data in an array of unsigned char (essentially
 *      an array of bytes). Examples:
 *      on the stack,
 *          unsigned char data[10];
 *      on the heap,
 *          unsigned char *data = malloc(10);
 *
 *   2. Don't make any assumptions about the header size. You should read in
 *      BMP_HEADER_SIZE_OFFSET bytes first, and then the header size,
 *      and then use this to allocate enough space for the actual header.
 *
 *   3. You can use memcpy to transfer bytes to and from the Bitmap "header" field.
 *      You can even write these bytes to memory allocated for variables of other types!
 *      For example:
 *          unsigned char bytes[4];
 *          int x = 10;
 *          int y;
 *          memcpy(bytes, &x, 4);  // Copy the int x into bytes.
 *          memcpy(&y, bytes, 4);  // Copy the contents of bytes into y.
 *
 *   4. You can use either fread/fwrite or read/write to perform I/O operations here.
 *
 *   5. Make good use of the provided macros in bitmap.h to index into the "header" array.
 */
Bitmap *read_header() {
    int height, width, header_size;
    //allocating memory for bitmap
    Bitmap* bitmap_ptr = malloc(sizeof(Bitmap));
    unsigned char initial_data[BMP_HEADER_SIZE_OFFSET];
    unsigned char head_size[sizeof(int)];

    if(fread(initial_data, 1, BMP_HEADER_SIZE_OFFSET, stdin) != BMP_HEADER_SIZE_OFFSET){
        perror("fread");
        exit(1);
    }
   
    if(fread(head_size, 1, sizeof(int), stdin) != sizeof(int)){
        perror("fread");
        exit(1);
    }
    
    memcpy(&header_size, head_size, sizeof(int));

    unsigned char* header = malloc(header_size);
    unsigned char remaining_data[header_size - BMP_HEADER_SIZE_OFFSET - sizeof(int)];
    
    if(fread(remaining_data, 1, header_size - BMP_HEADER_SIZE_OFFSET - sizeof(int), stdin) !=
        header_size - BMP_HEADER_SIZE_OFFSET - sizeof(int)){
        perror("fread");
        exit(1);
    }
    //Check if we can even memcpy 
    if(BMP_HEIGHT_OFFSET + sizeof(int) > header_size || BMP_WIDTH_OFFSET + sizeof(int) > header_size){
        fprintf(stderr, "Header size not big enough to store width or height");
        exit(1);
    }
    
    memcpy(&width, &remaining_data[BMP_WIDTH_OFFSET - BMP_HEADER_SIZE_OFFSET - sizeof(int)], sizeof(int));
    memcpy(&height, &remaining_data[BMP_HEIGHT_OFFSET - BMP_HEADER_SIZE_OFFSET - sizeof(int)], sizeof(int)); 
    
    bitmap_ptr->width = width;
    bitmap_ptr->height = height;
    bitmap_ptr->headerSize = header_size;
    

    memcpy(header, initial_data, BMP_HEADER_SIZE_OFFSET);
    memcpy(header + BMP_HEADER_SIZE_OFFSET, head_size, sizeof(int));
    memcpy(header + BMP_HEADER_SIZE_OFFSET + sizeof(int), remaining_data, header_size - BMP_HEADER_SIZE_OFFSET - sizeof(int));

    bitmap_ptr->header = header;
    
    return bitmap_ptr;
}

/*
 * Write out bitmap metadata to stdout.
 * You may add extra fprintf calls to *stderr* here for debugging purposes.
 */
void write_header(const Bitmap *bmp) {
    fwrite(bmp->header, bmp->headerSize, 1, stdout);
}

/*
 * Free the given Bitmap struct.
 */
void free_bitmap(Bitmap *bmp) {
    free(bmp->header);
    free(bmp);
}

/*
 * Update the bitmap header to record a resizing of the image.
 *
 * TODO: complete this function when working on the "scale" filter.
 *
 * Notes:
 *   1. As with read_header, use memcpy and the provided macros in bitmap.h.
 *
 *   2. bmp->header *must* be updated, as this is what's written out
 *      in write_header.
 *
 *   3. You may choose whether or not to also update bmp->width and bmp->height.
 *      This choice may depend on how you implement the scale filter.
 */
void scale(Bitmap *bmp, int scale_factor) {
    bmp->scale_factor = scale_factor;
    int width, height;
    width = bmp->width * scale_factor;
    height = bmp->height * scale_factor;
    int file_size;
    file_size = 3 * width * height + bmp->headerSize;
    memcpy(&bmp->header[BMP_HEIGHT_OFFSET], &height, sizeof(int));
    memcpy(&bmp->header[BMP_WIDTH_OFFSET], &width, sizeof(int));
    memcpy(&bmp->header[BMP_FILE_SIZE_OFFSET], &file_size, sizeof(int));
}


/*
 * The "main" function.
 *
 * Run a given filter function, and apply a scale factor if necessary.
 * You don't need to modify this function to make it work with any of
 * the filters for this assignment.
 */
void run_filter(void (*filter)(Bitmap *), int scale_factor) {
    Bitmap *bmp = read_header();

    if (scale_factor > 1) {
        scale(bmp, scale_factor);
    }

    write_header(bmp);

    // Note: here is where we call the filter function.
    filter(bmp);

    free_bitmap(bmp);
}


/******************************************************************************
 * The gaussian blur and edge detection filters.
 * You should NOT modify any of the code below.
 *****************************************************************************/
const int gaussian_kernel[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}
};

const int kernel_dx[3][3] = {
    {1, 0, -1},
    {2, 0, -2},
    {1, 0, -1}
};

const int kernel_dy[3][3] = {
    {1, 2, 1},
    {0, 0, 0},
    {-1, -2, -1}
};

const int gaussian_normalizing_factor = 16;


Pixel apply_gaussian_kernel(Pixel *row0, Pixel *row1, Pixel *row2) {
    int b = 0, g = 0, r = 0;
    Pixel *rows[3] = {row0, row1, row2};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b += rows[i][j].blue * gaussian_kernel[i][j];
            g += rows[i][j].green * gaussian_kernel[i][j];
            r += rows[i][j].red * gaussian_kernel[i][j];
        }
    }

    b /= gaussian_normalizing_factor;
    g /= gaussian_normalizing_factor;
    r /= gaussian_normalizing_factor;

    Pixel new = {
        .blue = b,
        .green = g,
        .red = r
    };

    return new;
}


Pixel apply_edge_detection_kernel(Pixel *row0, Pixel *row1, Pixel *row2) {
    int b_dx = 0, b_dy = 0;
    int g_dx = 0, g_dy = 0;
    int r_dx = 0, r_dy = 0;
    Pixel *rows[3] = {row0, row1, row2};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            b_dx += rows[i][j].blue * kernel_dx[i][j];
            b_dy += rows[i][j].blue * kernel_dy[i][j];
            g_dx += rows[i][j].green * kernel_dx[i][j];
            g_dy += rows[i][j].green * kernel_dy[i][j];
            r_dx += rows[i][j].red * kernel_dx[i][j];
            r_dy += rows[i][j].red * kernel_dy[i][j];
        }
    }
    int b = floor(sqrt(square(b_dx) + square(b_dy)));
    int g = floor(sqrt(square(g_dx) + square(g_dy)));
    int r = floor(sqrt(square(r_dx) + square(r_dy)));

    int edge_val = max(r, max(g, b));
    Pixel new = {
        .blue = edge_val,
        .green = edge_val,
        .red = edge_val
    };

    return new;
}
