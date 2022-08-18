#include <stdio.h>
#include "stdlib.h"
#include "compression.h"
#include "image.h"
#include "config.h"
#include "stb_image/stb_image.h"

#define DEV 0

int main() {


    if(DEV){
        uint8_t *data;
        int width;
        int height;
        int channels;

        if((data = stbi_load(FILE_IN, &width, &height, &channels, 0)) == NULL) return 1;
        if(channels != 3) return 1;

        compression_init();

        for (int i = 0; i < width*height/(BLOCK_SIZE*BLOCK_SIZE); ++i) {
            compress_MCU(data+(i*BLOCK_SIZE*BLOCK_SIZE));
        }

        compression_exit();
    } else {
        Image img;
        Image_load(&img, FILE_IN);
        compress_image(&img);
        Image_free(&img);
    }

    return 0;
}
