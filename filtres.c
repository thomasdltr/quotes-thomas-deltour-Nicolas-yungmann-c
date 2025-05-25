#include <stdlib.h>
#include "filtres.h"

/**
Crée un noyau de flou moyen (Box Blur)
- Matrice 3x3 avec toutes les valeurs à 1/9
- Permet un floutage uniforme de l'image
*/
float **createBoxBlurKernel() {
    float **kernel = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f; // Normalisation pour que la somme = 1
        }
    }
    return kernel;
}

/**
Crée un noyau de flou gaussien
- Pondération plus forte au centre
- Réduit le bruit tout en préservant mieux les contours
*/
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
            kernel[i][j] = values[i][j] / 16.0f; // Normalisation (somme = 1)
        }
    }
    return kernel;
}

/**
Crée un noyau de détection de contours (Outline)
- Centre positif (8), entourage négatif (-1)
- Met en évidence les transitions de couleur
*/
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

/**
Crée un noyau de relief (Emboss)
- Simule un effet 3D en jouant sur les dégradés
*/
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

/**
Crée un noyau de netteté (Sharpen)
- Renforce les contours en accentuant les contrastes locaux
*/
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

/**
Libère la mémoire d'un noyau de convolution
*/
void freeKernel(float **kernel) {
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
}
