#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <png.h>

/********************* MACROS LIÉES À LA BIBLIOTHÈQUE PNG ********************/
#define PNG_SETJMP_NOT_SUPPORTED // évite une gestion d'erreur complexe au sein de la libpng
#define ERROR 1 // code d'erreur en cas de problème à la lecture du fichier png

/*************** TYPES POUR LA MANIPULATION D'UNE IMAGE PNG ******************/

// Type RVB : une couleur sur 3 octets (0..255)
using Composante = unsigned char;
struct RVB {
	Composante rouge, vert, bleu;
};
// Opérateurs de comparaisons de RVB
inline bool operator==(const RVB & pix1, const RVB & pix2) {
    return (pix1.rouge==pix2.rouge) && (pix1.vert==pix2.vert) && (pix1.bleu==pix2.bleu);
}
inline bool operator!=(const RVB & pix1, const RVB & pix2) {
    return !(pix1==pix2);
}
// Opérateur d'affichage d'un RVB
inline std::ostream& operator<<(std::ostream &os, const RVB & rgb){
	return os<<"["<<+rgb.rouge <<","<<+rgb.vert <<","<<+rgb.bleu<<"]";
}

// Type Image_PNG
// la matrice des pixels représentant une image. Le champ *pixels* est 
// utilisable comme un tableau de (hauteur) lignes contenant chacune 
// (largeur) RGB accessibles via pixels[indice_ligne][indice_colonne]
struct Image_PNG {
	RVB ** pixels;
	size_t hauteur, largeur;
};

/*****************************************************************************/ 
/************************ SIGNATURES DES SOUS ALGORITHMES*********************/ 
/*****************************************************************************/ 

/***************** FONCTION DE CRÉATION D'UNE IMAGE PNG NOIRE ****************/
// RÔLE : retourne une Image_PNG vierge (noire) de hauteur et largeur spécifiée
Image_PNG creer_PNG(size_t hauteur, size_t largeur );

/******************** FONCTION DE LECTURE D'UNE IMAGE PNG ********************/
// RÔLE : retourne une Image_PNG correspondant au contenu du fichier "nom_fichier"
// NOTE : cette fonction cause l'abandon du programme qui l'invoque si l'une des erreurs suivantes se produit :
// - nom_fichier ne correspond pas à un fichier existant
// - nom_fichier n'est pas le nom d'un fichier contenant une image PNG
// - nom_fichier est le nom d'un fichier contenant une image PNG dont l'entête est corrompu
// - l'allocation des structures de lecture d'image est impossible
Image_PNG charger_PNG(const std::string nom_fichier) ;

/******************** PROCÉDURE D'ÉCRITURE D'UNE IMAGE PNG ********************/
// RÔLE : écrit dans le fichier "nom_fichier" l'Image_PNG img
// NOTE : cette procédure cause l'abandon du programme qui l'invoque si l'une des erreurs suivantes se produit :
// - nom_fichier correspond à un fichier qui ne peut être ouvert/créé
// - l'allocation des structures d'écriture d'image est impossible
void sauver_PNG(const std::string nom_fichier, Image_PNG img) ;


/******************* PROCÉDURE D'AFFICHAGE D'UNE IMAGE PNG *******************/
// RÔLE : affiche à l'écran le contenu du fichier nom_fichier
// NOTE : utilise la commande "display" du terminal ; provoque une erreur
// si la commande n'est pas disponible ou que le fichier ne contient pas une image
inline void afficher_PNG(std::string nom_fichier ) ;

/******************* PROCÉDURE DE CREATION D'UN GIF ANIME  DEPUIS UNE SERIE D'IMAGES PNG *******************/
// RÔLE : crée un fichier nom_gif.gif contenant le GIF animé résultant d'une série d'images png portant 
// le même nom nom_pngs et un suffixe numérique dans un intervalle donné [premier,dernier] ; dans l'animation, 
// chaque image restera affichée delay millisecondes ; l'animation sera répétée nb_loop fois (0=à l'infini)
// NOTE : utilise la commande "convert" du terminal ; provoque une erreur si la commande n'est pas disponible,
// ou que les fichiers png n'existent pas 
void generer_GIF(const std::string & nom_pngs, const std::string & nom_gif, size_t premier, size_t dernier, size_t delay, size_t nb_loop ) ;


/*****************************************************************************/ 
/************************ DEFINITION DES SOUS ALGORITHMES*********************/ 
/*****************************************************************************/ 

