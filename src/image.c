#include "image.h"
#include "utils.h"
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

/* Right now this uses the stbi image library to decode a jpeg file, then further compress it using compression.h,
 * this program will hopefully be changed to get rid of Image.h and stbi entirely, and work with a raw RGB stream instead of
 * needing to decode a jpeg first before doing further compression */
void Image_load(Image *img, const char *fname) {
    if((img->data = stbi_load(fname, &img->width, &img->height, &img->channels, 0)) != NULL) {
        img->size = img->width * img->height * img->channels;
        img->allocation_ = STB_ALLOCATED;
        img->colour_space = RGB;
        img->YCbCr_data = calloc(img->size, 1);
    }
}

void Image_free(Image *img) {
    if(img->allocation_ != NO_ALLOCATION && img->data != NULL) {
        if(img->allocation_ == STB_ALLOCATED) {
            stbi_image_free(img->data);
        } else {
            free(img->data);
            free(img->YCbCr_data);
        }
        img->data = NULL;
        img->width = 0;
        img->height = 0;
        img->size = 0;
        img->allocation_ = NO_ALLOCATION;
    }
}

void generate_ycbcr_data(Image *img) {
    ON_ERROR_EXIT(!(img->allocation_ != NO_ALLOCATION && img->channels == 3), "The input image must have 3 channels.");

    for(unsigned char *p = img->data, *pg = img->YCbCr_data; p != img->data + img->size; p += img->channels, pg += img->channels) {
        *pg       = (uint8_t)( 0.257   * *p + 0.504  * *(p + 1) + 0.098   * *(p + 2)) + 16;          // Y
        *(pg + 1) = (uint8_t)(-0.148   * *p - 0.291  * *(p + 1) + 0.439   * *(p + 2)) + 128;         // Cb
        *(pg + 2) = (uint8_t)( 0.439   * *p - 0.368  * *(p + 1) - 0.071   * *(p + 2)) + 128;         // Cr
    }
}

/* Used for testing earlier to make sure the colour space conversions were working correctly - Same values should be
 * used in the decoder to convert back to RGB */
void generate_rgb_data(Image *img) {
    ON_ERROR_EXIT(!(img->allocation_ != NO_ALLOCATION && img->channels == 3), "The input image must have 3 channels.");

    for(unsigned char *p = img->YCbCr_data, *pg = img->data; p != img->YCbCr_data + img->size; p += img->channels, pg += img->channels) {
        *pg       = (uint8_t)(1.164*(*p - 16) + 1.596*(*(p + 2) - 128));         // r
        *(pg + 1) = (uint8_t)(1.164*(*p - 16) - 0.813*(*(p + 2) - 128) - 0.392*(*(p + 1) - 128));         // g
        *(pg + 2) = (uint8_t)(1.164*(*p - 16) + 2.017*(*(p + 1) - 128));         // b
    }
}


