#include "png_io.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <cassert>
#include <random>
#include <algorithm>
#include <optional>
#include <omp.h>

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

optional<int> trouver_param_entier(const Config& config, const string& nom) {
    for(size_t i = 0; i < config.nb_params_entier; i++) {
        if (config.params_entier[i].nom == nom) {
            return config.params_entier[i].valeur;
        }
    }
    return nullopt;
}

optional<string> trouver_param_chaine(const Config& config, const string& nom) {
    for(size_t i = 0; i < config.nb_params_chaine; i++) {
        if (config.params_chaine[i].nom == nom) {
            return config.params_chaine[i].valeur;
        }
    }
    return nullopt;
}

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

void assombrir_image(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    for (size_t y = 0; y < image_source.hauteur; ++y) {
        for (size_t x = 0; x < image_source.largeur; ++x) {
            RVB pixel_source = image_source.pixels[y][x];
            image_destination.pixels[y][x] = {
                static_cast<Composante>(pixel_source.rouge * ratio),
                static_cast<Composante>(pixel_source.vert  * ratio),
                static_cast<Composante>(pixel_source.bleu  * ratio)
            };
        }
    }
}

void convertir_niveaux_gris(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    for (size_t y = 0; y < image_source.hauteur; ++y) {
        for (size_t x = 0; x < image_source.largeur; ++x) {
            RVB pixel = image_source.pixels[y][x];
            double luminosite = pixel.rouge * 0.299 + pixel.vert * 0.587 + pixel.bleu * 0.114;
            image_destination.pixels[y][x] = {
                static_cast<Composante>(pixel.rouge * ratio + luminosite * (1-ratio)),
                static_cast<Composante>(pixel.vert  * ratio + luminosite * (1-ratio)),
                static_cast<Composante>(pixel.bleu  * ratio + luminosite * (1-ratio))
            };
        }
    }
}

void bruiter_image(const Image_PNG& image_source, Image_PNG& image_destination, int intensite) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    // TODO: a completer pour la consigne du projet
    for(size_t y = 0; y < image_source.hauteur; y++) {
        for(size_t x = 0; x < image_source.largeur; x++) {
            RVB pixel_src = image_source.pixels[y][x];
            image_destination.pixels[y][x].rouge = clamp(rand() % intensite - intensite/2 + pixel_src.rouge, 0, 255);
            image_destination.pixels[y][x].vert  = clamp(rand() % intensite - intensite/2 + pixel_src.vert , 0, 255);
            image_destination.pixels[y][x].bleu  = clamp(rand() % intensite - intensite/2 + pixel_src.bleu , 0, 255);
        }
    }
}

double gaussian_function(int x, int y, double sigma) {
    return (1.0 / (2.0 * M_PI * sigma * sigma)) * std::exp(-(x * x + y * y) / (2.0 * sigma * sigma));
}

void flouter_image(
    const Image_PNG& image_source, 
    Image_PNG& image_destination, 
    Image_PNG& image_tampon, 
    double intensite
) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    assert(image_source.largeur == image_tampon.largeur);
    assert(image_source.hauteur == image_tampon.hauteur);

    size_t hauteur = image_source.largeur;
    size_t largeur = image_source.largeur;

    int kernel_size = static_cast<int>(std::ceil(3 * intensite)) + 1;

    double factor_sum = 0;
    for(int rx = -kernel_size/2; rx <= kernel_size/2; rx++) {
        factor_sum += gaussian_function(rx, 0, intensite);
    }

    #pragma omp parallel for
    for(size_t y = 0; y < hauteur; y++) {
        for(size_t x = 0; x < largeur; x++) {
            double r = 0;
            double g = 0;
            double b = 0;

            for(int ry = -kernel_size/2; ry <= kernel_size/2; ry++) {
                int resolved_y = y + ry;

                if (resolved_y < 0) resolved_y = y - ry;
                if (resolved_y >= static_cast<int>(hauteur)) resolved_y = y - ry;

                RVB r_pixel = image_source.pixels[resolved_y][x];

                double factor = gaussian_function(0, ry, intensite) / factor_sum;

                r += static_cast<double>(r_pixel.rouge) * factor;
                g += static_cast<double>(r_pixel.vert)  * factor;
                b += static_cast<double>(r_pixel.bleu)  * factor;
            }

            image_tampon.pixels[y][x].rouge = static_cast<Composante>(r);
            image_tampon.pixels[y][x].vert  = static_cast<Composante>(g);
            image_tampon.pixels[y][x].bleu  = static_cast<Composante>(b);
        }
    }

    #pragma omp parallel for
    for(size_t y = 0; y < hauteur; y++) {
        for(size_t x = 0; x < largeur; x++) {
            double r = 0;
            double g = 0;
            double b = 0;

            for(int rx = -kernel_size/2; rx <= kernel_size/2; rx++) {
                int resolved_x = x + rx;

                if (resolved_x < 0) resolved_x = x - rx;
                if (resolved_x >= static_cast<int>(largeur)) resolved_x = x - rx;

                RVB r_pixel = image_tampon.pixels[y][resolved_x];

                double factor = gaussian_function(rx, 0, intensite) / factor_sum;

                r += static_cast<double>(r_pixel.rouge) * factor;
                g += static_cast<double>(r_pixel.vert)  * factor;
                b += static_cast<double>(r_pixel.bleu)  * factor;
            }

            image_destination.pixels[y][x].rouge = static_cast<Composante>(r);
            image_destination.pixels[y][x].vert  = static_cast<Composante>(g);
            image_destination.pixels[y][x].bleu  = static_cast<Composante>(b);
        }
    }
}

