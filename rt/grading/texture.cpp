// #include "parse.h"
// #include "texture.h"
// #include "dump_png.h"
// #include <algorithm>
// #include <iostream>
// #include <cstdlib>  // For rand()
// #include <cmath>  // For floor and ceil functions

// Texture::Texture(const Parse* parse, std::istream& in)
// {
//     std::string filename;
//     in >> name >> filename >> use_bilinear_interpolation;
    
//     Read_png(data, width, height, filename.c_str());

//     // Debugging code
//     std::cout << "Reading file: " << filename << std::endl;
//     if(!data)
//     {
//         std::cerr << "Failed to load image data." << std::endl;
//     }
//     else
//     {
//         std::cout << "Image dimensions: " << width << "x" << height << std::endl;
//         std::cout << "First few pixel values after reading PNG:" << std::endl;
//         // for(int k = 0; k < 1500 && k < width * height; k++)
//         // {
//         //     std::cout << From_Pixel(data[k]) << std::endl;
//         // }
//         std::cout << From_Pixel(data[250000]);
//     }
// }



// Texture::~Texture()
// {
//     delete [] data;
// }

// vec3 Texture::Get_Color(const vec2& uv) const {
//     // 1. Check UV bounds.
//     if (uv[0] < 0 || uv[0] > 1 || uv[1] < 0 || uv[1] > 1) {
//         Pixel_Print("Warning: Out of bounds UV coordinates: ", uv[0], ", ", uv[1]);
//         return vec3(1, 0, 0);  // Return bright red for any out-of-bounds UVs for easy spotting.
//     }

//     // 2. Convert UV to texture coordinates.
//     int i = uv[0] * width;
//     int j = uv[1] * height;

//     // 3. Wrap coordinates if necessary.
//     int wrappedI = wrap(i, width);
//     int wrappedJ = wrap(j, height);
//     if (i != wrappedI || j != wrappedJ) {
//         Pixel_Print("Info: Wrapped UV coordinates from ", i, ", ", j, " to ", wrappedI, ", ", wrappedJ);
//     }

//     // 4. Fetch the pixel.
//     Pixel pixelValue = data[wrappedJ * width + wrappedI];
//     vec3 colorFromPixel = From_Pixel(pixelValue);
    
//     // 5. Debug print.
//     Pixel_Print("UV: ", uv[0], ", ", uv[1], " Coords: ", wrappedI, ", ", wrappedJ, " Color: ", colorFromPixel[0], ", ", colorFromPixel[1], ", ", colorFromPixel[2]);

//     return colorFromPixel;
// }

#include "parse.h"
#include "texture.h"
#include "dump_png.h"

Texture::Texture(const Parse* parse,std::istream& in)
{
    std::string filename;
    in>>name>>filename>>use_bilinear_interpolation;
    Read_png(data,width,height,filename.c_str());
}

Texture::~Texture()
{
    delete [] data;
}

// uses nearest neighbor interpolation to determine color at texture coordinates
// (uv[0],uv[1]).  To match the reference image, details on how this mapping is
// done will matter.

// 1. Assume that width=5 and height=4.  Texture coordinates with 0<=u<0.2 and
//    0<=v<0.25 should map to pixel index i=0, j=0.  Texture coordinates with
//    0.8<=u<1 and 0.75<=v<1 should map to pixel index i=4, j=3.

// 2. Texture coordinates should "wrap around."  Some of the objects contain
//    (u,v) coordinates that are less than 0 or greater than 1.  u=0.4 and u=1.4
//    and u=-0.6 should be considered equivalent.  There is a wrap function in
//    misc.h that you may use.

// 3. Be very careful about indexing out of bounds.  For example, u=0.999999
//    should result in i=width-1, not i=width.  The latter is out of bounds.

// 4. Be careful with your rounding.  For example, u=-0.0001 should map to
//    i=width-1 in accordance to the wrapping rule.  Remember that casting from
//    float to integer rounds towards zero, so that (int)u would produce 0.  You
//    may find the cmath functions floor, ceil, and rint to be helpful, as they
//    provide precise control over rounding.

// 5. Although there is a flag called use_bilinear_interpolation, none of the
//    test cases exercise this feature.  You do not need to implement it, though
//    of course you are welcome to do so if you like.  You may assume nearest
//    neighbor interpolation.

vec3 Texture::Get_Color(const vec2& uv) const
{
    // Calculate the pixel indices (i, j) from texture coordinates (uv)
    int i = (int)(std::floor(wrap(uv[0], 1.0f) * width));
    int j = (int)(std::floor(wrap(uv[1], 1.0f) * height));

    // Ensure the pixel indices are within bounds
    i = std::max(0, std::min(i, width - 1));
    j = std::max(0, std::min(j, height - 1));

    // Calculate the color at the pixel indices
    int index = j * width + i;
    Pixel pixelColor = data[index];
    vec3 color = From_Pixel(pixelColor);

    return color;
}

