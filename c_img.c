#include "c_img.h"
#include <stdio.h>
#include <math.h>

void create_img(struct rgb_img **im, size_t height, size_t width){
    // create an image struct

    // Function parameters:
        // im, of type rgb_img** -> pointer to a pointer to a struct
        // height, of type size_t
        // width, of type size_t

    // malloc space to store one rgb_img struct
    *im = (struct rgb_img *)malloc(sizeof(struct rgb_img));

    // set height to height
    (*im)->height = height;

    // set width to width
    (*im)->width = width;

    // malloc space for the raster element in the struct
    (*im)->raster = (uint8_t *)malloc(3 * height * width);
}


int read_2bytes(FILE *fp){
    // read 2 bytes from a given file and return it

    // Function parameters:
        // fp, of type FILE*

    // make an empty array to store 2 elements of type uint8_t
    uint8_t bytes[2];

    // read in 2 elements of size uint8_t from fp, and store in the array bytes
    fread(bytes, sizeof(uint8_t), 1, fp);
    fread(bytes+1, sizeof(uint8_t), 1, fp);

    // a << b = a * 2^b
    // return bytes[0] * 256 + bytes[1]
    return (  ((int)bytes[0]) << 8)  + (int)bytes[1];
}

void write_2bytes(FILE *fp, int num){
    // write 2 bytes into a given file

    // Function parameters:
        // fp, of type FILE*
        // num, of type int

    // make an empty array to store 2 elements of type uint8_t
    uint8_t bytes[2];

    // a >> b = a / (2^b)
    // b >> 8 shifts the variable b to the right by 8 bits
    // a & b = an int with a bit pattern that has
        // 0 in all positions where b has a 0
        // all positions where b has a 1, corresponding bit value from a is used
    // used to transform color values from a special format
    // to standard RGB values which are 8 bits long

    // transform the num into RGB color value, and insert in the bytes array
    bytes[0] = (uint8_t)((num & 0XFFFF) >> 8);
    bytes[1] = (uint8_t)(num & 0XFF);

    // write the bytes into the filestream
    fwrite(bytes, 1, 1, fp);
    fwrite(bytes+1, 1, 1, fp);
}

void read_in_img(struct rgb_img **im, char *filename){
    // read from the given file and add the image data to the struct

    // Function parameters:
        // im, of type rgb_img** -> pointer to a pointer to a struct
        // filename, of type char*

    // The mode string can also include the letter “b” after either the “+” or
    // the first letter.  This is strictly for compatibility with ISO/IEC
    // 9899:1990 (“ISO C90”) and has effect only for fmemopen() ; otherwise “b”
    // is ignored

    // open the file
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) {
        printf("File read unsuccessful");
    }

    // read 2 bytes from fp, and that's the height of the image
    size_t height = read_2bytes(fp);

    // read 2 more bytes from fp, and that's the width of the image
    size_t width = read_2bytes(fp);

    // create an image struct with the height and width
    // and put it in im
    create_img(im, height, width);

    // read 3*width*height bytes from the filestream
    // and store it in the raster element of the struct
    fread((*im)->raster, 1, 3*width*height, fp);
    fclose(fp);
}

void write_img(struct rgb_img *im, char *filename){
    // read from the given struct, and write the image data into the file

    // Function parameters:
        // im, of type rgb_img* -> pointer to a rgb_img struct
        // filename, of type char*

    // open the file in write mode
    FILE *fp = fopen(filename, "wb");

    // write 2 bytes into the filestream
    // by taking the height and width from the given struct
    write_2bytes(fp, im->height);
    write_2bytes(fp, im->width);

    // from the raster element in the struct, write into the filestream
    fwrite(im->raster, 1, im->height * im->width * 3, fp);
    fclose(fp);
}

uint8_t get_pixel(struct rgb_img *im, int y, int x, int col){
    // get the pixel value from the im struct at the specified location

    // Function parameters:
        // im, of type rgb_img* -> pointer to an rgb_img struct
        // x, y, col, of type int

    // return the pixel at the specified location in the im struct
    // stored in raster
    return im->raster[3 * (y*(im->width) + x) + col];
}

void set_pixel(struct rgb_img *im, int y, int x, int r, int g, int b){
    // set the pixel value at the specified location in the im struct

    // Function parameters:
        // im, of type rgb_img* -> pointer to an rgb_img struct
        // x, y, and r, g, b, of type int

    // set the pixel value at the specified location with the r, g, b values
    // store in the raster element of the im struct
    im->raster[3 * (y*(im->width) + x) + 0] = r;
    im->raster[3 * (y*(im->width) + x) + 1] = g;
    im->raster[3 * (y*(im->width) + x) + 2] = b;
}

void destroy_image(struct rgb_img *im){
    // free the memory blocks allocated to raster and its im struct
    free(im->raster);
    free(im);
}


void print_grad(struct rgb_img *grad){
    // print the im struct

    // Function parameters:
        // grad, of type rgb_img* -> pointer to an rgb_img struct

    // make a local variable to store the height and width
    int height = grad->height;
    int width = grad->width;

    // double for loop to print the contents stored in the im struct
        // separated by tabs
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("%d\t", get_pixel(grad, i, j, 0));
        }
    printf("\n");    
    }
}