void retrecir_image(const Image_PNG& image_source, Image_PNG& image_destination, double ratio) {
    assert(ratio >= 0.0 && ratio <= 1.0);
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);

    size_t hauteur = image_source.hauteur;
    size_t largeur = image_source.largeur;

    for (size_t y = 0; y < hauteur; y++) {
        for (size_t x = 0; x < largeur; x++) {
            image_destination.pixels[y][x] = {};
        }
    }

    size_t decalage_x = (largeur - static_cast<size_t>(largeur * ratio)) / 2;
    size_t decalage_y = (hauteur - static_cast<size_t>(hauteur * ratio)) / 2;

    for (size_t y = 0; y < hauteur; y++) {
        for (size_t x = 0; x < largeur; x++) {
            RVB pixel_source = image_source.pixels[y][x];

            size_t x_destination = decalage_x + static_cast<size_t>(x * ratio);
            size_t y_destination = decalage_y + static_cast<size_t>(y * ratio);

            if (x_destination < largeur && y_destination < hauteur) {
                image_destination.pixels[y_destination][x_destination] = pixel_source;
            }
        }
    }
}

void creer_animation_fondu_noir(const string& chemin_image, const string& chemin_destination, size_t nb_etapes) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        assombrir_image(image_source, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}

void creer_animation_fondu_niveaux_gris(const string& chemin_image, const string& chemin_destination, size_t nb_etapes) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        convertir_niveaux_gris(image_source, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}

void creer_animation_fondu_bruitage(
    const string& chemin_image, 
    const string& chemin_destination, 
    size_t nb_etapes, 
    double intensite_max
) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double intensite = (intensite_max / nb_etapes) * (i + 1);
        bruiter_image(image_source, image_destination, intensite);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}

void creer_animation_fondu_flou(
    const string& chemin_image, 
    const string& chemin_destination, 
    size_t nb_etapes, 
    size_t intensite_max
) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_tampon = creer_PNG(image_source.hauteur, image_source.largeur);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double intensite = (intensite_max / nb_etapes) * (i + 1);
        flouter_image(image_source, image_tampon, image_destination, intensite);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_tampon
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}

void creer_animation_retrecir(const string& chemin_image, const string& chemin_destination, size_t nb_etapes) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);
    Image_PNG image_destination = creer_PNG(image_source.hauteur, image_source.largeur);

    for(size_t i = 0; i < nb_etapes; i++) {
        double ratio = 1.0 - (static_cast<double>(i) / (nb_etapes - 1));
        retrecir_image(image_source, image_destination, ratio);
        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_destination
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}

void masquer_rideau(
    const Image_PNG& image_source, 
    Image_PNG& image_destination, 
    size_t ligne_courante, 
    RVB couleur
) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    
    for (size_t y = 0; y < image_source.hauteur; ++y) {
        for (size_t x = 0; x < image_source.largeur; ++x) {
            image_destination.pixels[y][x] = image_source.pixels[y][x];
        }
    }
    
    for (size_t y = 0; y <= ligne_courante && y < image_source.hauteur; ++y) {
        for (size_t x = 0; x < image_source.largeur; ++x) {
            image_destination.pixels[y][x] = couleur;
        }
    }
}

