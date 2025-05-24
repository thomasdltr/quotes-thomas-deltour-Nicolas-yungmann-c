#include <stdlib.h>
#include "filtres.h"

float **createBoxBlurKernel() {
    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }
    return kernel;
}

float **createGaussianBlurKernel() {
    float values[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };
    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = values[i][j] / 16.0f;
        }
    }
    return kernel;
}

float **createOutlineKernel() {
    int values[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };
    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = (float)values[i][j];
        }
    }
    return kernel;
}

float **createEmbossKernel() {
    int values[3][3] = {
        {-2, -1, 0},
        {-1,  1, 1},
        { 0,  1, 2}
    };
    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = (float)values[i][j];
        }
    }
    return kernel;
}

float **createSharpenKernel() {
    int values[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = (float)values[i][j];
        }
    }
    return kernel;
}

void freeKernel(float **kernel) {
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
}



