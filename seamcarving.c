#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "seamcarving.h"
#include "c_img.h"
#include <float.h>
#include <string.h>
#include <ctype.h>
/*
struct rgb_img{
    uint8_t *raster;
    size_t height;
    size_t width;
};
 */

void calc_energy(struct rgb_img *im, struct rgb_img **grad) {
    // Function parameters:
        // im, of type rgb_img* -> pointer to an image struct
        // grad, of type rgb_img** -> pointer to a pointer to an image struct

    // Compute the dual-gradient energy function of the im struct
    // Place the energy function in *grad

    create_img(grad, im->height, im->width);

    int height = im->height;
    int width = im->width;
    // In image processing, pixel (y, x) refers to the pixel in column x and row y
    // pixel (0,0) at the upper-left corner
    // pixel (H-1, W-1) at the lower-right corner

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int x_post, x_pre, y_post, y_pre;

            if (y == 0) {
                y_pre = height - 1;
                y_post = y + 1;
            } else if (y == height - 1) {
                y_pre = y - 1;
                y_post = 0;
            } else {
                y_pre = y-1;
                y_post = y+1;
            }

            if (x == 0) {
                x_pre = width - 1;
                x_post = x + 1;
            } else if (x == width - 1) {
                x_pre = x - 1;
                x_post = 0;
            } else {
                x_pre = x-1;
                x_post = x+1;
            }

            int Rx = get_pixel(im, y, x_post, 0) - get_pixel(im, y, x_pre, 0);
            int Gx = get_pixel(im, y, x_post, 1) - get_pixel(im, y, x_pre, 1);
            int Bx = get_pixel(im, y, x_post, 2) - get_pixel(im, y, x_pre, 2);

            int Ry = get_pixel(im, y_post, x, 0) - get_pixel(im, y_pre, x, 0);
            int Gy = get_pixel(im, y_post, x, 1) - get_pixel(im, y_pre, x, 1);
            int By = get_pixel(im, y_post, x, 2) - get_pixel(im, y_pre, x, 2);

            int res = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2) + pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);
            uint8_t energy = (uint8_t)(pow(res, 0.5) / 10);

            set_pixel(*grad, y, x, energy, energy, energy);

        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr) {
    // Function parameters:
        // grad, of type rgb_img* -> pointer to an image struct
        // best_arr, of type double** -> pointer to a pointer to a double

    // allocate and compute the dynamic array *best_arr
    // refer to the houses.py algorithm to make the matrix with the min cost

    int height = grad->height;
    int width = grad->width;
    *best_arr = (double*)malloc(sizeof(double) * height * width);
    int k = 0;

    for (int j = 0; j < width; j++) {
        (*best_arr)[0 + j] = get_pixel(grad, 0, j, 0);
    }
    int low, a, b, c;

    for (int y = 1; y < height; y++) {
        for (int x = 0; x < width; x++) {
            k++;
            a = (*best_arr)[(y-1)*width+x];

            if (x == 0) {
                b = 10000000;
            } else {
                b = (*best_arr)[(y-1)*width+(x-1)];
            }

            if (x == width-1) {
                c = 10000000;
            } else {
                c = (*best_arr)[(y-1)*width+(x+1)];
            }

            if (a <= b) {
                if (a <= c) {
                    low = a;
                } else {
                    low = c;
                }
            } else {
                if (b <= c) {
                    low = b;
                } else {
                    low = c;
                }
            }

            (*best_arr)[y*width+x] = get_pixel(grad, y, x, 0) + low;
        }
    }


}

void recover_path(double *best, int height, int width, int **path) {
    // Function parameters:
        // best, of type double* -> pointer to a double
        // height and width -> int
        // path, of type int** -> pointer to a pointer to an int

    // allocate memory in path
    // grab the index for the lowest number at each row
    // add to the path

    *path = (int*)malloc(sizeof(int) * height);
    int k = 0;

    for (int y = 0; y < height; y++) {
        double cur_min = best[y*width + 0];
        int cur_ind = 0;
        for (int x = 1; x < width; x++) {
            if (best[y*width+x] < cur_min) {
                cur_min = best[y*width+x];
                cur_ind = x;
            }
        }

        (*path)[k] = cur_ind;
        k++;
    }

}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path) {
    // Function parameters:
        // src, of type rgb_img* -> pointer to an image struct
        // dest, of type rgb_img** -> pointer to a pointer to an image struct
        // path, of type int* -> pointer to an int

    // create the destination image
    // write the source image to it, with the seam removed
    create_img(dest, (src->height), (src->width)-1);

    for (int y = 0; y < (*dest)->height; y++) {
        int k = 0;
        for (int x = 0; x < (*dest)->width; x++) {
            if (k == path[y]) {
                k++;
            }

            int r = get_pixel(src, y, k, 0);
            int g = get_pixel(src, y, k, 1);
            int b = get_pixel(src, y, k, 2);

            set_pixel(*dest, y, x, r, g, b);
            k++;

        }
    }

}

