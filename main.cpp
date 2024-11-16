#include "pngio.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct ParamEntier{
    string nom;
    int valeur;   
};

struct ParamChaine{
    string nom,valeur;  
};

struct Config{
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
            } else if(nom_param[0] == 'S') {
                result.params_chaine[result.nb_params_chaine].nom = nom_param;
                string temp;
                flux >> temp;
                flux >> result.params_chaine[result.nb_params_chaine].valeur;
                result.nb_params_chaine++;
            } else {
                cout << "Erreur : parametre invalid: " << nom_param << endl;
            }
        }
        flux.close();
    } else {
        cout << "Erreur : impossible d'ouvrir " << fichier_nom << endl;
    }
}




void assombrir_image(ImageRGB &img_png,string fichier_nom, double ratio_reel, size_t nb_etape) {

    ImageRGB img_assombri;

    img_assombri.width = img_png.width;
    img_assombri.height = img_png.height;

    for (size_t i = 0; i < img_png.height; ++i) {
        for (size_t j = 0; j < img_png.width; ++j) {
            uint8_t r = get<0>(img_png.data[i * img_png.width + j]) * ratio_reel;
            uint8_t g = get<1>(img_png.data[i * img_png.width + j]) * ratio_reel;
            uint8_t b = get<2>(img_png.data[i * img_png.width + j]) * ratio_reel;

           img_assombri.data.push_back({r,g,b});
        }

    }
    writepng(" assombri_" + to_string(nb_etape)+"_" + fichier_nom, img_assombri);

}

int creer_etape_fondu_noir(ImageRGB &img, size_t nb_etape, string fname){


    double ratio = 1.0/nb_etape;
    
    for(int i = 1; i < nb_etape; i++){
            double coef = 1.0 - (ratio * i);

            assombrir_image(img,fname,coef,i);

        }
        assombrir_image(img,fname,0,nb_etape);
}


void creer_fondu_noir(string nom_fichier_source){ 

    ImageRGB img = readpng(nom_fichier_source);
    string fname = nom_fichier_source;
    size_t nb_etape;

    cout << "Combien d'étape pour le fondu ?";
    cin >> nb_etape;
    creer_etape_fondu_noir(img,nb_etape,fname);
}




int main(int argc, char * argv[]) {
    try {
        if (argc != 2) {
            throw runtime_error("USAGE: " + string(argv[0]) + " file");
        }


        creer_fondu_noir(argv[1]);
   
    } 
    
    catch (exception & e) {
        cout << "Error: " << e.what() << endl;
    }

    cout << "c'est magnifique" << endl;
    return 0;
} 


