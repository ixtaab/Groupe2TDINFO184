#include "png_io.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>

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


// TODO
// void floutage_gaussien()


// void niveau_gris


// void taille_imagr



// void bruiter_img


//


void assombrir_image(Image_PNG *image_original, Image_PNG *image_assombri, double ratio) {
    size_t hauteur = image_original->hauteur;
    size_t largeur = image_original->largeur;

    for (size_t i = 0; i < hauteur; ++i) {
        for (size_t j = 0; j < largeur; ++j) {
            RVB pixel_original = image_original->pixels[i][j];

            RVB pixel_assombri;
            pixel_assombri.rouge = static_cast<Composante>(pixel_original.rouge * ratio);
            pixel_assombri.vert = static_cast<Composante>(pixel_original.vert * ratio);
            pixel_assombri.bleu = static_cast<Composante>(pixel_original.bleu * ratio);

            image_assombri->pixels[i][j] = pixel_assombri;
        }
    }
}

void creer_fondu_noir(const string fichier_path, size_t nb_etapes) {
    Image_PNG image = charger_PNG(fichier_path);
    size_t lastSlash = fichier_path.find_last_of("/\\");
    size_t lastDot = fichier_path.find_last_of(".");
    string nom_fichier = fichier_path.substr(lastSlash + 1, lastDot - lastSlash - 1 );

    if (!filesystem::exists("out")) {
        filesystem::create_directory("out");
    }

    if (!filesystem::exists("out/images")) {
        filesystem::create_directory("out/images");
    }

    if (!filesystem::exists("out/gif")) {
        filesystem::create_directory("out/gif");
    }

    Image_PNG image_buffer = creer_PNG(image.hauteur, image.largeur);
    
    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = (1.0 / nb_etapes) * i;
        assombrir_image(&image, &image_buffer, ratio);
        sauver_PNG("out/images/" + nom_fichier + "_" + to_string(i) + ".png", image_buffer);
    }

    if (nb_etapes >= 1) {
        generer_GIF("out/images/" + nom_fichier + "_", "out/gif/" + nom_fichier, 0, nb_etapes - 1, 15, 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw runtime_error("USAGE: " + string(argv[0]) + " file");
    }


    size_t nb_etapes;
    cout << "Combien d'étapes pour le fondu ?\n";
    cin >> nb_etapes;
    
    creer_fondu_noir(argv[1], nb_etapes);
}
