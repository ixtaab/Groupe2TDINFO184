#include "animations.hpp"

#include <optional>

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