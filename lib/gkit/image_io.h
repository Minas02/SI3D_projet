
#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H

#include "image.h"


//! \addtogroup image utilitaires pour manipuler des images
///@{

//! \file
//! manipulation directe d'images

//! charge une image a partir d'un fichier. renvoie Image::error() en cas d'echec. a detruire avec image::release( ).
//! \param filemane nom de l'image a charger
Image read_image( const char *filename, const bool flipY= true );
Image read_image_hdr( const char *filename, const bool flipY= true );

//! renvoie la taille en octets de l'image chargee. ou 0 si erreur.
unsigned read_image_size( const char *filename );

//! enregistre une image au format .png
bool write_image( const Image& image, const char *filename, const bool flipY= true );
//! enregistre une image au format .png
bool write_image_png( const Image& image, const char *filename, const bool flipY= true );
//! enregistre une image au format .bmp
bool write_image_bmp( const Image& image, const char *filename, const bool flipY= true );
//! enregistre une image au format .hdr
bool write_image_hdr( const Image& image, const char *filename, const bool flipY= true );

//! raccourci pour write_image_png(tone(image, range(image)), "image.png")
bool write_image_preview( const Image& image, const char *filename, const bool flipY= true );

//! reduit une image.
Image mipmap( const Image& image );

//! transformation couleur : rgb lineaire vers srgb
Image srgb( const Image& image );
//! transformation couleur : srgb vers rgb lineaire
Image linear( const Image& image );

//! retourne l'image
Image flipY( const Image& image );
//! retourne l'image
Image flipX( const Image& image );

//! renvoie un bloc de l'image
Image copy( const Image& image, const unsigned xmin, const unsigned ymin, const unsigned width, const unsigned height );

//! remplace un bout d'image par une autre. copie l'image pixels dans image [xmin, xmin+pixels.width] x [ymin, ymin+pixels.height].
Image& blit( Image& image, const unsigned xmin, const unsigned ymin, const Image& pixels );

//! stockage temporaire des donnees d'une image.
struct ImageData
{
    ImageData( ) : pixels(), width(0), height(0), channels(0), size(0) {}
    ImageData( const unsigned w, const unsigned h, const unsigned c, const unsigned s= 1 ) : pixels(w*h*c*s, 0), width(w), height(h), channels(c), size(s) {}
    
    unsigned offset( const unsigned x, const unsigned y, const unsigned c= 0 ) const { return (y * width +x) * channels * size + c * size; }
    const void *data( ) const { return pixels.data(); }
    void *data( ) { return pixels.data(); }
    
    std::vector<unsigned char> pixels;
    
    unsigned width;
    unsigned height;
    unsigned channels;
    unsigned size;
};


//! charge les donnees d'un fichier png. renvoie une image initialisee par defaut en cas d'echec.
ImageData read_image_data( const char *filename, const bool flipY= true );

//! enregistre des donnees dans un fichier png.
int write_image_data(const ImageData& image, const char *filename, const bool flipY= true );

//! charge les donnees d'un fichier png stocke en memoire. renvoie une image initialisee par defaut en cas d'echec.
ImageData read_image_data( const void *buffer, const unsigned size, const bool flipY= true );

//! transformation couleur : rgb lineaire vers srgb
ImageData srgb( const ImageData& image );

//! transformation couleur : srgb vers rgb lineaire
ImageData linear( const ImageData& image );

//! retourne l'image
ImageData flipY( const ImageData& image );
//! retourne l'image
ImageData flipX( const ImageData& image );

//! renvoie un bloc de l'image
ImageData copy( const ImageData& image, const unsigned xmin, const unsigned ymin, const unsigned width, const unsigned height );

//! remplace un bout d'image par une autre. copie l'image pixels dans image [xmin, xmin+pixels.width] x [ymin, ymin+pixels.height].
ImageData& blit( ImageData& image, const unsigned xmin, const unsigned ymin, const ImageData& pixels );

//! renvoie une image filtree plus petite.
ImageData mipmap( const ImageData& image );

//! renvoie le nombre de mipmap d'une image width x height.
int miplevels( const int width, const int height );

///@}

#endif
