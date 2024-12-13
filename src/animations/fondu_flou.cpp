#include "animations.hpp"

double fonction_gaussienne(int x, int y, double sigma) {
    return (1.0 / (2.0 * M_PI * sigma * sigma)) * exp(-(x * x + y * y) / (2.0 * sigma * sigma));
}

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

    const int rayon_kernel = static_cast<int>(ceil(3 * intensity)) / 2;

    double somme_facteurs = 0.0;
    std::vector<double> facteurs_gaussien(rayon_kernel + 1);
    for(int i = 0; i <= rayon_kernel; i++) {
        facteurs_gaussien[i] = fonction_gaussienne(i, 0, intensity);
        somme_facteurs += (i == 0) ? facteurs_gaussien[i] : 2 * facteurs_gaussien[i];
    }
    const double facteur_normaliser = 1.0 / somme_facteurs;

    auto refleter_coordonnee = [](int coord, int size) {
        if (coord < 0) return -coord;
        if (coord >= size) return 2 * size - coord - 2;
        return coord;
    };

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