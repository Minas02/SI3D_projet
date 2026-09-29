
#include <cfloat>
#include <string>

#include "color.h"
#include "image.h"
#include "image_io.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Image srgb( const Image& image )
{
    Image tmp(image.width(), image.height());
    
    for(unsigned i= 0; i < image.size(); i++)
        tmp(i)= srgb(image(i));
    
    return tmp;
}

Image linear( const Image& image )
{
    Image tmp(image.width(), image.height());
    
    for(unsigned i= 0; i < image.size(); i++)
        tmp(i)= linear(image(i));
    
    return tmp;
}

float range( const Image& image )
{
    float gmin= FLT_MAX;
    float gmax= 0;
    for(unsigned i= 0; i < image.size(); i++)
    {
        Color color= image(i);
        float g= color.r + color.g + color.b;
        
        if(g < gmin) gmin= g;
        if(g > gmax) gmax= g;
    }
    
    //! \todo utiliser une distribution en log, plutot que lineaire...
    int bins[100] = {};
    for(unsigned i= 0; i < image.size(); i++)
   {
        Color color= image(i);
        float g= color.r + color.g + color.b;
        
        int b= (g - gmin) * 100 / (gmax - gmin);
        if(b >= 99) b= 99;
        if(b < 0) b= 0;
        bins[b]++;
    }
    
    float saturation= 0;
    float qbins= 0;
    for(unsigned i= 0; i < 100; i++)
    {
        qbins= qbins + float(bins[i]) / float(image.size());
        if(qbins > .75f)
            return gmin + float(i+1) / 100 * (gmax - gmin);
    }
    
    return gmax;
}

Image tone( const Image& image, const float saturation )
{
    Image tmp(image.width(), image.height());
    
    float k= 1 / std::pow(saturation, 1 / 2.2f);
    for(unsigned i= 0; i < image.size(); i++)
    {
        Color color= image(i);
        if(std::isnan(color.r) || std::isnan(color.g) || std::isnan(color.b))
            // marque les pixels pourris avec une couleur improbable...            
            color= Color(1, 0, 1);
        else
            // sinon transformation rgb -> srgb
            color= k * srgb(color);
        
        tmp(i)= Color(color, 1);
    }
    
    return tmp;
}


Image mipmap( const Image& image )
{
    unsigned w= std::max(unsigned(1), image.width() / 2);
    unsigned h= std::max(unsigned(1), image.height() / 2);
    
    Image tmp(w, h);
    
    for(unsigned py= 0; py < h; py++)
    for(unsigned px= 0; px < w; px++)
    {
        unsigned x= 2*px;
        unsigned y= 2*py;
        tmp(px, py)= (image(x, y) + image(x +1, y) + image(x +1, y +1) + image(x, y +1)) / 4;   
    }
    
    return tmp;
}

Image flipY( const Image& image )
{
    // flip de l'image : origine en haut a gauche
    Image flip(image.width(), image.height());

    for(unsigned y= 0; y < image.height(); y++)
    for(unsigned x= 0; x < image.width(); x++)
    {
        unsigned s= image.offset(x, y);
        unsigned d= flip.offset(x, flip.height() - y -1);
        
        flip(d)= image(s);
    }

    return flip;
}

Image flipX( const Image& image )
{
    Image flip(image.width(), image.height());

    for(unsigned y= 0; y < image.height(); y++)
    for(unsigned x= 0; x < image.width(); x++)
    {
        unsigned s= image.offset(x, y);
        unsigned d= flip.offset(flip.width() -x -1, y);
        
        flip(d)= image(s);
    }

    return flip;
}

Image copy( const Image& image, const unsigned xmin, const unsigned ymin, const unsigned width, const unsigned height )
{
    Image copy(width, height);
    
    for(unsigned y= 0; y < height; y++)
    for(unsigned x= 0; x < width; x++)
    {
        unsigned s= image.offset(xmin+x, ymin+y);
        unsigned d= copy.offset(x, y);
        
        copy(d)= image(s);
    }
    
    return copy;
}

