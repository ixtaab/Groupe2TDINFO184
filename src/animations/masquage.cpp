#include "animations.hpp"

/**
 * Applique un effet de rideau sur une image
 * Masque progressivement l'image du haut vers le bas avec une couleur donnée
 * @param image_source Image d'entrée
 * @param image_destination Image de sortie
 * @param ligne_courante Position actuelle du rideau (en pixels depuis le haut)
 * @param couleur Couleur du masque
 * 
 *  @note invariances: 
 * 
 *  - image_source et image_destination doivent avoir les mêmes dimensions. 
 */
void masquer_rideau(
    const Image_PNG& image_source, 
    Image_PNG& image_destination, 
    size_t ligne_courante, 
    RVB couleur
) {
    // Vérifie que les dimensions des images correspondent
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    
    // Copie l'image source dans l'image destination
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            image_destination.pixels[y][x] = image_source.pixels[y][x];
        }
    }
    
    // Applique le masque jusqu'à la ligne courante
    for (size_t y = 0; y <= ligne_courante && y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            image_destination.pixels[y][x] = couleur;
        }
    }
}

/**
 * Applique un effet de persiennes sur une image
 * Crée des lignes verticales avec une couleur et une intensité données
 * @param image_source Image d'entrée
 * @param image_destination Image de sortie
 * @param intensite Intensité de l'effet (entre 0 et 1)
 * @param separation Espacement entre les lignes
 * @param decalage Décalage horizontal des lignes
 * @param couleur Couleur des persiennes
 */
void masquer_persiennes(
    const Image_PNG& image_source, 
    Image_PNG& image_destination, 
    double intensite, 
    size_t separation, 
    size_t decalage, 
    RVB couleur
) {
    // Vérifie que les dimensions des images correspondent
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    
    // Copie l'image source dans l'image destination
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x++) {
            image_destination.pixels[y][x] = image_source.pixels[y][x];
        }
    }
    
    // Calcule la couleur avec l'intensité appliquée
    RVB couleur_actuelle = {
        static_cast<Composante>(couleur.rouge * intensite),
        static_cast<Composante>(couleur.vert * intensite),
        static_cast<Composante>(couleur.bleu * intensite)
    };
    
    // Dessine les lignes verticales
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x += separation) {
            image_destination.pixels[y][min(x + decalage, image_source.largeur - 1)] = couleur_actuelle;
        }
    }
}

/**
 * Crée une animation de masquage d'une image
 * Supporte deux méthodes : rideau et persiennes
 * @param chemin_image Chemin de l'image source
 * @param chemin_destination Dossier de destination
 * @param nb_etapes Nombre d'images dans l'animation
 * @param couleur Couleur du masque
 * @param method Méthode de masquage (RIDEAU ou PERSIENNES)
 * 
 * Les images sont sauvegardées dans chemin_destination/images/
 * Le GIF final est sauvegardé dans chemin_destination/gif/
 */
void creer_animation_masquage(
    const string& chemin_image, 
    const string& chemin_destination, 
    size_t nb_etapes, 
    RVB couleur,
    MasquageMethod method
) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);

    // Génère chaque étape de l'animation
    for(size_t i = 0; i < nb_etapes; i++) {
        switch (method) {
            case MasquageMethod::RIDEAU: {
                size_t ligne_courante = (i * image_source.hauteur) / (nb_etapes - 1);
                masquer_rideau(image_source, image_source, ligne_courante, couleur);
                break;
            }
            case MasquageMethod::PERSIENNES: {
                double intensite = static_cast<double>(i) / (nb_etapes - 1);
                masquer_persiennes(image_source, image_source, intensite, nb_etapes, i, couleur);
            }
        }

        // Sauvegarde l'image de l'étape courante
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_source
        );
    }

    // Génère le GIF final
    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}