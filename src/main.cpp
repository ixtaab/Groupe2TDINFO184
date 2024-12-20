#include "animations.hpp"

/**
 * Structure représentant un paramètre de type entier
 */
struct ParamEntier {
    string nom;
    int valeur;
};

/**
 * Structure représentant un paramètre de type chaîne de caractères
 */
struct ParamChaine {
    string nom;
    string valeur;
};

/**
 * Structure représentant une configuration complète
 */
struct Config {
    string nom_fonction;
    size_t nb_params_entier;
    size_t nb_params_chaine;
    ParamEntier params_entier[10];
    ParamChaine params_chaine[10];
};

/**
 * Recherche un paramètre entier dans la configuration
 * @param config Configuration à parcourir
 * @param nom Nom du paramètre recherché
 * @param defaut Valeur par défaut si le paramètre n'est pas trouvé
 * @return Valeur du paramètre ou valeur par défaut
 */
int trouver_param_entier(const Config& config, const string& nom, int defaut) {
    for(size_t i = 0; i < config.nb_params_entier; i++) {
        if (config.params_entier[i].nom == nom) {
            return config.params_entier[i].valeur;
        }
    }
    return defaut;
}

/**
 * Recherche un paramètre chaîne dans la configuration
 * @param config Configuration à parcourir
 * @param nom Nom du paramètre recherché
 * @param defaut Valeur par défaut si le paramètre n'est pas trouvé
 * @return Valeur du paramètre ou valeur par défaut
 */
string trouver_param_chaine(const Config& config, const string& nom, string defaut) {
    for(size_t i = 0; i < config.nb_params_chaine; i++) {
        if (config.params_chaine[i].nom == nom) {
            return config.params_chaine[i].valeur;
        }
    }
    return defaut;
}

/**
 * Affiche un paramètre entier
 */
void affichage_param_entier(ParamEntier params_entier) {
    cout << params_entier.nom << " = " << params_entier.valeur << "\n";
}

/**
 * Affiche un paramètre chaîne
 */
void affichage_param_chaine(ParamChaine params_chaine) {
    cout << params_chaine.nom << " = " << params_chaine.valeur << "\n";
}

/**
 * Affiche une configuration complète
 */
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

/**
 * Lit une configuration depuis un fichier
 * Format attendu:
 * - Première ligne: nom de la fonction
 * - Lignes suivantes: paramètres
 *   - E... pour les paramètres entiers
 *   - S... pour les paramètres chaîne
 * @param fichier_nom Chemin du fichier de configuration
 * @return Configuration lue
 */
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
                flux >> temp; // Lecture du "="
                flux >> result.params_entier[result.nb_params_entier].valeur;
                result.nb_params_entier++;
            } else if (nom_param[0] == 'S') {
                result.params_chaine[result.nb_params_chaine].nom = nom_param;
                string temp;
                flux >> temp; // Lecture du "="
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

/**
 * Programme principal
 * Lit un fichier de configuration et exécute la fonction demandée
 * avec les paramètres spécifiés
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw runtime_error("Usage: " + string(argv[0]) + " fichier_config");
    }

    Config config = lire_config(argv[1]);
    string nom_fonction = config.nom_fonction;

    // Récupère les paramètres communs
    string chemin_destination = trouver_param_chaine(config, "Schemin_destination", nom_fonction);
    string chemin_image = trouver_param_chaine(config, "Schemin_image", "image.png");
    size_t nb_etapes = trouver_param_entier(config, "Enb_etapes", 5);

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
        size_t intensite_max = trouver_param_entier(config, "Eintensite", 5);
        string method = trouver_param_chaine(config, "Smethod", "alteration");
        
        BruitageMethod bruitage_method;
        if (method == "alteration") bruitage_method = BruitageMethod::ALTERATION_ALEATOIRE;
        else if (method == "remplacement") bruitage_method = BruitageMethod::REMPLACEMENT_ALEATOIRE;
        else throw runtime_error("Invalid bruitage method: " + method);

        creer_animation_fondu_bruitage(
            chemin_image, 
            chemin_destination, 
            nb_etapes, 
            intensite_max, 
            bruitage_method
        );
    } else if (nom_fonction == "creer_animation_fondu_flou") {
        size_t intensite_max = trouver_param_entier(config, "Eintensite_max", 5);
        creer_animation_fondu_flou(
            chemin_image,
            chemin_destination,
            nb_etapes,
            intensite_max
        );
    } else if (nom_fonction == "creer_animation_retrecir") {
        creer_animation_retrecir(
            chemin_image,
            chemin_destination,
            nb_etapes
        );
    } else if (nom_fonction == "creer_animation_masquage") {
        Composante couleur_r = static_cast<Composante>(trouver_param_entier(config, "Ecouleur_r", 0));
        Composante couleur_v = static_cast<Composante>(trouver_param_entier(config, "Ecouleur_v", 0));
        Composante couleur_b = static_cast<Composante>(trouver_param_entier(config, "Ecouleur_b", 0));
        string method = trouver_param_chaine(config, "Smethod", "rideau");
        
        MasquageMethod masquage_method;
        if      (method == "rideau") masquage_method = MasquageMethod::RIDEAU;
        else if (method == "persiennes") masquage_method = MasquageMethod::PERSIENNES;
        else throw runtime_error("Invalid masquage method: " + method);

        creer_animation_masquage(
            chemin_image,
            chemin_destination,
            nb_etapes,
            {couleur_r, couleur_v, couleur_b},
            masquage_method
        );
    } else if (nom_fonction == "creer_animation_transition_noir") {
        string chemin_image_cible = trouver_param_chaine(config, "Schemin_image_cible", "image_cible.png");
        
        creer_animation_transition_noir(
            chemin_image,
            chemin_image_cible,
            chemin_destination,
            nb_etapes
        );
    } else if (nom_fonction == "creer_animation_transition_fondu") {
        string chemin_image_cible = trouver_param_chaine(config, "Schemin_image_cible", "image_cible.png");
        
        creer_animation_transition_fondu(
            chemin_image,
            chemin_image_cible,
            chemin_destination,
            nb_etapes
        );
    } else if (nom_fonction == "creer_animation_transition_glissement") {
        string chemin_image_cible = trouver_param_chaine(config, "Schemin_image_cible", "image_cible.png");
        
        creer_animation_transition_glissement(
            chemin_image,
            chemin_image_cible,
            chemin_destination,
            nb_etapes
        );
    } else if (nom_fonction == "creer_animation_transition_remplacement") {
        string chemin_image_cible = trouver_param_chaine(config, "Schemin_image_cible", "image_cible.png");
        string method = trouver_param_chaine(config, "Smethod", "rideau");
        
        RemplacementMethod remplacement_method;
        if      (method == "aleatoire") remplacement_method = RemplacementMethod::ALEATOIRE;
        else if (method == "persiennes") remplacement_method = RemplacementMethod::PERSIENNES;
        else if (method == "rideau") remplacement_method = RemplacementMethod::RIDEAU;
        else throw runtime_error("Invalid masquage method: " + method);

        creer_animation_transition_remplacement(
            chemin_image,
            chemin_image_cible,
            chemin_destination,
            nb_etapes,
            remplacement_method
        );
    }
}