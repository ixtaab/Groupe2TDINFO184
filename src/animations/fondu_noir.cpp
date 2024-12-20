#include "animations.hpp"

/**
* Assombrit une image selon un ratio donné
* @param image_source Image source 
* @param image_destination Image de destination
* @param ratio Ratio d'assombrissement (1 = image originale, 0 = noir complet)
* 
* @note invariances: 
*
* - image_source, image_cible et image_destination doivent avoir les mêmes dimensions. 
*/
void assombrir_image(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            RVB pixel_source = image_source.pixels[y][x];
            image_destination.pixels[y][x] = {
                static_cast<Composante>(pixel_source.rouge * ratio),
                static_cast<Composante>(pixel_source.vert * ratio),
                static_cast<Composante>(pixel_source.bleu * ratio)
            };
        }
    }
}

/**
* Crée une animation de fondu au noir
* @param chemin_image Chemin de l'image source
* @param chemin_destination Dossier de destination
* @param nb_etapes Nombre d'images dans l'animation
*/
void creer_animation_fondu_noir(const string& chemin_image, const string& chemin_destination, size_t nb_etapes) {
    creer_dossiers_sortie(chemin_destination);
    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        assombrir_image(image_source, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}

/**
* Crée une animation de transition entre deux images via un fondu au noir
* @param chemin_image_source Chemin de l'image de départ
* @param chemin_image_cible Chemin de l'image d'arrivée
* @param chemin_destination Dossier de destination
* @param nb_etapes Nombre total d'images dans l'animation
* 
* L'animation se fait en deux parties :
* 1. Première moitié : fondu au noir de l'image source
* 2. Deuxième moitié : apparition progressive de l'image cible depuis le noir
*/
void creer_animation_transition_noir(
    const string& chemin_image_source,
    const string& chemin_image_cible,
    const string& chemin_destination,
    size_t nb_etapes
) {
    creer_dossiers_sortie(chemin_destination);
    string nom_image = extraire_nom_fichier(chemin_image_source);
    Image_PNG image_source = charger_PNG(chemin_image_source);
    Image_PNG image_cible = charger_PNG(chemin_image_cible);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    // Première moitié : fondu au noir de l'image source
    size_t nb_etapes_demi = nb_etapes / 2;
    for(size_t i = 0; i < nb_etapes_demi; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes_demi));
        assombrir_image(image_source, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    // Deuxième moitié : apparition de l'image cible
    size_t nb_etapes_demi2 = nb_etapes - nb_etapes_demi;
    for(size_t i = 0; i < nb_etapes_demi2; i++) {
        double ratio = static_cast<double>(i) / (nb_etapes_demi2 - 1);
        assombrir_image(image_cible, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i + nb_etapes_demi) + ".png", image_destination
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}