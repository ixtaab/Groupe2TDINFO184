#include "animations.hpp"

void convertir_niveaux_gris(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            RVB pixel = image_source.pixels[y][x];
            double luminosite = pixel.rouge * 0.299 + pixel.vert * 0.587 + pixel.bleu * 0.114;
            image_destination.pixels[y][x] = {
                static_cast<Composante>(pixel.rouge * ratio + luminosite * (1-ratio)),
                static_cast<Composante>(pixel.vert  * ratio + luminosite * (1-ratio)),
                static_cast<Composante>(pixel.bleu  * ratio + luminosite * (1-ratio))
            };
        }
    }
}

void creer_animation_fondu_niveaux_gris(const string& chemin_image, const string& chemin_destination, size_t nb_etapes) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        convertir_niveaux_gris(image_source, image_destination, ratio);
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
