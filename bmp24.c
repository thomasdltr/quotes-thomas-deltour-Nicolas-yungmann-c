#include "bmp24.h"
#include <stdlib.h>
#include "filtres.h"

/*
Fonction utilitaire pour lire des données brutes depuis un fichier
- Se positionne à l'offset donné
- Lit n éléments de taille size dans le buffer
*/
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

/*
Alloue la mémoire pour une matrice de pixels
- Crée un tableau 2D de t_pixel
- Gère les erreurs d'allocation
*/
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

/*
Libère la mémoire d'une matrice de pixels
*/
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (!pixels) return;
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

/*
Alloue une structure t_bmp24 complète
- Initialise les dimensions et la profondeur de couleur
- Alloue la matrice de pixels
*/
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

/*
Libère toute la mémoire d'une image BMP 24 bits
*/
void bmp24_free(t_bmp24 *img) {
    if (!img) return;
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}

/*
Charge une image BMP 24 bits depuis un fichier
- Vérifie que la profondeur est bien 24 bits
- Lit les en-têtes et les données
*/
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : impossible d ouvrir le fichier %s\n", filename);
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
        printf("Erreur : image non 24 bits (%d bits detectes)\n", bits);
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
    printf("Bits detectes : %d\n", bits);

    fclose(file);
    return img;
}

/*
Lit la valeur d'un pixel spécifique depuis un fichier
- Lit les 3 composantes BGR
- Stocke dans la structure t_pixel (RGB)
*/
void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint8_t bgr[3];
    fread(bgr, sizeof(uint8_t), 3, file);
    image->data[y][x].blue  = bgr[0];
    image->data[y][x].green = bgr[1];
    image->data[y][x].red   = bgr[2];
}

/*
Lit toutes les données pixels d'une image BMP 24 bits
- Gère le padding des lignes (multiple de 4 octets)
- Lit de bas en haut (format BMP)
*/
void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    int padding = (4 - (image->width * 3) % 4) % 4;

    fseek(file, image->header.offset, SEEK_SET);
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }
        fseek(file, padding, SEEK_CUR);  // sauter les octets de padding à chaque ligne
    }
}

/*
Écrit la valeur d'un pixel spécifique dans un fichier
- Convertit de RGB à BGR (format BMP)
*/
void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint8_t bgr[3];
    bgr[0] = image->data[y][x].blue;
    bgr[1] = image->data[y][x].green;
    bgr[2] = image->data[y][x].red;
    fwrite(bgr, sizeof(uint8_t), 3, file);
}

/*
Écrit toutes les données pixels d'une image BMP 24 bits
- Ajoute le padding nécessaire pour les lignes
- Écrit de bas en haut (format BMP)
*/
void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    int padding = (4 - (image->width * 3) % 4) % 4;
    uint8_t pad[3] = {0, 0, 0};

    fseek(file, image->header.offset, SEEK_SET);

    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
        fwrite(pad, 1, padding, file);  // écrire les octets de padding à la fin de chaque ligne
    }
}

/*
Fonction utilitaire pour écrire des données brutes dans un fichier
*/
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

/*
Sauvegarde une image BMP 24 bits dans un fichier
- Écrit les en-têtes puis les données pixels
*/
void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : impossible d'ouvrir le fichier %s en ecriture\n", filename);
        return;
    }

    // Écriture des en-têtes BMP
    file_rawWrite(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawWrite(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    // Écriture des données de pixels
    bmp24_writePixelData(img, file);

    fclose(file);
    printf("Image sauvegardee dans %s\n", filename);
}

/*
Applique un effet négatif à une image couleur
- Inverse chaque composante RGB (255 - valeur)
*/
void bmp24_negative(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red   = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue  = 255 - img->data[y][x].blue;
        }
    }
}

/*
Convertit une image couleur en niveaux de gris
- Moyenne des 3 composantes RGB pour chaque pixel
*/
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

/*
Ajuste la luminosité d'une image couleur
- Ajoute une valeur à chaque composante RGB
- Clampe les valeurs entre 0 et 255
*/
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

/*
Applique une convolution à un pixel spécifique
- Utilise un noyau de convolution donné
- Gère les bords de l'image
*/
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