/***************** FONCTION DE CRÉATION D'UNE IMAGE PNG NOIRE ****************/
// RÔLE : retourne une Image_PNG vierge (noire) de hauteur et largeur spécifiée
Image_PNG creer_PNG(size_t hauteur, size_t largeur ) {
    assert((hauteur>0) and (largeur>0)); // précondition : image non vide
	Image_PNG img = {NULL, hauteur, largeur}; // l'image à retourner
	png_bytep *rows = (png_bytep*)malloc(img.hauteur*sizeof(png_bytep)); // allocation des lignes
	for (size_t i=0; i< img.hauteur; ++i) {
        rows[i] = (png_bytep)malloc(3*img.largeur*sizeof(png_byte)); // allocation des colonnes
        for (size_t j=0; j<3*img.largeur; ++j) {
            rows[i][j] = 0; // composante noire
        }
    }
    img.pixels = reinterpret_cast<RVB**>(rows); // transtypage en RVB
	return img;
}

/******************** FONCTION DE LECTURE D'UNE IMAGE PNG ********************/
// RÔLE : retourne une Image_PNG correspondant au contenu du fichier "nom_fichier"
// NOTE : cette fonction cause l'abandon du programme qui l'invoque si l'une des erreurs suivantes se produit :
// - nom_fichier ne correspond pas à un fichier existant
// - nom_fichier n'est pas le nom d'un fichier contenant une image PNG
// - nom_fichier est le nom d'un fichier contenant une image PNG dont l'entête est corrompu
// - l'allocation des structures de lecture d'image est impossible
Image_PNG charger_PNG(const std::string nom_fichier) {
	Image_PNG img; // l'image à retourner
	size_t i,j,k; // des variables de boucles
    FILE *fp; // le fichier png
	// les structures de stockage d'image png :
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *rows; // matrice des octets de l'image
    int bit_depth; // nombre de bits utilisés par composantes = 1, 2, 4, 8 ou 16 ; nous travaillerons uniquement avec des images pour lesquelles cette donnée vaut 8
    int color_type; // type de couleurs codées = PNG_COLOR_TYPE_GRAY | PNG_COLOR_TYPE_GRAY_ALPHA | PNG_COLOR_TYPE_PALETTE | PNG_COLOR_TYPE_RGB | PNG_COLOR_TYPE_RGB_ALPHA | PNG_COLOR_MASK_PALETTE | PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA ; nous travaillerons  uniquement avec des images pour lesquelles cette donnée vaut PNG_COLOR_TYPE_PALETTE | PNG_COLOR_TYPE_RGB

	// ouverture du fichier
	fp = fopen(nom_fichier.c_str(), "rb");
    if (!fp) {
		std::cerr << "ERREUR : fichier " << nom_fichier << " impossible à lire" << std::endl;
   	    abort();
	}

	// allocation des structures de stockage
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
		std::cerr << "ERREUR : allocation des structures de lecture d'image impossible" << std::endl;
       	abort();
	}
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
		png_destroy_read_struct(&png_ptr,(png_infopp)NULL, (png_infopp)NULL);
		std::cerr << "ERREUR : allocation des structures de lecture d'image impossible" << std::endl;
		abort();
    }

	// initialisation des structures de stockage
	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	// récupérations des données relatives à l'image
	img.largeur = png_get_image_width(png_ptr, info_ptr);
	img.hauteur = png_get_image_height(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	// si les composantes sont codées sur 1, 2, 4 ou 16 bits, on les ramène à 8 bits
    if (bit_depth < 8) png_set_packing(png_ptr);
    if (bit_depth == 16) png_set_strip_16(png_ptr);
	// si l'image utilise une palette, on la transforme en rgb
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);

	// récupération des pixels de l'image : ils sont stockés dans une matrice d'octets de
	// taille hauteur X 3*largeur puisque chaque pixel occupe 3 octets
	rows = (png_bytep*)malloc(img.hauteur*sizeof(png_bytep));
	for (i=0; i< img.hauteur; ++i) {
        rows[i] = (png_bytep)malloc(3*img.largeur*sizeof(png_byte));
    }

    if (color_type == PNG_COLOR_TYPE_RGBA) { // négliger la composante alpha (transparence)
    	for (i=0, j=0, k=0; i<img.hauteur; (j==4*img.largeur)?j=0,k=0,++i:++j) {
    	    if (j %4 <3) {
                rows[i][k++] = png_get_rows(png_ptr,info_ptr)[i][j];
            }
        }
    }
    else {
        // // incorrect
	    // for (i=0, j=0; i<img.hauteur; (j==3*img.largeur)?j=0,++i:++j) {
        //    rows[i][j] = png_get_rows(png_ptr,info_ptr)[i][j];
        // }

        // // correct
        // for (i=0, j=0; i<img.hauteur; (j==(3*img.largeur-1))?j=0,++i:++j) {
        //     rows[i][j] = png_get_rows(png_ptr,info_ptr)[i][j];
        // }

        // correct
        for (size_t i = 0; i < img.hauteur; i++) {
            for (size_t j = 0; j < 3 * img.largeur; j++) {
                rows[i][j] = png_get_rows(png_ptr, info_ptr)[i][j];
            }
        }
    }
	img.pixels = reinterpret_cast<RVB**>(rows); // transtypage

	// libèration de la mémoire allouée pour les structures de stockage
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	// clôture le fichier
	fclose(fp);

	return img;
}

