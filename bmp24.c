#include "bmp24.h"
#include <stdlib.h>


void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}



t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
    if (!pixels) return NULL;

    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (!pixels[i]) {
            for (int j = 0; j < i; j++) free(pixels[j]);
            free(pixels);
            return NULL;
        }
    }
    return pixels;
}

void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (!pixels) return;
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    img->data = bmp24_allocateDataPixels(width, height);
    if (!img->data) {
        free(img);
        return NULL;
    }
    return img;
}

void bmp24_free(t_bmp24 *img) {
    if (!img) return;
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}




t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : impossible d’ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // Lire largeur, hauteur et profondeur manuellement
    int32_t width, height;
    uint16_t bits;
    fseek(file, BITMAP_WIDTH, SEEK_SET);
    fread(&width, sizeof(int32_t), 1, file);
    fread(&height, sizeof(int32_t), 1, file);
    fseek(file, BITMAP_DEPTH, SEEK_SET);
    fread(&bits, sizeof(uint16_t), 1, file);

    if (bits != 24) {
        printf("Erreur : image non 24 bits (%d bits détectés)\n", bits);
        fclose(file);
        return NULL;
    }

    // Allouer la structure
    t_bmp24 *img = bmp24_allocate(width, height, bits);
    if (!img) {
        fclose(file);
        return NULL;
    }

    // Lire les en-têtes
    file_rawRead(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawRead(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    // Lire les données (avec fonctions demandées)
    bmp24_readPixelData(img, file);

    fclose(file);
    return img;
}



void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint8_t bgr[3];
    fread(bgr, sizeof(uint8_t), 3, file);
    image->data[y][x].blue  = bgr[0];
    image->data[y][x].green = bgr[1];
    image->data[y][x].red   = bgr[2];
}

void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    fseek(file, image->header.offset, SEEK_SET);
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}

void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint8_t bgr[3];
    bgr[0] = image->data[y][x].blue;
    bgr[1] = image->data[y][x].green;
    bgr[2] = image->data[y][x].red;
    fwrite(bgr, sizeof(uint8_t), 3, file);
}

void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    fseek(file, image->header.offset, SEEK_SET);
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}



void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}


void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : impossible d’ouvrir le fichier %s en écriture\n", filename);
        return;
    }

    // Écriture des en-têtes BMP
    file_rawWrite(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawWrite(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    // Écriture des données de pixels
    bmp24_writePixelData(img, file);

    fclose(file);
    printf("Image sauvegardée dans %s\n", filename);
}

void bmp24_negative(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red   = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue  = 255 - img->data[y][x].blue;
        }
    }
}


void bmp24_grayscale(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            uint8_t r = img->data[y][x].red;
            uint8_t g = img->data[y][x].green;
            uint8_t b = img->data[y][x].blue;
            uint8_t gray = (r + g + b) / 3;
            img->data[y][x].red = img->data[y][x].green = img->data[y][x].blue = gray;
        }
    }
}


void bmp24_brightness(t_bmp24 *img, int value) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int r = img->data[y][x].red + value;
            int g = img->data[y][x].green + value;
            int b = img->data[y][x].blue + value;
            img->data[y][x].red   = (r > 255) ? 255 : (r < 0 ? 0 : r);
            img->data[y][x].green = (g > 255) ? 255 : (g < 0 ? 0 : g);
            img->data[y][x].blue  = (b > 255) ? 255 : (b < 0 ? 0 : b);
        }
    }
}




t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    int offset = kernelSize / 2;
    float r = 0.0, g = 0.0, b = 0.0;

    for (int i = -offset; i <= offset; i++) {
        for (int j = -offset; j <= offset; j++) {
            int xi = x + j;
            int yi = y + i;

            if (xi >= 0 && xi < img->width && yi >= 0 && yi < img->height) {
                r += img->data[yi][xi].red   * kernel[i + offset][j + offset];
                g += img->data[yi][xi].green * kernel[i + offset][j + offset];
                b += img->data[yi][xi].blue  * kernel[i + offset][j + offset];
            }
        }
    }

    t_pixel result;
    result.red   = (r > 255) ? 255 : (r < 0 ? 0 : (uint8_t)(r + 0.5));
    result.green = (g > 255) ? 255 : (g < 0 ? 0 : (uint8_t)(g + 0.5));
    result.blue  = (b > 255) ? 255 : (b < 0 ? 0 : (uint8_t)(b + 0.5));

    return result;
}


void bmp24_boxBlur(t_bmp24 *img) {
    float **kernel = createBoxBlurKernel();
    t_bmp24 *tmp = bmp24_allocate(img->width, img->height, img->colorDepth);
    if (!tmp) return;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            tmp->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }

    // recopier dans l’image originale
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = tmp->data[y][x];
        }
    }

    bmp24_free(tmp);
    freeKernel(kernel);
}

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

void freeKernel(float **kernel) {
    for (int i = 0; i < 3; i++) {
        free(kernel[i]);
    }
    free(kernel);
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

void bmp24_gaussianBlur(t_bmp24 *img) {
    float **kernel = createGaussianBlurKernel();
    t_bmp24 *tmp = bmp24_allocate(img->width, img->height, img->colorDepth);
    if (!tmp) {
        freeKernel(kernel);
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            tmp->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = tmp->data[y][x];
        }
    }

    bmp24_free(tmp);
    freeKernel(kernel);
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

void bmp24_outline(t_bmp24 *img) {
    float **kernel = createOutlineKernel();
    t_bmp24 *tmp = bmp24_allocate(img->width, img->height, img->colorDepth);
    if (!tmp) {
        freeKernel(kernel);
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            tmp->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = tmp->data[y][x];
        }
    }

    bmp24_free(tmp);
    freeKernel(kernel);
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

void bmp24_emboss(t_bmp24 *img) {
    float **kernel = createEmbossKernel();
    t_bmp24 *tmp = bmp24_allocate(img->width, img->height, img->colorDepth);
    if (!tmp) {
        freeKernel(kernel);
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            tmp->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = tmp->data[y][x];
        }
    }

    bmp24_free(tmp);
    freeKernel(kernel);
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

void bmp24_sharpen(t_bmp24 *img) {
    float **kernel = createSharpenKernel();
    t_bmp24 *tmp = bmp24_allocate(img->width, img->height, img->colorDepth);
    if (!tmp) {
        freeKernel(kernel);
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            tmp->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = tmp->data[y][x];
        }
    }

    bmp24_free(tmp);
    freeKernel(kernel);
}







