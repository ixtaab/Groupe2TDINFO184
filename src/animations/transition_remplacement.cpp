#include "animations.hpp"

/**
 * Crée une transition de type rideau entre deux images
 * L'image cible remplace progressivement l'image source du haut vers le bas
 * 
 * @param image_source Image initiale
 * @param image_cible Image qui remplace progressivement l'image source
 * @param image_destination Image résultante
 * @param progression Avancement de la transition (entre 0 et 1)
 *        0.0 = uniquement image source visible
 *        1.0 = uniquement image cible visible
 * 
 * @note invariances: 
 * 
 *  - image_source, image_cible et image_destination doivent avoir les mêmes dimensions. 
 */
void transition_rideau(
    const Image_PNG& image_source, 
    const Image_PNG& image_cible, 
    Image_PNG& image_destination, 
    double progression
) {
    // Copie d'abord l'image source entièrement
    for (size_t y = 0; y < image_destination.hauteur; y++) {
        for (size_t x = 0; x < image_destination.largeur; x++) {
            image_destination.pixels[y][x] = image_source.pixels[y][x];
        }
    }

    // Remplace progressivement par l'image cible du haut vers le bas
    size_t ligne_limite = static_cast<size_t>(progression * image_source.hauteur);
    for (size_t y = 0; y < ligne_limite && y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            image_destination.pixels[y][x] = image_cible.pixels[y][x];
        }
    }
}

/**
 * Crée une animation de transition entre deux images
 * Supporte différentes méthodes de remplacement (actuellement uniquement rideau)
 * 
 * @param chemin_image_source Chemin de l'image initiale
 * @param chemin_image_cible Chemin de l'image finale
 * @param chemin_destination Dossier de destination
 * @param nb_etapes Nombre d'images dans l'animation
 * @param method Méthode de remplacement à utiliser
 * @param separation Paramètre de séparation (non utilisé actuellement)
 * 
 * Les images sont sauvegardées dans chemin_destination/images/
 * Le GIF final est sauvegardé dans chemin_destination/gif/
 * Si nb_etapes = 1, seule l'image finale est générée (pas de GIF)
 */
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

    // Génère chaque étape de l'animation
    for(size_t i = 0; i < nb_etapes; i++) {
        // Calcule la progression (évite la division par zéro si nb_etapes = 1)
        double progression = (nb_etapes > 1) 
                            ? static_cast<double>(i) / (nb_etapes - 1) 
                            : 1.0;

        transition_rideau(image_source, image_cible, image_destination, progression);

        // Sauvegarde l'image de l'étape courante
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    // Génère le GIF uniquement si plus d'une étape
    if (nb_etapes > 1) {
        generer_GIF(
            chemin_destination + "/images/" + nom_image + "_",
            chemin_destination + "/gif/" + nom_image,
            0, nb_etapes - 1, 15, 0
        );
    }
}