void masquer_persiennes(
    const Image_PNG& image_source, 
    Image_PNG& image_destination, 
    double intensite, 
    size_t separation, 
    size_t decalage, 
    RVB couleur
) {
    assert(image_source.largeur == image_destination.largeur);
    assert(image_source.hauteur == image_destination.hauteur);
    
    for (size_t y = 0; y < image_source.hauteur; ++y) {
        for (size_t x = 0; x < image_source.largeur; ++x) {
            image_destination.pixels[y][x] = image_source.pixels[y][x];
        }
    }
    
    RVB couleur_actuelle = {
        static_cast<Composante>(couleur.rouge * intensite),
        static_cast<Composante>(couleur.vert * intensite),
        static_cast<Composante>(couleur.bleu * intensite)
    };
    
    for (size_t y = 0; y < image_source.hauteur; y++) {
        for (size_t x = 0; x < image_source.largeur; x += separation) {
            image_destination.pixels[y][min(x + decalage, image_source.largeur - 1)] = couleur_actuelle;
        }
    }
}

void creer_animation_masquage(
    const string& chemin_image, 
    const string& chemin_destination, 
    size_t nb_etapes, 
    RVB couleur, 
    bool mode_rideau, 
    size_t K = 1
) {
    creer_dossiers_sortie(chemin_destination);

    string nom_image = extraire_nom_fichier(chemin_image);
    Image_PNG image_source = charger_PNG(chemin_image);

    for(size_t i = 0; i < nb_etapes; i++) {
        if (mode_rideau) {
            size_t ligne_courante = (i * image_source.hauteur) / (nb_etapes - 1);
            masquer_rideau(image_source, image_source, ligne_courante, couleur);
        } else {
            double intensite = static_cast<double>(i) / (nb_etapes - 1);
            masquer_persiennes(image_source, image_source, intensite, K, i, couleur);
        }

        sauver_PNG(
            chemin_destination + "/images/" + nom_image + "_" +
            to_string(i) + ".png", image_source
        );
    }

    generer_GIF(
        chemin_destination + "/images/" + nom_image + "_",
        chemin_destination + "/gif/" + nom_image,
        0, nb_etapes - 1, 15, 0
    );
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw runtime_error("Usage: " + string(argv[0]) + " fichier_config");
    }

    Config config = lire_config(argv[1]);
    string nom_fonction = config.nom_fonction;

    string chemin_destination = trouver_param_chaine(config, "Schemin_destination").value_or(nom_fonction);
    string chemin_image = trouver_param_chaine(config, "Schemin_image").value_or("image.png");
    size_t nb_etapes = trouver_param_entier(config, "Enb_etapes").value_or(5);

    if (nom_fonction == "creer_animation_fondu_noir") {
        creer_animation_fondu_noir(
            chemin_image,
            chemin_destination,
            nb_etapes
        );
    } else if (nom_fonction == "creer_animation_fondu_niveaux_gris") {
        creer_animation_fondu_niveaux_gris(
            chemin_image,
            chemin_destination,
            nb_etapes
        );
    } else if (nom_fonction == "creer_animation_fondu_bruitage") {
        optional<size_t> intensite_max = trouver_param_entier(config, "Eintensite_max");
        creer_animation_fondu_bruitage(
            chemin_image,
            chemin_destination,
            nb_etapes,
            intensite_max.value_or(5)
        );
    } else if (nom_fonction == "creer_animation_fondu_flou") {
        optional<size_t> intensite_max = trouver_param_entier(config, "Eintensite_max");
        creer_animation_fondu_flou(
            chemin_image,
            chemin_destination,
            nb_etapes,
            intensite_max.value_or(5)
        );
    } else if (nom_fonction == "creer_animation_retrecir") {
        creer_animation_retrecir(
            chemin_image,
            chemin_destination,
            nb_etapes
        );
    } else if (nom_fonction == "creer_animation_masquage") {
        Composante couleur_r = static_cast<Composante>(trouver_param_entier(config, "Ecouleur_r").value_or(5));
        Composante couleur_v = static_cast<Composante>(trouver_param_entier(config, "Ecouleur_v").value_or(5));
        Composante couleur_b = static_cast<Composante>(trouver_param_entier(config, "Ecouleur_b").value_or(5));

        creer_animation_masquage(
            chemin_image,
            chemin_destination,
            nb_etapes,
            {couleur_r, couleur_v, couleur_b},
            true,
            15
        );
    }
}