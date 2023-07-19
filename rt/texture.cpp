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
