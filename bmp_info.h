#ifndef GH_BMP_INFO_H
#define GH_BMP_INFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qdbmp.h"

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define LONG int

#define FILE_TYPE_ERROR 1
#define RESERVED_NULLS_ERROR 2
#define VERSION_ERROR 3
#define WIDTH_ERROR 4
#define HEIGHT_ERROR 5
#define PLANES_ERROR 6
#define BYTES_READING_ERROR 8
#define MEMORY_ALLOCATION_ERROR 9
#define FILE_OPEN_ERROR 10

#define FIRST_PARAMETER_ERROR 1
#define SECOND_PARAMETER_ERROR 2
#define THIRD_PARAMETER_ERROR 3
#define FOURTH_PARAMETER_ERROR 4
#define PARAMETER_COUNT_ERROR 5
#define NORMAL_PARAMETER_COUNT 5

#define error(...) (fprintf(stderr, __VA_ARGS__))

typedef struct {
    BYTE r;
    BYTE g;
    BYTE b;
} PIXEL;

#pragma pack(push, 1)
typedef struct {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER;
#pragma pack(pop)

typedef struct {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    PIXEL **data;
} BMP_24;

typedef struct {
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    PIXEL *palette;
    BYTE **data;
} BMP_8;

typedef struct {
    char bits_per_pixel[10];
    char code_owner[10];
    char src_file[100];
    char dst_file[100];
} PARAMETER_STRUCT;

int r_bitmapfileheader(BITMAPFILEHEADER *bitmapfileheader, FILE *fin);
int r_bitmapinfoheader(BITMAPINFOHEADER *bitmapinfoheader, FILE *fin);
int load_bmp_24(BMP_24 *image, char const *Filename);
int write_negative_bmp_24(BMP_24 *image, char const *Filename);
int load_bmp_8(BMP_8 *image, char const *filename);
int write_negative_bmp_8(BMP_8 *image, char const *filename);
int return_bits_per_pixel(char const *filename);
int compare_pixels(PIXEL a, PIXEL b);
int compare_bmp8(BMP_8 *image1, BMP_8 *image2);
int compare_bmp24(BMP_24 *image1, BMP_24 *image2);
int read_arguments(int argc, char *argv[], PARAMETER_STRUCT *parameters);

#endif //GH_BMP_INFO_H