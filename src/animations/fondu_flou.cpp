#include "animations.hpp"

double gaussian_function(int x, int y, double sigma) {
    return (1.0 / (2.0 * M_PI * sigma * sigma)) * exp(-(x * x + y * y) / (2.0 * sigma * sigma));
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

    int kernel_size = static_cast<int>(ceil(3 * intensite)) + 1;

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