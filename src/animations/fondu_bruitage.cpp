#include "animations.hpp"

/**
* Applique un effet de bruit sur une image
* @param image_source Image source à bruiter
* @param image_destination Image où stocker le résultat
* @param intensite Niveau de bruit (0-255)
* @param method Méthode de bruitage à utiliser :
*
*    - ALTERATION_ALEATOIRE : Modifie légèrement chaque pixel 
*
*    - REMPLACEMENT_ALEATOIRE : Remplace aléatoirement certains pixels
* Les images source et destination doivent avoir les mêmes dimensions
*
* @note invariances: 
*
* - image_source et image_destination doivent avoir les mêmes dimensions. 
*/
void bruiter_image(
    const Image_PNG& image_source, 
    Image_PNG& image_destination,
    int intensite, 
    BruitageMethod method
) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);

    // Copie de l'image source pour ne pas la modifier directement
    Image_PNG temp = creer_PNG(image_source.hauteur, image_source.largeur);
    for(size_t y = 0; y < image_source.hauteur; y++) {
        for(size_t x = 0; x < image_source.largeur; x++) {
            temp.pixels[y][x] = image_source.pixels[y][x];
        }
    }

    switch(method) {
        case BruitageMethod::ALTERATION_ALEATOIRE: {
            // Modifie légèrement chaque composante RGB de chaque pixel
            for(size_t y = 0; y < temp.hauteur; y++) {
                for(size_t x = 0; x < temp.largeur; x++) {
                    RVB& pixel = temp.pixels[y][x];
                    pixel.rouge = clamp(pixel.rouge + (rand() % 3 - 1) * intensite, 0, 255);
                    pixel.vert = clamp(pixel.vert + (rand() % 3 - 1) * intensite, 0, 255);
                    pixel.bleu = clamp(pixel.bleu + (rand() % 3 - 1) * intensite, 0, 255);
                }
            }
            break;
        }
        case BruitageMethod::REMPLACEMENT_ALEATOIRE: {
            // Remplace un nombre de pixels proportionnel à l'intensité par des couleurs aléatoires
            int nb_pixels_to_change = (temp.hauteur * temp.largeur * intensite) / 255;
            for(int i = 0; i < nb_pixels_to_change; i++) {
                size_t x = rand() % temp.largeur;
                size_t y = rand() % temp.hauteur;
                temp.pixels[y][x].rouge = rand() % 256;
                temp.pixels[y][x].vert = rand() % 256;
                temp.pixels[y][x].bleu = rand() % 256;
            }
            break;
        }
    }

    // Copie le résultat dans l'image de destination
    for(size_t y = 0; y < temp.hauteur; y++) {
        for(size_t x = 0; x < temp.largeur; x++) {
            image_destination.pixels[y][x] = temp.pixels[y][x];
        }
    }
}

/**
* Crée une animation GIF montrant une image progressivement bruitée
* @param chemin_image Chemin vers l'image source
* @param chemin_destination Dossier où sauvegarder les fichiers générés
* @param nb_etapes Nombre d'images dans l'animation
* @param intensite Intensité du bruitage à chaque étape
* @param method Méthode de bruitage à utiliser
* 
* Les images intermédiaires sont sauvées dans chemin_destination/images/
* Le GIF final est sauvé dans chemin_destination/gif/
*/
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

    // Applique le bruitage progressivement et sauve chaque étape
    for(size_t i = 0; i < nb_etapes; i++) {
        bruiter_image(image_source, image_source, intensite, method);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_source
        );
    }

    // Génère le GIF à partir des images
    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}