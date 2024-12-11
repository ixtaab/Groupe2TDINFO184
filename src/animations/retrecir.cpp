#include "animations.hpp"

void retrecir_image(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    assert(ratio >= 0.0 && ratio <= 1.0);
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);

    size_t hauteur = image_source.hauteur;
    size_t largeur = image_source.largeur;

    for (size_t y = 0; y < hauteur; y++) {
        for (size_t x = 0; x < largeur; x++) {
            image_destination.pixels[y][x] = {};
        }
    }

    size_t decalage_x = (largeur - static_cast<size_t>(largeur * ratio)) / 2;
    size_t decalage_y = (hauteur - static_cast<size_t>(hauteur * ratio)) / 2;

    for (size_t y = 0; y < hauteur; y++) {
        for (size_t x = 0; x < largeur; x++) {
            RVB pixel_source = image_source.pixels[y][x];

            size_t x_destination = decalage_x + static_cast<size_t>(x * ratio);
            size_t y_destination = decalage_y + static_cast<size_t>(y * ratio);

            if (x_destination < largeur && y_destination < hauteur) {
                image_destination.pixels[y_destination][x_destination] = pixel_source;
            }
        }
    }
}

void creer_animation_retrecir(const string& chemin_image, const string& chemin_destination, size_t nb_etapes) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        retrecir_image(image_source, image_destination, ratio);
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