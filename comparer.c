#include <stdio.h>
#include <stdlib.h>
#include "bmp_info.h"
#define error(...) (fprintf(stderr, __VA_ARGS__))
int main(int argc, char *argv[]) {
    int bit_count1 = return_bits_per_pixel(argv[1]);
    int bit_count2 = return_bits_per_pixel(argv[2]);
    if (bit_count1 != bit_count2) {
        error("In file %s %d bits per pixel but in file %s %d", argv[1], bit_count1, argv[2], bit_count2);
        return 1;
    } else {
        if (bit_count1 == 8) {
            BMP_8 *image1 = malloc(sizeof(*image1));
            BMP_8 *image2 = malloc(sizeof(*image2));
            int loading_result1 = load_bmp_8(image1, argv[1]);
            int loading_result2 = load_bmp_8(image2, argv[2]);
            if (loading_result1 != 0) {
                return loading_result1;
            }
            if (loading_result2 != 0) {
                return loading_result2;
            }
            int compare_result = compare_bmp8(image1, image2);
            if (compare_result != 0) {
                return compare_result;
            }
            free(image1);
            free(image2);
        } else if (bit_count1 == 24) {
            BMP_24 *image1 = malloc(sizeof(*image1));
            BMP_24 *image2 = malloc(sizeof(*image2));
            int loading_result1 = load_bmp_24(image1, argv[1]);
            int loading_result2 = load_bmp_24(image2, argv[2]);
            if (loading_result1 != 0) {
                return loading_result1;
            }
            if (loading_result2 != 0) {
                return loading_result2;
            }
            int compare_result = compare_bmp24(image1, image2);
            if (compare_result != 0) {
                return compare_result;
            }
        }
        free(image1);
        free(image2);
    }
    return 0;
}