/******************** PROCÉDURE D'ÉCRITURE D'UNE IMAGE PNG ********************/
// RÔLE : écrit dans le fichier "nom_fichier" l'Image_PNG img
// NOTE : cette procédure cause l'abandon du programme qui l'invoque si l'une des erreurs suivantes se produit :
// - nom_fichier correspond à un fichier qui ne peut être ouvert/créé
// - l'allocation des structures d'écriture d'image est impossible
void sauver_PNG(const std::string nom_fichier, Image_PNG img) {
    FILE *fp; // le fichier png
    // les structures de stockage d'image png :
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *rows;

    // ouverture du fichier
    fp = fopen(nom_fichier.c_str(), "wb");
    if (!fp) {
        std::cerr << "ERREUR : fichier " << nom_fichier << "impossible à ouvrir/creer" << std::endl;
        abort();
    }

    // allocation des structures de stockage
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        std::cerr << "ERREUR : allocation des structures d'écriture d'image impossible" << std::endl;
        abort();
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
        std::cerr << "ERREUR : allocation des structures d'écriture d'image impossible" << std::endl;
        abort();
    }

    // remplissage des données de l'image
    png_set_IHDR(png_ptr, info_ptr, static_cast<unsigned int>(img.largeur), static_cast<unsigned int>(img.hauteur), 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    rows = reinterpret_cast<png_bytep*>(img.pixels); // transtypage

    // copie de la matrice de pixels dans les structures
    png_set_rows(png_ptr, info_ptr, rows);

    // écriture de l'image dans le fichier
    png_init_io(png_ptr, fp);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    // libèration de la mémoire allouée pour les structures de stockage
    png_destroy_write_struct(&png_ptr, &info_ptr);

    // clôture du fichier
    fclose(fp);
}

/******************* PROCÉDURE D'AFFICHAGE D'UNE IMAGE PNG *******************/
// RÔLE : affiche à l'écran le contenu du fichier nom_fichier
// NOTE : utilise la commande "display" du terminal ; provoque une erreur
// si la commande n'est pas disponible ou que le fichier ne contient pas une image
inline void afficher_PNG(std::string nom_fichier ) {
#ifdef __APPLE__
    std::system(("open "+nom_fichier).c_str());
#else
    std::system(("display -immutable -frame 1 -resize '100<' -resize 'x100<' -resize '800x600>' "+nom_fichier+" &").c_str());
#endif
}

/******************* PROCÉDURE DE CREATION D'UN GIF ANIME DEPUIS UNE SERIE D'IMAGES PNG *******************/
// RÔLE : crée le Gif animé résultant d'une série d'images png portant le même nom et un suffixe numérique dans un intervalle donné
// PRE : premier < dernier  et nom_fichier sans espaces ou caractères spéciaux
// NOTE : utilise la commande "convert" du terminal ; provoque une erreur
// si la commande n'est pas disponible ou que les fichiers png n'existent pas 
void generer_GIF(const std::string & nom_pngs, const std::string & nom_gif, size_t premier, size_t dernier, size_t delay, size_t loop ) {
   assert(premier<dernier);
   std::string concat_inputs = "";
   for (size_t i= premier; i <= dernier; ++i)
      concat_inputs += nom_pngs + std::to_string(i) + ".png ";

    [[maybe_unused]] int result = std::system(("convert -delay " + std::to_string(delay) + " -loop " + std::to_string(loop) + " " + concat_inputs + nom_gif + ".gif").c_str());
}