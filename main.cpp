#include "png_io.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

struct ParamEntier {
    string nom;
    int valeur;
};

struct ParamChaine {
    string nom, valeur;
};

struct Config {
    string nom_fonction;
    size_t nb_params_entier, nb_params_chaine;
    ParamEntier params_entier[10];
    ParamChaine params_chaine[10];
};

void affichage_param_entier(ParamEntier params_entier) {
    cout << params_entier.nom << " = " << params_entier.valeur << "\n";
}

void affichage_param_chaine(ParamChaine params_chaine) {
    cout << params_chaine.nom << " = " << params_chaine.valeur << "\n";
}

void affichage_config(Config config) {
    cout << "fonction " << config.nom_fonction << ":\n";

    for (size_t i = 0; i < config.nb_params_entier; i++) {
        cout << "  ";
        affichage_param_entier(config.params_entier[i]);
    }

    for (size_t i = 0; i < config.nb_params_chaine; i++) {
        cout << "  ";
        affichage_param_chaine(config.params_chaine[i]);
    }
}

Config lire_config(string fichier_nom) {
    Config result;
    result.nb_params_chaine = 0;
    result.nb_params_entier = 0;
    result.nom_fonction = "";

    fstream flux;
    flux.open(fichier_nom, ios::in);
    if (flux.is_open()) {
        flux >> result.nom_fonction;
        while (flux.good()) {
            string nom_param;
            flux >> nom_param;
            if (nom_param[0] == 'E') {
                result.params_entier[result.nb_params_entier].nom = nom_param;
                string temp;
                flux >> temp;
                flux >> result.params_entier[result.nb_params_entier].valeur;
                result.nb_params_entier++;
            } else if (nom_param[0] == 'S') {
                result.params_chaine[result.nb_params_chaine].nom = nom_param;
                string temp;
                flux >> temp;
                flux >> result.params_chaine[result.nb_params_chaine].valeur;
                result.nb_params_chaine++;
            } else {
                cout << "Erreur : parametre invalide: " << nom_param << endl;
            }
        }
        flux.close();
    } else {
        cout << "Erreur : impossible d'ouvrir " << fichier_nom << endl;
    }
    return result;
}

void assombrir_image(Image_PNG &img, const string &nom_fichier, double ratio, size_t etape, size_t nb_etapes) {
    size_t hauteur = img.hauteur;
    size_t largeur = img.largeur;

    // Calcul du facteur d'assombrissement pour cette étape
    double facteur = 1.0 - (ratio * etape);

    // Application du facteur d'assombrissement pixel par pixel
    for (size_t i = 0; i < hauteur; ++i) {
        for (size_t j = 0; j < largeur; ++j) {
            RVB &pixel = img.pixels[i][j];
            pixel.rouge = static_cast<Composante>(pixel.rouge * facteur);
            pixel.vert = static_cast<Composante>(pixel.vert * facteur);
            pixel.bleu = static_cast<Composante>(pixel.bleu * facteur);
        }
    }

    // Sauvegarde de l'image de cette étape
    string nom_image_assombri = "image/assombri_" + to_string(etape) + ".png";
    sauver_PNG(nom_image_assombri, img);
}

void creer_etape_fondu_noir(Image_PNG &img, size_t nb_etape, const string &fname) {
    double ratio = 1.0 / nb_etape;

    for (size_t i = 1; i <= nb_etape; ++i) {
        assombrir_image(img, fname, ratio, i, nb_etape);
    }

    // Génération du GIF animé à partir des images sauvegardées
    if (nb_etape > 1) {
        generer_GIF("image/assombri_", "animation/animation_assombrissement", 1, nb_etape, 15, 0);
        cout << "GIF animé généré : animation/animation_assombrissement.gif" << endl;
    }
}

void creer_fondu_noir(const string &nom_fichier_source) {
    Image_PNG img = charger_PNG(nom_fichier_source);
    string fname = nom_fichier_source;
    size_t nb_etape;

    cout << "Combien d'étapes pour le fondu ?";
    cin >> nb_etape;
    creer_etape_fondu_noir(img, nb_etape, fname);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw runtime_error("USAGE: " + string(argv[0]) + " file");
    }

    creer_fondu_noir(argv[1]);

    return 0;
}
