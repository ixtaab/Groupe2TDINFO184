#include "animations.hpp"

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

string extraire_nom_fichier(const string& chemin_fichier) {
    size_t derniere_barre = chemin_fichier.find_last_of("/\\");
    size_t dernier_point = chemin_fichier.find_last_of(".");
    return chemin_fichier.substr(derniere_barre + 1, dernier_point - derniere_barre - 1);
}