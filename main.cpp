#include "png_io.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <cmath>
#include <vector>
#include <cassert>

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

double gaussian_function(int x, int y, double sigma) {
    return (1.0 / (2.0 * M_PI * sigma * sigma)) * std::exp(-(x * x + y * y) / (2.0 * sigma * sigma));
}

// TODO
void floutage_gaussien(Image_PNG *image_originale, Image_PNG *image_floue, double sigma) {
    assert(image_originale->largeur == image_floue->largeur);
    assert(image_originale->hauteur == image_floue->hauteur);

    int kernel_size = static_cast<int>(std::ceil(3 * sigma)) + 1;
    double factor_sum = 0;

    for(int rx = -kernel_size/2; rx <= kernel_size/2; rx++) {
        for(int ry = -kernel_size/2; ry <= kernel_size/2; ry++) {
            factor_sum += gaussian_function(rx, ry, sigma);
        }
    }

    for(size_t y = 0; y < image_originale->hauteur; y++) {
        for(size_t x = 0; x < image_originale->largeur; x++) {
            RVB pixel = image_originale->pixels[y][x];

            double r = 0;
            double g = 0;
            double b = 0;

            for(int ry = -kernel_size/2; ry <= kernel_size/2; ry++) {
                for(int rx = -kernel_size/2; rx <= kernel_size/2; rx++) {
                    RVB r_pixel;
                    if (
                        (y + ry) < 0 || (x + rx) < 0 || 
                        (y + ry) >= image_originale->hauteur || 
                        (x + rx) >= image_originale->largeur
                    ) {
                        r_pixel = pixel;
                    } else {
                        r_pixel = image_originale->pixels[y + ry][x + rx];
                    }

                    double factor = gaussian_function(ry, rx, sigma) / factor_sum;

                    r += static_cast<double>(r_pixel.rouge) * factor;
                    g += static_cast<double>(r_pixel.vert)  * factor;
                    b += static_cast<double>(r_pixel.bleu)  * factor;
                }
            }

            image_floue->pixels[y][x].rouge = static_cast<Composante>(r);
            image_floue->pixels[y][x].vert  = static_cast<Composante>(g);
            image_floue->pixels[y][x].bleu  = static_cast<Composante>(b);
        }
    }
}

void creer_fondu_flou(const string fichier_path, size_t nb_etapes, double somme_max) {
    Image_PNG image = charger_PNG(fichier_path);
    size_t lastSlash = fichier_path.find_last_of("/\\");
    size_t lastDot = fichier_path.find_last_of(".");
    string nom_fichier = fichier_path.substr(lastSlash + 1, lastDot - lastSlash - 1);

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
        double somme = (somme_max / nb_etapes) * (i + 1);
        floutage_gaussien(&image, &image_buffer, somme);
        sauver_PNG("out/images/" + nom_fichier + "_" + to_string(i) + ".png", image_buffer);
    }

    if (nb_etapes >= 1) {
        generer_GIF("out/images/" + nom_fichier + "_", "out/gif/" + nom_fichier, 0, nb_etapes - 1, 15, 0);
    }
}


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
        sauver_PNG("out/images/" + nom_fichier + "_" + to_string(nb_etapes - i - 1) + ".png", image_buffer);
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

    // creer_fondu_noir(argv[1], nb_etapes);
    creer_fondu_flou(argv[1], nb_etapes, 10);
}
