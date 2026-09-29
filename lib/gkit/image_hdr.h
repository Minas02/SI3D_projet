
#ifndef _IMAGE_HDR_H
#define _IMAGE_HDR_H

#include "image.h"


//! \addtogroup image utilitaires pour manipuler des images
//@{

//! \file
//! manipulation directe d'images, format .hdr et .pfm

//! renvoie vrai si le nom de fichier se termine par .hdr.
bool is_hdr_image( const char *filename );

//! charge une image a partir d'un fichier .pfm.
Image read_image_pfm( const char *filename );

//! enregistre une image dans un fichier .pfm.
int write_image_pfm( const Image& image, const char *filename );

//! renvoie vrai si le nom de fichier se termine par .pfm.
bool is_pfm_image( const char *filename );
//@}

#endif
