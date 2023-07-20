




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


// vec3 Texture::Get_Color(const vec2& uv) const
// {
//     // Check if UVs are out of bounds
//     if(uv[0] < 0.0 || uv[0] > 1.0 || uv[1] < 0.0 || uv[1] > 1.0)
//     {
//         std::cerr << "Warning: Out-of-bounds UV: " << uv[0] << ", " << uv[1] << std::endl;
//         return {1,0,1};  // return magenta for any out-of-bounds UVs
//     }

//     // Normalize UVs to ensure they are within [0, 1] range. 
//     vec2 normalized_uv;
//     normalized_uv[0] = uv[0] - std::floor(uv[0]);
//     normalized_uv[1] = uv[1] - std::floor(uv[1]);

//     // Convert the UVs into pixel-space
//     float u_pixelSpace = normalized_uv[0] * width;
//     float v_pixelSpace = normalized_uv[1] * height;

//     // Using floor function for rounding to get the nearest pixel value
//     int u_index = wrap(static_cast<int>(std::floor(u_pixelSpace)), width);
//     int v_index = wrap(static_cast<int>(std::floor(v_pixelSpace)), height);

//     int pixel_index = v_index * width + u_index;

//     if (pixel_index >= 0 && pixel_index < width * height)
//     {
//         vec3 color = From_Pixel(data[pixel_index]);
//         return color;
//     }

//     std::cerr << "Error: Invalid pixel index " << pixel_index << std::endl;
//     return {0,0,0};  // return black for any invalid pixel indices
// }
vec3 Texture::Get_Color(const vec2& uv) const
{
    // For debugging: 
    if(uv[0] < 0.0 || uv[0] > 1.0 || uv[1] < 0.0 || uv[1] > 1.0)
    {
        return {1.0, 0.0, 0.0};  // Return red if UVs are out of bounds
    }

    // Return UVs as colors otherwise
    return {uv[0], uv[1], 0.0};
}


