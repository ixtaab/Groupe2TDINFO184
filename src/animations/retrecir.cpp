#include "animations.hpp"

/**
 * Rétrécit une image selon un ratio donné
 * L'image rétrécie est centrée dans l'image de destination
 * @param image_source Image d'entrée
 * @param image_destination Image de sortie
 * @param ratio Facteur de rétrécissement (entre 0 et 1)
 *        1.0 = taille originale, 0.0 = image réduite à un point
 * 
 *  @note invariances: 
 * 
 *  - image_source et image_destination doivent avoir les mêmes dimensions. 
 */
void retrecir_image(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    // Vérifie que le ratio est valide et que les dimensions correspondent
    assert(ratio >= 0.0 && ratio <= 1.0);
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);

    size_t hauteur = image_source.hauteur;
    size_t largeur = image_source.largeur;

    // Initialise l'image de destination avec des pixels vides
    for (size_t y = 0; y < hauteur; y++) {
        for (size_t x = 0; x < largeur; x++) {
            image_destination.pixels[y][x] = {};
        }
    }

    // Calcule les décalages pour centrer l'image rétrécie
    size_t decalage_x = (largeur - static_cast<size_t>(largeur * ratio)) / 2;
    size_t decalage_y = (hauteur - static_cast<size_t>(hauteur * ratio)) / 2;

    // Copie les pixels de l'image source vers l'image destination
    // en appliquant le rétrécissement
    for (size_t y = 0; y < hauteur; y++) {
        for (size_t x = 0; x < largeur; x++) {
            RVB pixel_source = image_source.pixels[y][x];

            // Calcule les nouvelles coordonnées avec le ratio appliqué
            size_t x_destination = decalage_x + static_cast<size_t>(x * ratio);
            size_t y_destination = decalage_y + static_cast<size_t>(y * ratio);

            // Vérifie que les coordonnées sont dans les limites de l'image
            if (x_destination < largeur && y_destination < hauteur) {
                image_destination.pixels[y_destination][x_destination] = pixel_source;
            }
        }
    }
}

/**
 * Crée une animation montrant une image qui rétrécit progressivement
 * @param chemin_image Chemin de l'image source
 * @param chemin_destination Dossier de destination
 * @param nb_etapes Nombre d'images dans l'animation
 * 
 * Les images sont sauvegardées dans chemin_destination/images/
 * Le GIF final est sauvegardé dans chemin_destination/gif/
 * L'animation commence avec l'image en taille normale et se termine
 * avec l'image complètement rétrécie
 */
void creer_animation_retrecir(const string& chemin_image, const string& chemin_destination, size_t nb_etapes) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    // Génère chaque étape de l'animation avec un ratio décroissant
    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        retrecir_image(image_source, image_destination, ratio);
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