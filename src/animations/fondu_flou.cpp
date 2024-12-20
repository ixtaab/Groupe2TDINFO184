#include "animations.hpp"

/**
* Calcule la valeur de la fonction gaussienne 2D en un point
* Utilisée pour créer un noyau de convolution gaussien pour le flou
* @param x Coordonnée x du point
* @param y Coordonnée y du point
* @param sigma Écart-type de la gaussienne (contrôle l'étalement)
* @return Valeur de la gaussienne au point (x,y)
*/
double fonction_gaussienne(int x, int y, double sigma) {
	return (1.0 / (2.0 * M_PI * sigma * sigma)) * exp(-(x * x + y * y) / (2.0 * sigma * sigma));
}

/**
* Applique un flou gaussien à une image
* Utilise une technique de séparation en x et y pour optimiser les calculs
* @param image_source Image d'entrée
* @param image_destination Image de sortie
* @param image_tampon Image temporaire pour le calcul intermédiaire
* @param intensity Intensité du flou (sigma de la gaussienne)
* 
* La fonction applique le flou en deux passes :
*
* 1. Flou vertical stocké dans image_tampon
*
* 2. Flou horizontal stocké dans image_destination
*
* @note invariances: 
*
* - image_source, image_tampon et image_destination doivent avoir les mêmes dimensions. 
*/
void flouter_image(
	const Image_PNG& image_source,
	Image_PNG& image_destination,
	Image_PNG& image_tampon,
	double intensity
) {
	assert(image_destination.largeur == image_tampon.largeur);
	assert(image_destination.hauteur == image_tampon.hauteur);
	
	const size_t largeur = image_source.largeur;
	const size_t hauteur = image_source.hauteur;
	
	// Calcul du rayon du noyau gaussien (3 sigma couvre 99.7% de la gaussienne)
	const int rayon_kernel = static_cast<int>(ceil(3 * intensity));
	
	// Précalcul des coefficients gaussiens et normalisation
	double somme_facteurs = 0.0;
	std::vector<double> facteurs_gaussien(rayon_kernel + 1);
	for(int i = 0; i <= rayon_kernel; i++) {
		facteurs_gaussien[i] = fonction_gaussienne(i, 0, intensity);
		somme_facteurs += (i == 0) ? facteurs_gaussien[i] : 2 * facteurs_gaussien[i];
	}
	const double facteur_normaliser = 1.0 / somme_facteurs;

	// Fonction pour gérer les bords de l'image par réflexion
	auto refleter_coordonnee = [](int coord, int size) {
		if (coord < 0) return -coord;
		if (coord >= size) return 2 * size - coord - 2;
		return coord;
	};

	// Première passe : flou vertical
	#pragma omp parallel for
	for(size_t y = 0; y < hauteur; y++) {
		for(size_t x = 0; x < largeur; x++) {
			double r = 0.0, g = 0.0, b = 0.0;
			for(int dy = -rayon_kernel; dy <= rayon_kernel; ++dy) {
				const int ny = refleter_coordonnee(static_cast<int>(y) + dy, hauteur);
				const double facteur = facteurs_gaussien[abs(dy)] * facteur_normaliser;
				const RVB& pixel = image_source.pixels[ny % hauteur][x];
				r += pixel.rouge * facteur;
				g += pixel.vert * facteur;
				b += pixel.bleu * facteur;
			}
			image_tampon.pixels[y][x] = {
				static_cast<Composante>(r),
				static_cast<Composante>(g),
				static_cast<Composante>(b)
			};
		}
	}

	// Deuxième passe : flou horizontal
	#pragma omp parallel for
	for(size_t y = 0; y < hauteur; y++) {
		for(size_t x = 0; x < largeur; x++) {
			double r = 0.0, g = 0.0, b = 0.0;
			for(int dx = -rayon_kernel; dx <= rayon_kernel; ++dx) {
				const int nx = refleter_coordonnee(static_cast<int>(x) + dx, largeur);
				const double facteur = facteurs_gaussien[abs(dx)] * facteur_normaliser;
				const RVB& pixel = image_tampon.pixels[y][nx % largeur];
				r += pixel.rouge * facteur;
				g += pixel.vert * facteur;
				b += pixel.bleu * facteur;
			}
			image_destination.pixels[y][x] = {
				static_cast<Composante>(r),
				static_cast<Composante>(g),
				static_cast<Composante>(b)
			};
		}
	}
}

/**
* Crée une animation montrant une image progressivement floutée
* @param chemin_image Chemin de l'image source
* @param chemin_destination Dossier de destination
* @param nb_etapes Nombre d'images dans l'animation
* @param intensite_max Intensité maximale du flou
* 
* Les images sont sauvegardées dans chemin_destination/images/
* Le GIF final est sauvegardé dans chemin_destination/gif/
*/
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

	// Crée les images avec une intensité de flou croissante
	for(size_t i = 0; i < nb_etapes; i++) {
		double intensite = (intensite_max / nb_etapes) * (i + 1);
		flouter_image(image_source, image_tampon, image_destination, intensite);
		sauver_PNG(
			chemin_destination + "/images/" + nom_image + "_" +
			to_string(i) + ".png", image_tampon
		);
	}

	// Génère le GIF final
	generer_GIF(
		chemin_destination + "/images/" + nom_image + "_",
		chemin_destination + "/gif/" + nom_image,
		0, nb_etapes - 1, 15, 0
	);
}