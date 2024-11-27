// Programme démontrant une utilisation simple de la bibliothèque png_io.h fournie (nécessitant la librairie libpng-dev) 
// NOTE : compiler avec l'option -lpng : gpp exemple.cpp -o exemple.exe -lpng
#include "png_io.h"
using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << " arguments : file.png taille output.png \n";
        return -1;
    }

    // décodage des arguments du programme :
    string f_entree = argv[1]; // noms du fichier d'entrée à charger
    size_t taille = static_cast<size_t>(stoul(argv[2])); // taille de l'image à créer
    string f_sortie = argv[3]; // noms du fichier de sortie
    // NOTE : les fonctions sto[T] convertissent des chaines en : 
    // int (stoi), unsigned long (stoul), double (stod), float(stof)

    // chargement de l'image dans le fichier d'entrée donné
    Image_PNG img1 = charger_PNG(f_entree);
    cout << f_entree << " : " << img1.largeur << "x" << img1.hauteur << endl;

    // affichage de l'image dans le fichier d'entrée
    afficher_PNG(f_entree);

    // création de l'image de sortie
    Image_PNG img2 =  creer_PNG( taille, taille*2);

    // coloration des 4 coins en rouge, vert, bleu et blanc :
    img2.pixels[0][0] = {255,255,255}; // haut-gauche : blanc
    img2.pixels[0][2*taille-1] = {0,255,0}; // haut-droite : vert
    img2.pixels[taille-1][0] = {255,50,50}; // bas-gauche : rouge clair
    img2.pixels[taille-1][2*taille-1] = {50,50,255}; // bas-droite : bleu clair

    // enregistrement de l'image dans le fichier de sortie
    sauver_PNG(f_sortie, img2);

    // affichage de l'image dans le fichier d'entrée
    afficher_PNG(f_sortie);
}
