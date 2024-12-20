#include "animations.hpp"

/**
 * Crée une image intermédiaire entre deux images par fondu
 * @param image_source Première image (image de départ)
 * @param image_cible Deuxième image (image d'arrivée)
 * @param image_destination Image résultante du fondu
 * @param ratio Proportion de l'image source (entre 0 et 1)
 *        1.0 = uniquement image_source
 *        0.0 = uniquement image_cible
 * 
 * @note invariances: 
 * 
 *  - L'image de destination, cible et source on la même hauteur et largeur. 
 */
void fondre_images(
    const Image_PNG& image_source,
    const Image_PNG& image_cible,
    Image_PNG& image_destination,
    double ratio
) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    assert(image_cible.largeur == image_destination.largeur);
    assert(image_cible.hauteur == image_destination.hauteur);

    // Pour chaque pixel, calcule la moyenne pondérée entre les deux images
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            RVB pixel_source = image_source.pixels[y][x];
            RVB pixel_cible = image_cible.pixels[y][x];
            
            // Interpolation linéaire entre les composantes RGB des deux images
            image_destination.pixels[y][x] = {
                static_cast<Composante>(pixel_source.rouge * ratio + pixel_cible.rouge * (1.0 - ratio)),
                static_cast<Composante>(pixel_source.vert  * ratio + pixel_cible.vert  * (1.0 - ratio)),
                static_cast<Composante>(pixel_source.bleu  * ratio + pixel_cible.bleu  * (1.0 - ratio))
            };
        }
    }
}

/**
 * Crée une animation de transition par fondu entre deux images
 * @param chemin_image_source Chemin de l'image de départ
 * @param chemin_image_cible Chemin de l'image d'arrivée
 * @param chemin_destination Dossier de destination
 * @param nb_etapes Nombre d'images dans l'animation
 * 
 * Les images sont sauvegardées dans chemin_destination/images/
 * Le GIF final est sauvegardé dans chemin_destination/gif/
 * L'animation commence avec l'image source et se termine avec l'image cible
 */
void creer_animation_transition_fondu(
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

    // Génère chaque étape de l'animation avec un ratio décroissant
    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        fondre_images(image_source, image_cible, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    // Génère le GIF final
    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}