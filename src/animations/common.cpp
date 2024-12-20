#include "animations.hpp"

/**
 * Crée les dossiers nécessaires pour stocker les fichiers de sortie
 * @param chemin_base Le chemin où créer les dossiers
 * Crée 3 dossiers : 
 * - chemin_base
 * - chemin_base/images
 * - chemin_base/gif
 */
void creer_dossiers_sortie(const string& chemin_base) {
    if (!filesystem::exists(chemin_base)) {
        filesystem::create_directory(chemin_base);
    }
    if (!filesystem::exists(chemin_base + "/images")) {
        filesystem::create_directory(chemin_base + "/images");
    }
    if (!filesystem::exists(chemin_base + "/gif")) {
        filesystem::create_directory(chemin_base + "/gif");
    }
}

/**
 * Extrait le nom d'un fichier depuis son chemin complet
 * @param chemin_fichier Le chemin complet du fichier
 * @return Le nom du fichier sans extension
 * Exemple : "/dossier/image.jpg" retourne "image"
 */
string extraire_nom_fichier(const string& chemin_fichier) {
    size_t derniere_barre = chemin_fichier.find_last_of("/\\");
    size_t dernier_point = chemin_fichier.find_last_of(".");
    return chemin_fichier.substr(derniere_barre + 1, dernier_point - derniere_barre - 1);
}