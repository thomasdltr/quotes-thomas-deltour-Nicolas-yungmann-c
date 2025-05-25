# quotes-thomas-deltour-Nicolas-yungmann-c

Documentation technique
Structure du projet
    bmp8.h/c : Gestion des images 8 bits (niveaux de gris)
    bmp24.h/c : Gestion des images 24 bits (couleur)
    filtres.h/c : Implémentation des noyaux de convolution
    main.c : Interface utilisateur et menu principal

Algorithmes clés
    Lecture/écriture BMP : Parsing des en-têtes et gestion du padding
    Filtres de convolution : Application de noyaux avec gestion des bords
    Égalisation d'histogramme :
        Calcul de l'histogramme et de la CDF
        Normalisation et transformation
        Version couleur via conversion YUV

Journal de bord
Chronologie du projet
    Semaine du 17/03 : Début du projet, Thomas implémente la structure de base pour les images 8 bits
    Semaine du 31/03 : Nicolas travaille sur la gestion des images 24 bits
    Semaine du 21/04 : Implémentation conjointe des filtres de convolution
    27/04 : Dépôt intermédiaire avec les structures de données de base
    Semaine du 28/04 : Correction des bugs de lecture/écriture
    Semaine du 19/05 : Finalisation de l'égalisation d'histogramme couleur
    25/05 : Dépôt final et préparation de la soutenance

Répartition des tâches
    Thomas :
        Structures et fonctions pour images 8 bits
        Filtres de convolution de base
        Égalisation d'histogramme niveaux de gris
        Menu principal 8 bits
    Nicolas :
        Structures et fonctions pour images 24 bits
        Conversion YUV et égalisation couleur
        Gestion du padding BMP
        Menu principal 24 bits

Bugs connus
    Problème d'affichage avec certaines images 24 bits après traitement (en cours de correction)
    Gestion des bords dans les filtres de convolution peut être améliorée
