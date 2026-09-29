
#include <chrono>
#include <cstring>
#include <string>

#include "rgbe.h"
#include "image_hdr.h"


bool is_hdr_image( const char *filename )
{
    return (std::string(filename).rfind(".hdr") != std::string::npos);
}

Image read_image_pfm( const char *filename )
{
    FILE *in= fopen(filename, "rb");
    if(in == nullptr)
    {
        printf("[error] loading pfm image '%s'...\n", filename);
        return Image();
    }
    
    int w, h;
    float endian= 0;
    if(fscanf(in, "PF\xa%d %d\xa%f[^\xa]", &w, &h, &endian) != 3 
    || endian != -1)
    {
        printf("[error] loading pfm image '%s'...\n", filename);
        return Image();
    }
    
    // saute la fin de l'entete
    unsigned char c= fgetc(in);
    while(c != '\xa')
        c= fgetc(in);
    
    // pourquoi aussi tordu ? fscanf(in, "PF\n%d %d\n%f\n") consomme les espaces apres le \n... ce qui est un poil genant pour relire les floats...
    
    printf("loading pfm image '%s' %dx%d...\n", filename, w, h);
    
    Image image(w, h);
    
    for(int y= 0; y < h; y++)
    for(int x= 0; x < w; x++)
    {
        Color pixel;
        if(fread(&pixel.r, sizeof(float), 3, in) == 3)
            image(x, y)= pixel;
    }
    fclose(in);
    
    return image;
}


//! enregistre une image dans un fichier .pfm.
int write_image_pfm( const Image& image, const char *filename )
{
    FILE *out= fopen(filename, "wb");
    if(out == nullptr)
    {
        printf("[error] writing pfm image '%s'...\n", filename);
        return -1;
    }
    
    fprintf(out, "PF\xa%d %d\xa-1\xa", image.width(), image.height());
    
    for(unsigned y= 0; y < image.height(); y++)
    for(unsigned x= 0; x < image.width(); x++)
    {
        Color pixel= image(x, y);
        fwrite(&pixel.r, sizeof(float), 3, out);
    }
    fclose(out);
    
    printf("writing pfm image '%s'...\n", filename);
    return 0;
}


//! renvoie vrai si le nom de fichier se termine par .pfm.
bool is_pfm_image( const char *filename )
{
    return (std::string(filename).rfind(".pfm") != std::string::npos);
}
