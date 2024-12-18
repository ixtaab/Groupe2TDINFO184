#include "animations.hpp"

void transition_rideau(
    const Image_PNG& image_source, 
    const Image_PNG& image_cible, 
    Image_PNG& image_destination, 
    double progression
) {
    for (size_t y = 0; y < image_destination.hauteur; y++) {
        for (size_t x = 0; x < image_destination.largeur; x++) {
            image_destination.pixels[y][x] = image_source.pixels[y][x];
        }
    }

    size_t ligne_limite = static_cast<size_t>(progression * image_source.hauteur);
    for (size_t y = 0; y < ligne_limite && y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            image_destination.pixels[y][x] = image_cible.pixels[y][x];
        }
    }
}

void creer_animation_transition_remplacement(
    const string& chemin_image_source,
    const string& chemin_image_cible, 
    const string& chemin_destination,
    size_t nb_etapes,
    RemplacementMethod method,
    size_t separation
) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image_source);
    
    Image_PNG image_source = charger_PNG(chemin_image_source);
    Image_PNG image_cible = charger_PNG(chemin_image_cible);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double progression = (nb_etapes > 1) 
                            ? static_cast<double>(i) / (nb_etapes - 1) 
                            : 1.0;

        transition_rideau(image_source, image_cible, image_destination, progression);

        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    if (nb_etapes > 1) {
        generer_GIF(
            chemin_destination + "/images/" + nom_image + "_",
            chemin_destination + "/gif/" + nom_image,
            0, nb_etapes - 1, 15, 0
        );
    }
}