#include "animations.hpp"

void assombrir_image(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            RVB pixel_source = image_source.pixels[y][x];
            image_destination.pixels[y][x] = {
                static_cast<Composante>(pixel_source.rouge * ratio),
                static_cast<Composante>(pixel_source.vert  * ratio),
                static_cast<Composante>(pixel_source.bleu  * ratio)
            };
        }
    }
}

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

    size_t nb_etapes_demi = nb_etapes / 2;
    for(size_t i = 0; i < nb_etapes_demi; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes_demi));
        assombrir_image(image_source, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

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