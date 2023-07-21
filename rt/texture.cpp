#include "parse.h"
#include "texture.h"
#include "dump_png.h"
#include <algorithm>

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
        // for(int k = 583739; k <583900 ; k++)
        // {
        //     std::cout << From_Pixel(data[k]) << std::endl;
        // }
    }
    // Debugging code ends here
}

Texture::~Texture()
{
    delete [] data;
}

// vec3 Texture::Get_Color(const vec2& uv) const
// {
//     // If there's no data, return a default color
//     if(!data)
//     {
//         std::cerr << "Error: No texture data loaded." << std::endl;
//         return vec3(1.0, 0.0, 0.0); // Red, for visibility in case of issues.
//     }

//     // Handle wrapping for u and v
//     double u = uv[0] - floor(uv[0]);
//     double v = uv[1] - floor(uv[1]);

//     // For wrapping, if u or v is slightly negative, they should wrap around to the end
//     if(u < 0) u += 1.0;
//     if(v < 0) v += 1.0;

//     // Convert the uv coordinates to pixel coordinates
//     double x = u * static_cast<double>(width);
//     double y = v * static_cast<double>(height);

//     // Fetch the nearest pixel without bilinear interpolation
//     int xi = static_cast<int>(round(x)) % width;
//     int yi = static_cast<int>(round(y)) % height;

//     vec3 color = From_Pixel(get_pixel(xi, yi));

//     return color;
// }
vec3 Texture::Get_Color(const vec2& uv) const
{
    // If there's no data, return a default color
    if(!data)
    {
        std::cerr << "Error: No texture data loaded." << std::endl;
        return vec3(1.0, 0.0, 0.0); // Red for visibility
    }

    // Convert UV to pixel coordinates, ensuring we don't exceed image bounds.
    int x = static_cast<int>(uv[0] * width) % width;
    int y = static_cast<int>(uv[1] * height) % height;

    // Fetch the pixel color from the texture
    vec3 color = From_Pixel(get_pixel(x, y));

    return color;
}
