#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include "filtres.h"

/*
Fonction pour charger une image BMP 8 bits (niveaux de gris)
- Ouvre le fichier et vérifie son existence
- Lit l'en-tête et la table des couleurs
- Alloue la mémoire pour l'image
- Vérifie que la profondeur est bien 8 bits
- Lit les données des pixels
*/
t_bmp8* bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : impossible d ouvrir le fichier %s\n", filename);
        return NULL;
    }

    t_bmp8 *image = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!image) {
        printf("Erreur : echec de l allocation memoire\n");
        fclose(file);
        return NULL;
    }

    // Lire le header (54 octets)
    fread(image->header, sizeof(unsigned char), 54, file);

    // Lire la table des couleurs (1024 octets)
    fread(image->colorTable, sizeof(unsigned char), 1024, file);

    // Extraire les métadonnées depuis le header BMP
    image->width       = *(unsigned int *)&image->header[18];
    image->height      = *(unsigned int *)&image->header[22];
    image->colorDepth  = *(unsigned short *)&image->header[28];
    image->dataSize    = image->width * image->height;

    // Vérification profondeur de couleur
    if (image->colorDepth != 8) {
        printf("Erreur : image non 8 bits (profondeur = %u)\n", image->colorDepth);
        free(image);
        fclose(file);
        return NULL;
    }

    // Allocation mémoire pour les pixels
    image->data = (unsigned char *)malloc(image->dataSize);
    if (!image->data) {
        printf("Erreur : echec allocation memoire des pixels\n");
        free(image);
        fclose(file);
        return NULL;
    }

    // Lire les données de l'image
    fread(image->data, sizeof(unsigned char), image->dataSize, file);

    fclose(file);
    return image;
}

/*
Sauvegarde une image BMP 8 bits
- Ouvre le fichier en écriture
- Écrit l'en-tête, la table des couleurs et les données
*/
void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (!img) {
        printf("Erreur : image invalide (NULL)\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : impossible d ouvrir le fichier %s en ecriture\n", filename);
        return;
    }

    // Écrire le header (54 octets)
    if (fwrite(img->header, sizeof(unsigned char), 54, file) != 54) {
        printf("Erreur lors de l ecriture du header\n");
        fclose(file);
        return;
    }

    // Écrire la table des couleurs (1024 octets)
    if (fwrite(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        printf("Erreur lors de l ecriture de la table de couleurs\n");
        fclose(file);
        return;
    }

    // Écrire les données image (pixels)
    if (fwrite(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        printf("Erreur lors de l ecriture des pixels\n");
        fclose(file);
        return;
    }

    fclose(file);
    printf("Image sauvegardee dans %s \n", filename);
}

/*
Libère la mémoire allouée pour une image BMP 8 bits
*/
void bmp8_free(t_bmp8 *img) {
    if (img != NULL) {
        if (img->data != NULL) {
            free(img->data);  // Libère les pixels
        }
        free(img);  // Libère la structure entière
    }
}

/*
Affiche les informations de base d'une image BMP 8 bits
*/
void bmp8_printInfo(t_bmp8 *img) {
    if (img == NULL) {
        printf("Image invalide.\n");
        return;
    }

    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
}

/*
- Applique un effet négatif à l'image
- Inverse chaque valeur de pixel (255 - valeur)
*/
void bmp8_negative(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image invalide pour bmp8_negative.\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];  // Inversion du pixel
    }

    printf("Effet negatif applique \n");
}

/*
Ajuste la luminosité de l'image
- Ajoute une valeur à chaque pixel
- Clampe les valeurs entre 0 et 255
*/
void bmp8_brightness(t_bmp8 *img, int value) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image invalide pour bmp8_brightness.\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        int pixel = img->data[i] + value;

        if (pixel > 255) pixel = 255;
        if (pixel < 0) pixel = 0;

        img->data[i] = (unsigned char)pixel;
    }

    printf("Luminosite ajustee (value = %d) \n", value);
}

/*
Applique un seuillage à l'image
- Met à 255 les pixels >= seuil
- Met à 0 les autres
*/
void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image invalide pour bmp8_threshold.\n");
        return;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }

    printf("Seuil applique (threshold = %d) \n", threshold);
}

/*
Applique un filtre de convolution à l'image
- Crée une copie des données pour éviter les effets de bord
- Applique le noyau de convolution à chaque pixel
- Clampe les valeurs entre 0 et 255
*/
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (img == NULL || img->data == NULL) {
        printf("Erreur : image invalide pour bmp8_applyFilter.\n");
        return;
    }

    int offset = kernelSize / 2;
    unsigned char *newData = malloc(img->dataSize);
    if (!newData) {
        printf("Erreur d'allocation mémoire pour le filtre.\n");
        return;
    }

    // Copie initiale des données (pour garder les bords inchangés)
    memcpy(newData, img->data, img->dataSize);

    // Application du filtre
    for (unsigned int y = offset; y < img->height - offset; y++) {
        for (unsigned int x = offset; x < img->width - offset; x++) {
            float sum = 0.0f;

            for (int i = -offset; i <= offset; i++) {
                for (int j = -offset; j <= offset; j++) {
                    int pixelIndex = (y + i) * img->width + (x + j);
                    sum += img->data[pixelIndex] * kernel[i + offset][j + offset];
                }
            }

            // Clamp entre 0 et 255
            int value = (int)(sum + 0.5f);
            if (value < 0) value = 0;
            if (value > 255) value = 255;

            newData[y * img->width + x] = (unsigned char)value;
        }
    }

    // Remplace l'image originale par la nouvelle
    free(img->data);
    img->data = newData;

    printf("Filtre applique  (kernelSize = %d)\n", kernelSize);
}

#include <math.h>  // pour round()

/*
Calcule l'histogramme d'une image 8 bits
- Compte le nombre de pixels pour chaque niveau de gris (0-255)
*/
unsigned int *bmp8_computeHistogram(t_bmp8 *img) {
    if (!img || !img->data) return NULL;

    unsigned int *hist = calloc(256, sizeof(unsigned int));
    if (!hist) {
        printf("Erreur allocation histogramme\n");
        return NULL;
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        hist[img->data[i]]++;
    }

    return hist;
}

/*
Calcule la CDF (fonction de répartition cumulée) d'un histogramme
- Normalise les valeurs pour l'égalisation d'histogramme
*/
unsigned int *bmp8_computeCDF(unsigned int *hist, unsigned int dataSize) {
    if (!hist) return NULL;

    unsigned int *cdf = malloc(256 * sizeof(unsigned int));
    if (!cdf) {
        printf("Erreur allocation CDF\n");
        return NULL;
    }

    // Étape 1 : CDF brute
    unsigned int cum = 0;
    for (int i = 0; i < 256; i++) {
        cum += hist[i];
        cdf[i] = cum;
    }

    // Étape 2 : chercher le plus petit cdf non nul
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // Étape 3 : normalisation
    for (int i = 0; i < 256; i++) {
        cdf[i] = round(((float)(cdf[i] - cdf_min) / (dataSize - cdf_min)) * 255);
    }

    return cdf;
}

/*
Applique l'égalisation d'histogramme à une image
- Utilise la CDF normalisée pour redistribuer les niveaux de gris
*/
void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq) {
    if (!img || !img->data || !hist_eq) return;

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char)hist_eq[img->data[i]];
    }

    printf("Egalisation d histogramme appliquee\n");
}
