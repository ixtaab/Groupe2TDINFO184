#include "animations.hpp"

void masquer_rideau(
    const Image_PNG& image_source, 
    Image_PNG& image_destination, 
    size_t ligne_courante, 
    RVB couleur
) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    
    for (size_t y = 0; y < image_source.hauteur; ++y) {
        for (size_t x = 0; x < image_source.largeur; ++x) {
            image_destination.pixels[y][x] = image_source.pixels[y][x];
        }
    }
    
    for (size_t y = 0; y <= ligne_courante && y < image_source.hauteur; ++y) {
        for (size_t x = 0; x < image_source.largeur; ++x) {
            image_destination.pixels[y][x] = couleur;
        }
    }
}

void masquer_persiennes(
    const Image_PNG& image_source, 
    Image_PNG& image_destination, 
    double intensite, 
    size_t separation, 
    size_t decalage, 
    RVB couleur
) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    
    for (size_t y = 0; y < image_source.hauteur; ++y) {
        for (size_t x = 0; x < image_source.largeur; ++x) {
            image_destination.pixels[y][x] = image_source.pixels[y][x];
        }
    }
    
    RVB couleur_actuelle = {
        static_cast<Composante>(couleur.rouge * intensite),
        static_cast<Composante>(couleur.vert * intensite),
        static_cast<Composante>(couleur.bleu * intensite)
    };
    
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x += separation) {
            image_destination.pixels[y][min(x + decalage, image_source.largeur - 1)] = couleur_actuelle;
        }
    }
}

void creer_animation_masquage(
    const string& chemin_image, 
    const string& chemin_destination, 
    size_t nb_etapes, 
    RVB couleur, 
    bool mode_rideau, 
    size_t K
) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);

    for(size_t i = 0; i < nb_etapes; i++) {
        if (mode_rideau) {
            size_t ligne_courante = (i * image_source.hauteur) / (nb_etapes - 1);
            masquer_rideau(image_source, image_source, ligne_courante, couleur);
        } else {
            double intensite = static_cast<double>(i) / (nb_etapes - 1);
            masquer_persiennes(image_source, image_source, intensite, K, i, couleur);
        }

        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_source
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}
