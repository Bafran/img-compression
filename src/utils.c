#include "utils.h"

float **calloc_mat(int dimX, int dimY){
    float **m = calloc(dimX, sizeof(float*));
    float *p = calloc(dimX*dimY, sizeof(float));
    int i;
    for(i=0; i <dimX;i++){
        m[i] = &p[i*dimY];
    }
    return m;
}

void free_mat(float **m){
    free(m[0]);
    free(m);
}