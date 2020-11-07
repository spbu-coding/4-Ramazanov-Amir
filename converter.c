#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_info.h"
#include "qdbmp.h"

int main(int argc, char *argv[]) {
    PARAMETER_STRUCT parameters;
    int reading_result = read_arguments(argc, argv, &parameters);
    if (reading_result != 0) {
        return reading_result;
    }
    char src_path[105];
    sprintf(src_path, "%s%s", "..\\", parameters.src_file);
    char dst_path[105];
    sprintf(dst_path, "%s%s", "..\\", parameters.dst_file);
    if (strcmp(parameters.code_owner, "--mine") == 0) {
        if (strcmp(parameters.bits_per_pixel, "--b24") == 0) {
            BMP_24 *image = malloc(sizeof(*image));
            int loading_result = load_bmp_24(image, src_path);
            if (loading_result != 0) {
                return loading_result;
            }
            int writing_result = write_negative_bmp_24(image, parameters.dst_file);
            if (writing_result != 0) {
                return writing_result;
            }
        } else {
            BMP_8 *image = malloc(sizeof(*image));
            int loading_result = load_bmp_8(image, src_path);
            if (loading_result != 0) {
                return loading_result;
            }
            int writing_result = write_negative_bmp_8(image, parameters.dst_file);
            if (writing_result != 0) {
                return writing_result;
            }
        }
    }
    return 0;
}