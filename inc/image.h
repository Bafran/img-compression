#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum allocation_type {
    NO_ALLOCATION, SELF_ALLOCATED, STB_ALLOCATED
};

enum colour_space_type {
    MONOCHROME, YCBCR, RGB
};

typedef struct {
    int width;
    int height;
    int channels;
    size_t size;
    uint8_t *data;
    uint8_t *YCbCr_data; // Digital YCbCr (shifted by +128)
    enum colour_space_type colour_space;
    enum allocation_type allocation_;
} Image;

// Use rgb to generate ycbcr values
void generate_ycbcr_data(Image *img);
// Use ycbcr to generate rgb values
void generate_rgb_data(Image *img);

void Image_load(Image *img, const char *fname);
void Image_create(Image *img, int width, int height, int channels, bool zeroed);
void Image_save(const Image *img, const char *fname);
void Image_free(Image *img);

void ycbcr_grayscale(Image *img);
void ycbcr_cb(Image *img);
void ycbcr_cr(Image *img);