#include "animations.hpp"

void bruiter_image(const Image_PNG& image_source, Image_PNG& image_destination, int intensite) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    // TODO: a completer pour la consigne du projet
    for(size_t y = 0; y < image_source.hauteur; y++) {
        for(size_t x = 0; x < image_source.largeur; x++) {
            RVB pixel_src = image_source.pixels[y][x];
            image_destination.pixels[y][x].rouge = clamp(rand() % intensite - intensite/2 + pixel_src.rouge, 0, 255);
            image_destination.pixels[y][x].vert  = clamp(rand() % intensite - intensite/2 + pixel_src.vert , 0, 255);
            image_destination.pixels[y][x].bleu  = clamp(rand() % intensite - intensite/2 + pixel_src.bleu , 0, 255);
        }
    }
}

void creer_animation_fondu_bruitage(
    const string& chemin_image, 
    const string& chemin_destination, 
    size_t nb_etapes, 
    double intensite_max
) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double intensite = (intensite_max / nb_etapes) * (i + 1);
        bruiter_image(image_source, image_destination, intensite);
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