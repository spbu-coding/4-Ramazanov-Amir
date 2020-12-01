#include "bmp_info.h"

int r_bitmapfileheader(BITMAPFILEHEADER *bitmapfileheader, FILE *x) {
    if (fread(bitmapfileheader, 14, 1, x) != 1) {
        error("Some bytes of BITMAPFILEHEADER were not read");
        return BYTES_READING_ERROR;
    }
    if (bitmapfileheader->bfType != 0x4d42) {
        error("Error in type of bmp file");
        return FILE_TYPE_ERROR;
    }
    if (bitmapfileheader->bfReserved1 != 0x00 || bitmapfileheader->bfReserved2 != 0x00) {
        error("Something was written to reserved nulls at 06-0A");
        return RESERVED_NULLS_ERROR;
    }
    return 0;
}
int r_bitmapinfoheader(BITMAPINFOHEADER *bitmapinfoheader, FILE *x) {
    if (fread(bitmapinfoheader, 40, 1, x) != 1) {
        error("Some bytes of BITMAPINFOHEADER were not read");
        return BYTES_READING_ERROR;
    }
    if (bitmapinfoheader->biSize != 0x28) {
        error("Wrong version of file (must be 3)");
        return VERSION_ERROR;
    }
    if (bitmapinfoheader->biWidth <= 0x00) {
        error("Error in width (position 12, must be greater 0)");
        return WIDTH_ERROR;
    }
    if (bitmapinfoheader->biHeight == 0x00) {
        error("Error in height (position 16, must not be 0)");
        return HEIGHT_ERROR;
    }
    if (bitmapinfoheader->biPlanes != 0x01) {
        error("Field biPlane is not equal 1");
        return PLANES_ERROR;
    }
    return 0;
}
int load_bmp_24(BMP_24 *image, char const *Filename) {
    FILE *input = fopen(Filename, "rb");
    if (input == NULL) {
        error("Can'n open file %s\n", Filename);
        return FILE_OPEN_ERROR;
    }
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    int correctness_file_header = r_bitmapfileheader(&file_header, input);
    if (correctness_file_header != 0) {
        return correctness_file_header;
    }
    int correctness_info_header = r_bitmapinfoheader(&info_header, input);
    if (correctness_info_header != 0) {
        return correctness_info_header;
    }
    PIXEL **data = (PIXEL **)malloc(sizeof(PIXEL *) * info_header.biHeight);
    if (data == NULL) {
        error("Can'n allocate memory for pixels data");
        return MEMORY_ALLOCATION_ERROR;
    }
    for (int i = 0; i < info_header.biWidth; ++i) {
        data[i] = (PIXEL *)malloc(sizeof(PIXEL) * info_header.biWidth);
        if (data[i] == NULL) {
            error("Cannot allocate memory for %d-th pixel row", i);
            return MEMORY_ALLOCATION_ERROR;
        }
    }
    for (int i = 0; i < info_header.biHeight; ++i) {
        for (int j = 0; j < info_header.biWidth; ++j) {
            if (fread(&data[i][j], sizeof(PIXEL), 1, input) != 1) {
                error("Cannot read pixel");
                return BYTES_READING_ERROR;
            }
        }
    }
    image->file_header = file_header;
    image->info_header = info_header;
    image->data = data;
    fclose(input);
    return 0;
}
int write_negative_bmp_24(BMP_24 *image, char const *Filename) {
    FILE *output = fopen(Filename, "wb");
    if (output == NULL) {
        error("Can'n open file %s", Filename);
        return FILE_OPEN_ERROR;
    }
    if (fwrite(&image->file_header, sizeof(BITMAPFILEHEADER), 1, output) != 1) {
        error("Some bytes of BITMAPFILEHEADER were not written");
        exit(BYTES_READING_ERROR);
    }
    if (fwrite(&image->info_header, sizeof(BITMAPINFOHEADER), 1, output) != 1) {
        error("Some bytes of BITMAPINFOHEADER were not written");
        exit(BYTES_READING_ERROR);
    }
    PIXEL negative_pixel;
    for (int i = 0; i < image->info_header.biHeight; ++i) {
        for (int j = 0; j < image->info_header.biWidth; ++j) {
            negative_pixel.r = ~image->data[i][j].r;
            negative_pixel.g = ~image->data[i][j].g;
            negative_pixel.b = ~image->data[i][j].b;
            if (fwrite(&negative_pixel, sizeof(BYTE), 3, output) != 3) {
                error("Some bytes of pixels data were not written");
                return BYTES_READING_ERROR;
            }
        }
    }
    return 0;
}
int load_bmp_8(BMP_8 *image, char const *Filename) {
    FILE *input = fopen(Filename, "rb");
    if (input == NULL) {
        error("Can'n open file %s", Filename);
        return FILE_OPEN_ERROR;
    }
    if (image == NULL) {
        error("Can'n allocate memory for loading image");
        return MEMORY_ALLOCATION_ERROR;
    }
    int correctness_file_header = r_bitmapfileheader(&image->file_header, input);
    if (correctness_file_header != 0) {
        return correctness_file_header;
    }
    int correctness_info_header = r_bitmapinfoheader(&image->info_header, input);
    if (correctness_info_header != 0) {
        return correctness_info_header;
    }
    image->palette = malloc(sizeof(PIXEL) * image->info_header.biClrUsed);
    if (image->palette == NULL) {
        error("Can'n allocate memory for palette");
        return MEMORY_ALLOCATION_ERROR;
    }
    image->data = malloc(sizeof(PIXEL *) * image->info_header.biHeight);
    if (image->data == NULL) {
        error("Can'n allocate memory for pixels data");
        return MEMORY_ALLOCATION_ERROR;
    }
    for (int i = 0; i < image->info_header.biHeight; ++i) {
        image->data[i] = malloc(sizeof(PIXEL) * image->info_header.biWidth);
        if (image->data[i] == NULL) {
            error("Can'n allocate memory for %d-th pixel row", i);
            return MEMORY_ALLOCATION_ERROR;
        }
    }
    BYTE zero_byte;
    for (int i = 0; i < image->info_header.biClrUsed; ++i) {
        if (fread(&image->palette[i], sizeof(BYTE), 3, input) != 3) {
            error("Some bytes of palette were not read");
            return BYTES_READING_ERROR;
        }
        if (fread(&zero_byte, sizeof(BYTE), 1, input) != 1) {
            error("0x00 byte after %d-th node in palette was nor read", i);
            return BYTES_READING_ERROR;
        }
    }
    for (int i = 0; i < image->info_header.biHeight; ++i) {
        for  (int j = 0; j < image->info_header.biWidth; ++j) {
            if (fread(&image->data[i][j], sizeof(BYTE), 1, input) != 1) {
                error("Some bytes of pixels data were not read");
                return BYTES_READING_ERROR;
            }
        }
    }
    fclose(input);
    return 0;
}
int write_negative_bmp_8(BMP_8 *image, char const *Filename) {
    BYTE zero_byte = 0x00;
    FILE *output = fopen(Filename, "wb");
    if (output == NULL) {
        error("Cannot open file %s", Filename);
        return FILE_OPEN_ERROR;
    }
    if (fwrite(&image->file_header, sizeof(BITMAPFILEHEADER), 1, output) != 1) {
        error("Some bytes of BITMAPFILEHEADER were not written");
        return BYTES_READING_ERROR;
    }
    if (fwrite(&image->info_header, sizeof(BITMAPINFOHEADER), 1, output) != 1) {
        error("Some bytes of BITMAPINFOHEADER were not written");
        return BYTES_READING_ERROR;
    }
    for (int i = 0; i < image->info_header.biClrUsed; ++i) {
        if (fwrite(&image->palette[i], sizeof(PIXEL), 1, output) != 1) {
            error("Some bytes of palette were not written");
            return BYTES_READING_ERROR;
        }
        if (fwrite(&zero_byte, sizeof(BYTE), 1, output) != 1) {
            error("0x00 byte after %d-th node in palette was nor written", i);
            return BYTES_READING_ERROR;
        }
    }
    for (int i = 0; i < image->info_header.biHeight; ++i) {
        for (int j = 0; j < image->info_header.biWidth; ++j) {
            BYTE tmp_byte = ~image->data[i][j];
            if (fwrite(&tmp_byte, sizeof(BYTE), 1, output) != 1) {
                error("Some bytes of pixels data were not written");
                return BYTES_READING_ERROR;
            }
        }
    }
    fclose(output);
    return 0;
}
    int return_bits_per_pixel(char const *filename) {
        FILE *input = fopen(filename, "rb");
        if (input == NULL) {
            return -1;
        }
        fseek(input, 28, SEEK_SET);
        int bits = 0;
        fread(&bits, 1, 2, input);
        fseek(input, 0, SEEK_SET);
        fclose(input);
        return bits;
    }

    int compare_pixels(PIXEL a, PIXEL b) {
        return (a.r - b.r || a.b - b.b || a.g - b.g);
    }

    int compare_bmp8(BMP_8 *image1, BMP_8 *image2) {
        LONG width1 = image1->info_header.biWidth;
        LONG height1 = image1->info_header.biHeight;
        LONG width2 = image2->info_header.biWidth;
        LONG height2 = image2->info_header.biHeight;
        if (width1 != width2) {
            error("Images have different width");
            return -1;
        }
        if (height1 != height2) {
            error("Images have different height");
            return -1;
        }
        BYTE **data1 = image1->data;
        BYTE **data2 = image2->data;
        int deviation_counter = 0;
        for (int i = 0; i < height1; ++i) {
            for (int j = 0; j < width1; ++j) {
                if (deviation_counter < 100) {
                    if (data1[i][j] != data2[i][j]) {
                        deviation_counter++;
                        printf("%d %d", data1[i][j], data2[i][j]);
                    }
                }
            }
        }
        return 0;
    }

    int compare_bmp24(BMP_24 *image1, BMP_24 *image2) {
        LONG width1 = image1->info_header.biWidth;
        LONG height1 = image1->info_header.biHeight;
        LONG width2 = image2->info_header.biWidth;
        LONG height2 = image2->info_header.biHeight;
        if (width1 != width2) {
            error("Images have different width");
            return -1;
        }
        if (height1 != height2) {
            error("Images have different height");
            return -1;
        }
        PIXEL **data1 = image1->data;
        PIXEL **data2 = image2->data;
        int deviation_counter = 0;
        for (int i = 0; i < height1; ++i) {
            for (int j = 0; j < width1; ++j) {
                if (deviation_counter < 100) {
                    if (compare_pixels(data1[i][j], data2[i][j])) {
                        deviation_counter++;
                        printf("%d %d\n", i, j);
                    }
                }
            }
        }
        return 0;
    }
    int read_arguments(int argc, char *argv[], PARAMETER_STRUCT *parameters) {
        if (argc != NORMAL_PARAMETER_COUNT) {
            error("Wrong count of parameters");
            return PARAMETER_COUNT_ERROR;
        }
        if (strcmp(argv[1], "--b24") != 0 && strcmp(argv[1], "--b8") != 0) {
            error("Wrong first parameter");
            return FIRST_PARAMETER_ERROR;
        }
        strncpy(parameters->bits_per_pixel, argv[1], strlen(argv[0]));
        if (strcmp(argv[2], "--mine") != 0 && strcmp(argv[2], "--theirs") != 0) {
            error("Wrong second parameter");
            return SECOND_PARAMETER_ERROR;
        }
        strncpy(parameters->code_owner, argv[2], strlen(argv[2]));
        if (strstr(argv[3], ".bmp") == NULL && strstr(argv[3], ".BMP") == NULL) {
            error("Wrong third parameter");
            return THIRD_PARAMETER_ERROR;
        }
        strncpy(parameters->src_file, argv[3], strlen(argv[3]));
        if (strstr(argv[4], ".bmp") == NULL) {
            error("Wrong fourth parameter");
            return FOURTH_PARAMETER_ERROR;
        }
        strncpy(parameters->dst_file, argv[4], strlen(argv[4]));
        return 0;
    }