Image& blit( Image& image, const unsigned xmin, const unsigned ymin, const Image& pixels )
{
    if(image.size() == 0)
        return image;
    if(pixels.size() == 0)
        return image;
    
    unsigned xmax= std::min( image.width(), xmin + pixels.width() );
    unsigned ymax= std::min( image.height(), ymin + pixels.height() );
    unsigned w= xmax - xmin;
    unsigned h= ymax - ymin;
    
    for(unsigned y= 0; y < h; y++)
    for(unsigned x= 0; x < w; x++)
        image(x + xmin, y + ymin)= pixels(x, y);
    
    return image;
}


unsigned read_image_size( const char *filename )
{
    int w, h, c;
    if(!stbi_info(filename, &w, &h, &c))
        return 0;
    
    return sizeof(Color)*w*h;
}

Image read_image( const char *filename, const bool flipY )
{
    stbi_ldr_to_hdr_scale(1);
    stbi_ldr_to_hdr_gamma(1);
    stbi_set_flip_vertically_on_load(flipY);
    
    int width, height, channels;
    float *data= stbi_loadf(filename, &width, &height, &channels, 4);
    if(!data)
    {
        printf("[error] loading '%s'...\n", filename);
        return {};
    }
    
    Image image(width, height);
    for(unsigned i= 0, offset= 0; i < image.size(); i++, offset+= 4)
        image(i)= Color( data[offset], data[offset + 1], data[offset + 2], data[offset + 3]);
    
    stbi_image_free(data);
    return image;
}

Image read_image_hdr( const char *filename, const bool flipY )
{ 
    return read_image(filename, flipY ); 
}


inline float clamp( const float x, const float min, const float max )
{
    if(x < min) return min;
    else if(x > max) return max;
    else return x;
}

bool write_image_png( const Image& image, const char *filename, const bool flipY )
{
    if(image.size() == 0)
        return false;
    
    std::vector<unsigned char> tmp(image.width()*image.height()*4);
    for(unsigned i= 0, offset= 0; i < image.size(); i++, offset+= 4)
    {
        Color pixel= image(i) * 255;
        tmp[offset   ]= clamp(pixel.r, 0, 255);
        tmp[offset +1]= clamp(pixel.g, 0, 255);
        tmp[offset +2]= clamp(pixel.b, 0, 255);
        tmp[offset +3]= clamp(pixel.a, 0, 255);
    }
    
    stbi_flip_vertically_on_write(flipY);
    return stbi_write_png(filename, image.width(), image.height(), 4, tmp.data(), image.width() * 4) != 0;
}

bool write_image( const Image& image, const char *filename, const bool flipY )
{
    return write_image_png(image, filename, flipY );
}

bool write_image_bmp( const Image& image, const char *filename, const bool flipY )
{
    if(image.size() == 0)
        return false;
    
    std::vector<unsigned char> tmp(image.width()*image.height()*4);
    for(unsigned i= 0, offset= 0; i < image.size(); i++, offset+= 4)
    {
        Color pixel= image(i) * 255;
        tmp[offset   ]= clamp(pixel.r, 0, 255);
        tmp[offset +1]= clamp(pixel.g, 0, 255);
        tmp[offset +2]= clamp(pixel.b, 0, 255);
        tmp[offset +3]= clamp(pixel.a, 0, 255);
    }
    
    stbi_flip_vertically_on_write(flipY);
    return stbi_write_bmp(filename, image.width(), image.height(), 4, tmp.data()) != 0;
}

bool write_image_hdr( const Image& image, const char *filename, const bool flipY )
{
    if(image.size() == 0)
        return false;
    
    stbi_flip_vertically_on_write(flipY);
    return stbi_write_hdr(filename, image.width(), image.height(), 4, image.data()) != 0;
}

bool write_image_preview( const Image& image, const char *filename, const bool flipY )
{
    if(image.size() == 0)
        return false;
    
    Image tmp= tone(image, range(image));
    return write_image_png(tmp, filename, flipY);
}