/*
Applique un filtre box blur à l'image
- Crée un noyau de flou moyen
- Applique la convolution à toute l'image
*/
void bmp24_boxBlur(t_bmp24 *img) {
    float **kernel = createBoxBlurKernel();
    t_bmp24 *tmp = bmp24_allocate(img->width, img->height, img->colorDepth);
    if (!tmp) return;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            tmp->data[y][x] = bmp24_convolution(img, x, y, kernel, 3);
        }
    }

    // recopier dans l'image originale
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = tmp->data[y][x];
        }
    }

    bmp24_free(tmp);
    freeKernel(kernel);
}

/*
Applique un filtre gaussien à l'image
- Crée un noyau gaussien 3x3
- Applique la convolution à toute l'image
*/
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

/*
Applique un filtre de détection de contours
- Crée un noyau spécifique
- Applique la convolution à toute l'image
*/
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

/*
Applique un filtre de relief (emboss)
- Crée un noyau spécifique
- Applique la convolution à toute l'image
*/
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

/*
Applique un filtre de netteté (sharpen)
- Crée un noyau spécifique
- Applique la convolution à toute l'image
*/
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

#include <math.h> // pour round()

/*
Applique l'égalisation d'histogramme à une image couleur
- Convertit en espace YUV
- Égalise la composante Y (luminance)
- Reconversion en RGB
*/
void bmp24_equalize(t_bmp24 *img) {
    if (!img || !img->data) return;

    int width = img->width;
    int height = img->height;

    // Étapes 1-2 : allouer des tableaux pour Y, U, V
    float **Y = malloc(height * sizeof(float *));
    float **U = malloc(height * sizeof(float *));
    float **V = malloc(height * sizeof(float *));
    for (int i = 0; i < height; i++) {
        Y[i] = malloc(width * sizeof(float));
        U[i] = malloc(width * sizeof(float));
        V[i] = malloc(width * sizeof(float));
    }

    // Étape 3 : convertir RGB → YUV et remplir les matrices
    unsigned int *hist = calloc(256, sizeof(unsigned int));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            t_pixel p = img->data[i][j];

            float r = (float)p.red;
            float g = (float)p.green;
            float b = (float)p.blue;

            float y = 0.299 * r + 0.587 * g + 0.114 * b;
            float u = -0.14713 * r - 0.28886 * g + 0.436 * b;
            float v =  0.615 * r - 0.51499 * g - 0.10001 * b;

            // clamp y entre 0 et 255 avant histogramme
            int y_int = (int)round(y);
            if (y_int < 0) y_int = 0;
            if (y_int > 255) y_int = 255;

            hist[y_int]++;

            Y[i][j] = y;
            U[i][j] = u;
            V[i][j] = v;
        }
    }

    // Étape 4 : CDF et normalisation
    unsigned int *cdf = malloc(256 * sizeof(unsigned int));
    unsigned int total = width * height;
    unsigned int cum = 0;
    for (int i = 0; i < 256; i++) {
        cum += hist[i];
        cdf[i] = cum;
    }

    // chercher le cdf_min ≠ 0
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // hist_eq = tableau de correspondance
    unsigned char hist_eq[256];
    for (int i = 0; i < 256; i++) {
        hist_eq[i] = (unsigned char)round(((float)(cdf[i] - cdf_min) / (total - cdf_min)) * 255);
    }

    // Étape 5-6 : appliquer égalisation sur Y, puis reconvertir en RGB
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int y_new = hist_eq[(int)round(Y[i][j])];
            float u = U[i][j];
            float v = V[i][j];

            // YUV → RGB
            int r = (int)round(y_new + 1.13983 * v);
            int g = (int)round(y_new - 0.39465 * u - 0.58060 * v);
            int b = (int)round(y_new + 2.03211 * u);

            // Clamp entre 0-255
            if (r < 0) r = 0; if (r > 255) r = 255;
            if (g < 0) g = 0; if (g > 255) g = 255;
            if (b < 0) b = 0; if (b > 255) b = 255;

            img->data[i][j].red = (unsigned char)r;
            img->data[i][j].green = (unsigned char)g;
            img->data[i][j].blue = (unsigned char)b;
        }
    }

    // Nettoyage mémoire
    for (int i = 0; i < height; i++) {
        free(Y[i]);
        free(U[i]);
        free(V[i]);
    }
    free(Y); free(U); free(V);
    free(hist); free(cdf);

    printf("Egalisation YUV terminee pour image couleur \n");
}
