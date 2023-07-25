#include "parse.h"
#include "texture.h"
#include "dump_png.h"
#include <cmath>  // for the floor function

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

vec3 Texture::Get_Color(const vec2& uv) const
{
    // Convert uv coordinates into pixel coordinates.
    float u = uv[0] * width;
    float v = uv[1] * height;

    // In case uv is outside [0, 1], wrap them around.
    u = u - std::floor(u);
    v = v - std::floor(v);

    int x = static_cast<int>(u);
    int y = static_cast<int>(v);

    if (!use_bilinear_interpolation)
    {
        // Nearest neighbor sampling
        Pixel pixel = data[y * width + x];
        return From_Pixel(pixel); // Use the utility function to convert the Pixel to vec3
    }
    else
    {
        // Bilinear interpolation
        int x1 = (x + 1) % width;
        int y1 = (y + 1) % height;

        float u_ratio = u - x;
        float v_ratio = v - y;
        float u_opposite = 1 - u_ratio;
        float v_opposite = 1 - v_ratio;

        vec3 color1 = From_Pixel(data[y * width + x]);
        vec3 color2 = From_Pixel(data[y * width + x1]);
        vec3 color3 = From_Pixel(data[y1 * width + x]);
        vec3 color4 = From_Pixel(data[y1 * width + x1]);

        vec3 col_u1 = color1 * u_opposite + color2 * u_ratio;
        vec3 col_u2 = color3 * u_opposite + color4 * u_ratio;

        vec3 final_color = col_u1 * v_opposite + col_u2 * v_ratio;

        return final_color;
    }
}