ImageData image_data( unsigned char *data,const int width, const int height, const int channels )
{
    int n= std::max(3, channels); 
    ImageData image(width, height, n);
    
    if(channels == 4)
    {
        for(int i= 0; i < width*height*4; i+= 4)
        {
            image.pixels[i]= data[i];
            image.pixels[i+1]= data[i+1];
            image.pixels[i+2]= data[i+2];
            image.pixels[i+3]= data[i+3];
        }
    }
    else if(channels == 3)
    {
        for(int i= 0; i < width*height*3; i+= 3)
        {
            image.pixels[i]= data[i];
            image.pixels[i+1]= data[i+1];
            image.pixels[i+2]= data[i+2];
        }
    }
    else
    {
        int k= 0;
        for(int i= 0; i < width*height*3; i+= 3)
        {
            unsigned char r= 0;
            unsigned char g= 0;
            unsigned char b= 0;
            
            if(n >= 1) { r= data[k++]; g= r; b= r; }      // rgb= rrr
            if(n >= 2) { g= data[k++]; b= 0; }              // rgb= rg0
            if(n >= 3) { b= data[k++]; }                        // rgb
            
            image.pixels[i]= r;
            image.pixels[i+1]= g;
            image.pixels[i+2]= b;
        }
    }
    
    stbi_image_free(data);
    return image;
}

ImageData read_image_data( const void *buffer, const unsigned size, const bool flipY )
{
    stbi_set_flip_vertically_on_load(flipY);
    
    int width, height, channels;
    unsigned char *data= stbi_load_from_memory((const unsigned char *) buffer, size, &width, &height, &channels, 0);
    if(!data)
    {
        printf("[error] reading buffer image...\n");
        return {};
    }
    
    return image_data(data, width, height, channels);
}

ImageData read_image_data( const char *filename, const bool flipY )
{
    stbi_set_flip_vertically_on_load(flipY);
    
    int width, height, channels;
    unsigned char *data= stbi_load(filename, &width, &height, &channels, 0);
    if(!data)
    {
        printf("[error] loading '%s'...\n", filename);
        return {};
    }
    
    return image_data(data, width, height, channels);
}


int write_image_data( const ImageData& image, const char *filename, const bool flipY )
{
    if(image.size != 1)
    {
        printf("[error] writing color image '%s'... not an 8 bits image.\n", filename);
        return -1;
    }

    stbi_flip_vertically_on_write(flipY);
    
    int code= 0;
    if(std::string(filename).rfind(".png") != std::string::npos)
        code= stbi_write_png(filename, image.width, image.height, image.channels, image.pixels.data(), image.width * image.channels) != 0;
    else if(std::string(filename).rfind(".bmp") != std::string::npos)
        code= stbi_write_bmp(filename, image.width, image.height, image.channels, image.pixels.data()) != 0;
    
    else 
    {
        printf("[error] writing color image '%s'... not a .png / .bmp image.\n", filename);
        return -1;
    }
    
    if(code > 0)
        return 0;
    
    printf("[error] writing color image '%s'...\n", filename);
    return -1;
}



ImageData srgb( const ImageData& image )
{
    ImageData tmp(image.width, image.height, image.channels);
    
    for(unsigned i= 0; i < image.pixels.size(); i+= image.channels)
    {
        Color pixel= srgb( Color( image.pixels[i], image.pixels[i+1], image.pixels[i+2], (image.channels > 3) ? image.pixels[i+3] : 255 ) / 255 );
        
        tmp.pixels[i]= clamp(pixel.r * 255, 0, 255); 
        tmp.pixels[i +1]= clamp(pixel.g * 255, 0, 255); 
        tmp.pixels[i +2]= clamp(pixel.b * 255, 0, 255); 
        if(image.channels > 3) 
            tmp.pixels[i +3]= clamp(pixel.a * 255, 0, 255); 
    }
    
    return tmp;
}

