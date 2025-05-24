#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include "bmp24.h"

void menu_bmp8() {
    t_bmp8 *img = bmp8_loadImage("C:/Users/thoma/CLionProjects/quotes-thomas-deltour-Nicolas-yungmann-c/barbara_gray.bmp");
    if (!img) return;

    int choix;
    do {
        printf("\n--- MENU BMP 8 BITS ---\n");
        printf("1 - Afficher les infos\n");
        printf("2 - Negatif\n");
        printf("3 - Modifier la luminosite\n");
        printf("4 - Seuillage\n");
        printf("5 - Sauvegarder sous 'resultat.bmp'\n");
        printf("6 - Filtres avances (convolution)\n");
        printf("0 - Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: bmp8_printInfo(img); break;
            case 2: bmp8_negative(img); break;
            case 3: {
                int value;
                printf("Valeur de luminosite : ");
                scanf("%d", &value);
                bmp8_brightness(img, value);
                break;
            }
            case 4: {
                int seuil;
                printf("Seuil (0-255) : ");
                scanf("%d", &seuil);
                bmp8_threshold(img, seuil);
                break;
            }
            case 5: bmp8_saveImage("resultat.bmp", img); break;
            case 6: {
                printf("Filtre : 1-Box 2-Gauss 3-Contours 4-Relief 5-Net\nVotre choix : ");
                int f; scanf("%d", &f);
                float **kernel = NULL;
                switch (f) {
                    case 1: kernel = createBoxBlurKernel(); break;
                    case 2: kernel = createGaussianBlurKernel(); break;
                    case 3: kernel = createOutlineKernel(); break;
                    case 4: kernel = createEmbossKernel(); break;
                    case 5: kernel = createSharpenKernel(); break;
                    default: printf("Choix invalide.\n"); break;
                }
                if (kernel) {
                    bmp8_applyFilter(img, kernel, 3);
                    freeKernel(kernel);
                }
                break;
            }
            case 0: break;
            default: printf("Choix invalide.\n"); break;
        }
    } while (choix != 0);

    bmp8_free(img);
}

void menu_bmp24() {
    t_bmp24 *img = bmp24_loadImage("C:/Users/thoma/CLionProjects/quotes-thomas-deltour-Nicolas-yungmann-c/flowers_color.bmp");
    if (!img) return;

    int choix;
    do {
        printf("\n--- MENU BMP 24 BITS ---\n");
        printf("1 - Negatif\n");
        printf("2 - Niveaux de gris\n");
        printf("3 - Modifier la luminosite\n");
        printf("4 - Box blur\n");
        printf("5 - Gaussian blur\n");
        printf("6 - Contours\n");
        printf("7 - Relief\n");
        printf("8 - Nettete\n");
        printf("9 - Sauvegarder sous 'resultat.bmp'\n");
        printf("0 - Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: bmp24_negative(img); break;
            case 2: bmp24_grayscale(img); break;
            case 3: {
                int value;
                printf("Valeur de luminosite : ");
                scanf("%d", &value);
                bmp24_brightness(img, value);
                break;
            }
            case 4: bmp24_boxBlur(img); break;
            case 5: bmp24_gaussianBlur(img); break;
            case 6: bmp24_outline(img); break;
            case 7: bmp24_emboss(img); break;
            case 8: bmp24_sharpen(img); break;
            case 9: bmp24_saveImage(img, "resultat.bmp"); break;
            case 0: break;
            default: printf("Choix invalide.\n"); break;
        }
    } while (choix != 0);

    bmp24_free(img);
}

int main() {
    int mode = 0;
    printf("=== MENU DE LANCEMENT ===\n");
    printf("1 - Utiliser une image BMP 8 bits (niveau de gris)\n");
    printf("2 - Utiliser une image BMP 24 bits (couleur)\n");
    printf("Votre choix : ");
    scanf("%d", &mode);

    if (mode == 1) {
        menu_bmp8();
    } else if (mode == 2) {
        menu_bmp24();
    } else {
        printf("Choix invalide. Le programme va se fermer.\n");
    }

    return 0;
}


