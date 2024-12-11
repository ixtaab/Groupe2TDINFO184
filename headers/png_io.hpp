#pragma once
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <png.h>

#define PNG_SETJMP_NOT_SUPPORTED
#define ERROR 1

using Composante = unsigned char;
struct RVB {
    Composante rouge, vert, bleu;
};

bool operator==(const RVB& pix1, const RVB& pix2);
bool operator!=(const RVB& pix1, const RVB& pix2);

std::ostream& operator<<(std::ostream& os, const RVB& rgb);

struct Image_PNG {
    RVB** pixels;
    size_t hauteur, largeur;
};

Image_PNG creer_PNG(size_t hauteur, size_t largeur);
Image_PNG charger_PNG(const std::string nom_fichier);

void sauver_PNG(const std::string nom_fichier, Image_PNG img);
void afficher_PNG(std::string nom_fichier);
void generer_GIF(
    const std::string& nom_pngs, 
    const std::string& nom_gif, 
    size_t premier, 
    size_t dernier, 
    size_t delay, 
    size_t nb_loop
);