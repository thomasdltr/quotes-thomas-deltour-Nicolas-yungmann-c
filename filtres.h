#ifndef FILTRES_H
#define FILTRES_H

float **createBoxBlurKernel();
float **createGaussianBlurKernel();
float **createOutlineKernel();
float **createEmbossKernel();
float **createSharpenKernel();
void freeKernel(float **kernel);

#endif
