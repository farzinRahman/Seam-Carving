#include <stdio.h>
#include "seamcarving.h"


int main() {
    printf("Running\n");
    struct rgb_img* im;
    read_in_img(&im, "3x4.bin");
    // print_grad(im);

    struct rgb_img* grad;
    calc_energy(im, &grad);
    print_grad(grad);
    printf("--------------------\n");

    struct rgb_img* im1;
    read_in_img(&im1, "6x5.bin");
    // print_grad(im1);
    struct rgb_img* grad1;
    calc_energy(im1, &grad1);
    print_grad(grad1);
    printf("--------------------\n");

    double* best_arr;
    dynamic_seam(grad1, &best_arr);

    int* path;
    recover_path(best_arr, grad1->height, grad1->width, &path);
    printf("[");
    for (int i = 0; i < grad1->height; i++) {
        printf("%d, ", path[i]);
    }
    printf("]\n");

    struct rgb_img* dest;
    remove_seam(grad1, &dest, path);
    print_grad(dest);

    return 0;
}
