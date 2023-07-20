




#include "parse.h"
#include "texture.h"
#include "dump_png.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>  // For rand()
#include <cmath>  // For floor and ceil functions

Texture::Texture(const Parse* parse, std::istream& in)
{
    std::string filename;
    in >> name >> filename >> use_bilinear_interpolation;
    
    Read_png(data, width, height, filename.c_str());

    // Debugging code
    std::cout << "Reading file: " << filename << std::endl;
    if(!data)
    {
        std::cerr << "Failed to load image data." << std::endl;
    }
    else
    {
        std::cout << "Image dimensions: " << width << "x" << height << std::endl;
        std::cout << "First few pixel values after reading PNG:" << std::endl;
        // for(int k = 0; k < 1500 && k < width * height; k++)
        // {
        //     std::cout << From_Pixel(data[k]) << std::endl;
        // }
        std::cout << From_Pixel(data[250000]);
    }
}



Texture::~Texture()
{
    delete [] data;
}

vec3 Texture::Get_Color(const vec2& uv) const {
    // 1. Check UV bounds.
    if (uv[0] < 0 || uv[0] > 1 || uv[1] < 0 || uv[1] > 1) {
        Pixel_Print("Warning: Out of bounds UV coordinates: ", uv[0], ", ", uv[1]);
        return vec3(1, 0, 0);  // Return bright red for any out-of-bounds UVs for easy spotting.
    }

    // 2. Convert UV to texture coordinates.
    int i = uv[0] * width;
    int j = uv[1] * height;

    // 3. Wrap coordinates if necessary.
    int wrappedI = wrap(i, width);
    int wrappedJ = wrap(j, height);
    if (i != wrappedI || j != wrappedJ) {
        Pixel_Print("Info: Wrapped UV coordinates from ", i, ", ", j, " to ", wrappedI, ", ", wrappedJ);
    }

    // 4. Fetch the pixel.
    Pixel pixelValue = data[wrappedJ * width + wrappedI];
    vec3 colorFromPixel = From_Pixel(pixelValue);
    
    // 5. Debug print.
    Pixel_Print("UV: ", uv[0], ", ", uv[1], " Coords: ", wrappedI, ", ", wrappedJ, " Color: ", colorFromPixel[0], ", ", colorFromPixel[1], ", ", colorFromPixel[2]);

    return colorFromPixel;
}