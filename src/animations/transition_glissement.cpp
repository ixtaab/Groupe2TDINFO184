#include "animations.hpp"

void transition_glissement(
    const Image_PNG& image_source,
    const Image_PNG& image_cible, 
    Image_PNG& image_destination,
    double progression
) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    assert(image_cible.largeur == image_destination.largeur);
    assert(image_cible.hauteur == image_destination.hauteur);

    size_t decalage = progression * image_source.largeur;

    for (size_t y = 0; y < image_destination.hauteur; y++) {
        for (size_t x = 0; x < image_destination.largeur; x++) {
            image_destination.pixels[y][x] = image_cible.pixels[y][x];
        }
    }

    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            if (x + decalage < image_destination.largeur) {
                image_destination.pixels[y][x + decalage] = image_source.pixels[y][x];
            }
        }
    }
}

void creer_animation_transition_glissement(
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

    for(size_t i = 0; i < nb_etapes; i++) {
        double progression = static_cast<double>(i) / (nb_etapes - 1);
        transition_glissement(image_source, image_cible, image_destination, progression);
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