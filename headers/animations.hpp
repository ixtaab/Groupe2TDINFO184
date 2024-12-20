#pragma once

#include "png_io.hpp"

#include <string>
#include <cassert>
#include <filesystem>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

enum class BruitageMethod {
    ALTERATION_ALEATOIRE,
    REMPLACEMENT_ALEATOIRE
};

enum class MasquageMethod {
    RIDEAU,
    PERSIENNES
};

enum class RemplacementMethod {
    RIDEAU,
    PERSIENNES,
    ALEATOIRE
};

void creer_dossiers_sortie(const string& chemin_base);
string extraire_nom_fichier(const string& chemin_fichier);

void assombrir_image(const Image_PNG& image_source, Image_PNG& image_destination, double ratio);
void convertir_niveaux_gris(const Image_PNG& image_source, Image_PNG& image_destination, double ratio);
void bruiter_image(const Image_PNG& image_source, Image_PNG& image_destination, int intensite);
void flouter_image(const Image_PNG& image_source, Image_PNG& image_destination, Image_PNG& image_tampon, double intensite);
void retrecir_image(const Image_PNG& image_source, Image_PNG& image_destination, double ratio);
void masquer_rideau(const Image_PNG& image_source, Image_PNG& image_destination, size_t ligne_courante, RVB couleur);
void masquer_persiennes(const Image_PNG& image_source, Image_PNG& image_destination, double intensite, size_t separation, size_t decalage, RVB couleur);

void creer_animation_fondu_noir(const string& chemin_image, const string& chemin_destination, size_t nb_etapes);
void creer_animation_fondu_niveaux_gris(const string& chemin_image, const string& chemin_destination, size_t nb_etapes);
void creer_animation_fondu_bruitage(const string& chemin_image, const string& chemin_destination, size_t nb_etapes, double intensite, BruitageMethod method);
void creer_animation_fondu_flou(const string& chemin_image, const string& chemin_destination, size_t nb_etapes, size_t intensite_max);
void creer_animation_retrecir(const string& chemin_image, const string& chemin_destination, size_t nb_etapes);
void creer_animation_masquage(const string& chemin_image, const string& chemin_destination, size_t nb_etapes, RVB couleur, MasquageMethod method);
void creer_animation_transition_noir(const string& chemin_image_source, const string& chemin_image_cible, const string& chemin_destination, size_t nb_etapes);
void creer_animation_transition_fondu(const string& chemin_image_source, const string& chemin_image_cible, const string& chemin_destination, size_t nb_etapes);
void creer_animation_transition_glissement(const string& chemin_image_source, const string& chemin_image_cible, const string& chemin_destination, size_t nb_etapes);
void creer_animation_transition_remplacement(const string& chemin_image_source, const string& chemin_image_cible, const string& chemin_destination, size_t nb_etapes, RemplacementMethod method, size_t separation = 2);