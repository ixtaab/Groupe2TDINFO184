#include "animations.hpp"

void bruiter_image(const Image_PNG& image_source, Image_PNG& image_destination, int intensite, BruitageMethod method) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);

    Image_PNG temp = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t y = 0; y < image_source.hauteur; y++) {
        for(size_t x = 0; x < image_source.largeur; x++) {
            temp.pixels[y][x] = image_source.pixels[y][x];
        }
    }

    switch(method) {
        case BruitageMethod::ALTERATION_ALEATOIRE: {
            for(size_t y = 0; y < temp.hauteur; y++) {
                for(size_t x = 0; x < temp.largeur; x++) {
                    RVB& pixel = temp.pixels[y][x];
                    pixel.rouge = clamp(pixel.rouge + (rand() % 3 - 1) * intensite, 0, 255);
                    pixel.vert  = clamp(pixel.vert  + (rand() % 3 - 1) * intensite, 0, 255);
                    pixel.bleu  = clamp(pixel.bleu  + (rand() % 3 - 1) * intensite, 0, 255);
                }
            }
            break;
        }
        case BruitageMethod::REMPLACEMENT_ALEATOIRE: {
            int nb_pixels_to_change = (temp.hauteur * temp.largeur * intensite) / 255;
            for(int i = 0; i < nb_pixels_to_change; i++) {
                size_t x = rand() % temp.largeur;
                size_t y = rand() % temp.hauteur;
                temp.pixels[y][x].rouge = rand() % 256;
                temp.pixels[y][x].vert  = rand() % 256;
                temp.pixels[y][x].bleu  = rand() % 256;
            }
            break;
        }
    }

    for(size_t y = 0; y < temp.hauteur; y++) {
        for(size_t x = 0; x < temp.largeur; x++) {
            image_destination.pixels[y][x] = temp.pixels[y][x];
        }
    }
}

void creer_animation_fondu_bruitage(
    const string& chemin_image, 
    const string& chemin_destination, 
    size_t nb_etapes, 
    double intensite,
    BruitageMethod method
) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);

    for(size_t i = 0; i < nb_etapes; i++) {
        bruiter_image(image_source, image_source, intensite, method);
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