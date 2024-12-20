#include "animations.hpp"

/**
* Convertit une image en niveaux de gris avec un ratio de conversion ajustable
* @param image_source Image source en couleur
* @param image_destination Image de destination
* @param ratio Ratio de conservation des couleurs (0 = niveaux de gris complets, 1 = couleurs originales)
*
* @note invariances:
* 
* - image_source et image_destination doivent avoir les mêmes dimensions. 
* 
* La conversion utilise la formule standard de luminosité :
* L = R * 0.299 + G * 0.587 + B * 0.114
* Puis fait une interpolation linéaire entre la couleur originale et la luminosité
*/
void convertir_niveaux_gris(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            RVB pixel = image_source.pixels[y][x];
            // Calcul de la luminosité selon les coefficients standard
            double luminosite = pixel.rouge * 0.299 + pixel.vert * 0.587 + pixel.bleu * 0.114;
            
            // Interpolation entre couleur originale et niveau de gris
            image_destination.pixels[y][x] = {
                static_cast<Composante>(pixel.rouge * ratio + luminosite * (1-ratio)),
                static_cast<Composante>(pixel.vert * ratio + luminosite * (1-ratio)),
                static_cast<Composante>(pixel.bleu * ratio + luminosite * (1-ratio))
            };
        }
    }
}

/**
* Crée une animation de transition entre une image couleur et sa version en niveaux de gris
* @param chemin_image Chemin de l'image source
* @param chemin_destination Dossier de destination
* @param nb_etapes Nombre d'images dans l'animation
* 
* L'animation commence par l'image en couleur et finit par l'image en niveaux de gris
* Les images sont sauvées dans chemin_destination/images/
* Le GIF final est sauvé dans chemin_destination/gif/
*/
void creer_animation_fondu_niveaux_gris(
    const string& chemin_image, 
    const string& chemin_destination, 
    size_t nb_etapes
) {
    creer_dossiers_sortie(chemin_destination);
    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    // Crée les images avec un ratio décroissant de couleur
    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        convertir_niveaux_gris(image_source, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    // Génère le GIF final
    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}