#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

int main() {
    t_bmp8 *img = bmp8_loadImage("C:/Users/thoma/CLionProjects/quotes-thomas-deltour-Nicolas-yungmann-c/barbara_gray.bmp");
    if (!img) return 1;

    int choix;
    do {
        printf("\n--- MENU TRAITEMENT IMAGE ---\n");
        printf("1 - Afficher les infos\n");
        printf("2 - Appliquer un negatif\n");
        printf("3 - Modifier la luminosite (+/-)\n");
        printf("4 - Appliquer un seuillage (threshold)\n");
        printf("5 - Sauvegarder sous 'resultat.bmp'\n");
        printf("6 - Appliquer un filtre avance\n");
        printf("0 - Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                bmp8_printInfo(img);
            break;

            case 2:
                bmp8_negative(img);
            break;

            case 3: {
                int value;
                printf("Valeur de luminosité (+/-) : ");
                scanf("%d", &value);
                bmp8_brightness(img, value);
                break;
            }

            case 4: {
                int seuil;
                printf("Valeur du seuil (0-255) : ");
                scanf("%d", &seuil);
                bmp8_threshold(img, seuil);
                break;
            }

            case 5:
                bmp8_saveImage("resultat.bmp", img);
            break;

            case 6:
                printf("Choisissez un filtre :\n");
            printf("1 - Box Blur\n2 - Gaussian Blur\n3 - Outline\n4 - Emboss\n5 - Sharpen\nVotre choix : ");
            int f;
            scanf("%d", &f);
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


            case 0:
                printf("Fermeture du programme.\n");
            break;

            default:
                printf("Choix invalide.\n");
            break;
        }
    } while (choix != 0);

    bmp8_free(img);
    return 0;
}




