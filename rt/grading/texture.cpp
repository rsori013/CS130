// #include "parse.h"
// #include "texture.h"
// #include "dump_png.h"
// #include <algorithm>
// #include <iostream>

// Texture::Texture(const Parse* parse, std::istream& in)
// {
//     std::string filename;
//     in >> name >> filename >> use_bilinear_interpolation;
    
//     Read_png(data, width, height, filename.c_str());

//     // Debugging code starts here
//     std::cout << "Reading file: " << filename << std::endl;
//     if(!data)
//     {
//         std::cerr << "Failed to load image data." << std::endl;
//     }
//     else
//     {
//         std::cout << "Image dimensions: " << width << "x" << height << std::endl;
//         std::cout << "First few pixel values after reading PNG:" << std::endl;
//         for(int k = 0; k < 5 && k < width * height; k++)
//         {
//             std::cout << From_Pixel(data[k]) << std::endl;
//         }
//     }
//     // Debugging code ends here
// }


// Texture::~Texture()
// {
//     delete [] data;
// }

// // uses nearest neighbor interpolation to determine color at texture coordinates
// // (uv[0],uv[1]).  To match the reference image, details on how this mapping is
// // done will matter.

// // 1. Assume that width=5 and height=4.  Texture coordinates with 0<=u<0.2 and
// //    0<=v<0.25 should map to pixel index i=0, j=0.  Texture coordinates with
// //    0.8<=u<1 and 0.75<=v<1 should map to pixel index i=4, j=3.

// // 2. Texture coordinates should "wrap around."  Some of the objects contain
// //    (u,v) coordinates that are less than 0 or greater than 1.  u=0.4 and u=1.4
// //    and u=-0.6 should be considered equivalent.  There is a wrap function in
// //    misc.h that you may use.

// // 3. Be very careful about indexing out of bounds.  For example, u=0.999999
// //    should result in i=width-1, not i=width.  The latter is out of bounds.

// // 4. Be careful with your rounding.  For example, u=-0.0001 should map to
// //    i=width-1 in accordance to the wrapping rule.  Remember that casting from
// //    float to integer rounds towards zero, so that (int)u would produce 0.  You
// //    may find the cmath functions floor, ceil, and rint to be helpful, as they
// //    provide precise control over rounding.

// // 5. Although there is a flag called use_bilinear_interpolation, none of the
// //    test cases exercise this feature.  You do not need to implement it, though
// //    of course you are welcome to do so if you like.  You may assume nearest
// //    neighbor interpolation.

// // vec3 Texture::Get_Color(const vec2& uv) const
// // {
// //     TODO;
// //     return {0,0,0};
// // }


// vec3 Texture::Get_Color(const vec2& uv) const {
//     if (!data) {
//         // If there's no PNG data, return a default color (white)
//         return vec3(1, 1, 1);
//     }

//     // Wrap UV coordinates to ensure they're between 0 and 1
//     float wrapped_u = wrap(uv[0], 1.0f);
//     float wrapped_v = wrap(uv[1], 1.0f);

//     // Convert wrapped UV coordinates to pixel indices
//     int i = static_cast<int>(wrapped_u * width);
//     int j = static_cast<int>((1.0f - wrapped_v) * height - 1);

//     // Ensure the indices are in the valid range
//     i = std::max(0, std::min(width - 1, i));
//     j = std::max(0, std::min(height - 1, j));

//     // Debugging output
//     // Comment out if not needed to avoid cluttering the console
//     // std::cout << "Pixel indices: i=" << i << ", j=" << j << std::endl;

//     // Retrieve the pixel data
//     Pixel pixel = data[j * width + i];

//     // Debugging output
//     // Comment out if not needed
//     // std::cout << "Raw pixel data: " << pixel << std::endl;

//     // Convert the pixel to vec3 color using the provided function
//     vec3 color = From_Pixel(pixel);

//     // Debugging output
//     // Comment out if not needed
//     // std::cout << "Converted color: (" << color[0] << ", " << color[1] << ", " << color[2] << ")" << std::endl;

//     return color;
// }

#include "parse.h"
#include "texture.h"
#include "dump_png.h"
#include <algorithm>
#include <iostream>

Texture::Texture(const Parse* parse, std::istream& in)
{
    std::string filename;
    in >> name >> filename >> use_bilinear_interpolation;
    
    Read_png(data, width, height, filename.c_str());

    // Debugging code starts here
    std::cout << "Reading file: " << filename << std::endl;
    if(!data)
    {
        std::cerr << "Failed to load image data." << std::endl;
    }
    else
    {
        std::cout << "Image dimensions: " << width << "x" << height << std::endl;
        std::cout << "First few pixel values after reading PNG:" << std::endl;
        for(int k = 0; k < 5 && k < width * height; k++)
        {
            std::cout << From_Pixel(data[k]) << std::endl;
        }
    }
    // Debugging code ends here
}

Texture::~Texture()
{
    delete [] data;
}

vec3 Texture::Get_Color(const vec2& uv) const {
    int u = static_cast<int>(uv[0] * (width - 1));
    int v = static_cast<int>(uv[1] * (height - 1));

    if (!use_bilinear_interpolation) {
        Pixel pixel = data[v * width + u];
        return vec3(
            ((pixel >> 24) & 0xFF) / 255.0,
            ((pixel >> 16) & 0xFF) / 255.0,
            ((pixel >> 8) & 0xFF) / 255.0
        );
    } else {
        // Bilinear Interpolation
        int u1 = std::clamp(u, 0, width - 1);
        int u2 = std::clamp(u + 1, 0, width - 1);
        int v1 = std::clamp(v, 0, height - 1);
        int v2 = std::clamp(v + 1, 0, height - 1);

        Pixel p1 = data[v1 * width + u1];
        Pixel p2 = data[v1 * width + u2];
        Pixel p3 = data[v2 * width + u1];
        Pixel p4 = data[v2 * width + u2];

        double fx = uv[0] * width - u;
        double fy = uv[1] * height - v;

        vec3 col1 =Texture::mix (
            vec3(((p1 >> 24) & 0xFF) / 255.0, ((p1 >> 16) & 0xFF) / 255.0, ((p1 >> 8) & 0xFF) / 255.0),
            vec3(((p2 >> 24) & 0xFF) / 255.0, ((p2 >> 16) & 0xFF) / 255.0, ((p2 >> 8) & 0xFF) / 255.0),
            fx
        );

        vec3 col2 = Texture::mix(
            vec3(((p3 >> 24) & 0xFF) / 255.0, ((p3 >> 16) & 0xFF) / 255.0, ((p3 >> 8) & 0xFF) / 255.0),
            vec3(((p4 >> 24) & 0xFF) / 255.0, ((p4 >> 16) & 0xFF) / 255.0, ((p4 >> 8) & 0xFF) / 255.0),
            fx
        );

        return mix(col1, col2, fy);
    }
}

// A simple implementation of the mix function
vec3 Texture::mix(const vec3& a, const vec3& b, double factor) {
    return a + factor * (b - a);
}