ImageData linear( const ImageData& image )
{
    ImageData tmp(image.width, image.height, image.channels);
    
    for(unsigned i= 0; i < image.pixels.size(); i+= image.channels)
    {
        Color pixel= linear( Color( image.pixels[i], image.pixels[i+1], image.pixels[i+2], (image.channels > 3) ? image.pixels[i+3] : 255 ) / 255 );
        
        tmp.pixels[i]= clamp(pixel.r * 255, 0, 255); 
        tmp.pixels[i +1]= clamp(pixel.g * 255, 0, 255); 
        tmp.pixels[i +2]= clamp(pixel.b * 255, 0, 255); 
        if(image.channels > 3) 
            tmp.pixels[i +3]= clamp(pixel.a * 255, 0, 255); 
    }
    
    return tmp;
}

ImageData flipY( const ImageData& image )
{
    // flip de l'image : origine en haut a gauche
    ImageData flip(image.width, image.height, image.channels);

    for(unsigned y= 0; y < image.height; y++)
    for(unsigned x= 0; x < image.width; x++)
    {
        unsigned s= image.offset(x, y);
        unsigned d= flip.offset(x, flip.height - y -1);
        
        for(unsigned i= 0; i < image.channels; i++)
            flip.pixels[d +i]= image.pixels[s +i];
    }

    return flip;
}

ImageData flipX( const ImageData& image )
{
    ImageData flip(image.width, image.height, image.channels);

    for(unsigned y= 0; y < image.height; y++)
    for(unsigned x= 0; x < image.width; x++)
    {
        unsigned s= image.offset(x, y);
        unsigned d= flip.offset(flip.width -x -1, y);
        
        for(unsigned i= 0; i < image.channels; i++)
            flip.pixels[d +i]= image.pixels[s +i];
    }

    return flip;
}

ImageData copy( const ImageData& image, const unsigned xmin, const unsigned ymin, const unsigned width, const unsigned height )
{
    ImageData copy(width, height, image.channels);
    
    for(unsigned y= 0; y < height; y++)
    for(unsigned x= 0; x < width; x++)
    {
        unsigned s= image.offset(xmin +x, ymin +y);
        unsigned d= copy.offset(x, y);
        
        for(unsigned i= 0; i < image.channels; i++)
            copy.pixels[d +i]= image.pixels[s +i];
    }
    
    return copy;
}


ImageData& blit( ImageData& image, const unsigned xmin, const unsigned ymin, const ImageData& pixels )
{
    if(image.pixels.size() == 0)
        return image;
    if(pixels.pixels.size() == 0)
        return image;
    if(image.channels != pixels.channels)
        return image;
    
    unsigned xmax= std::min( image.width, xmin + pixels.width );
    unsigned ymax= std::min( image.height, ymin + pixels.height );
    unsigned w= xmax - xmin;
    unsigned h= ymax - ymin;
    
    for(unsigned y= 0; y < h; y++)
    for(unsigned x= 0; x < w; x++)
    {
        unsigned char *dst= image.pixels.data() + image.offset(x + xmin, y + ymin);
        const unsigned char *src= pixels.pixels.data() + pixels.offset(x, y);
        for(unsigned i= 0; i < pixels.channels; i++)
            dst[i]= src[i];
    }
    
    return image;
}

ImageData mipmap( const ImageData& image )
{
    unsigned w= std::max(unsigned(1), image.width / 2);
    unsigned h= std::max(unsigned(1), image.height / 2);
    
    ImageData tmp(w, h, image.channels);
    
    for(unsigned py= 0; py < h; py++)
    for(unsigned px= 0; px < w; px++)
    {
        unsigned x= 2*px;
        unsigned y= 2*py;
        unsigned d= tmp.offset(px, py);
        
        for(unsigned i= 0; i < image.channels; i++)
            tmp.pixels[d +i]= (
                    image.pixels[image.offset(x, y) +i] 
                + image.pixels[image.offset(x+1, y) +i] 
                + image.pixels[image.offset(x, y+1) +i] 
                + image.pixels[image.offset(x+1, y+1) +i] ) / 4;
    }
    
    return tmp;
}

int miplevels( const int width, const int height )
{
    int w= width;
    int h= height;
    int levels= 1;
    while(w > 1 || h > 1)
    {
        w= std::max(1, w / 2);
        h= std::max(1, h / 2);
        levels= levels + 1;
    }
    
    return levels;
}
