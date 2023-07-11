#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

// vec3 Reflective_Shader::
// Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
//     const vec3& intersection_point,const vec3& normal,int recursion_depth) const
// {
//     TODO; // determine the color
//     return {};
// }

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    if (shader == nullptr) {
        // Handle the case where shader is null
        // (for instance, return a default color)
        return {1, 0, 0};
    }

    // Calculate the direct illumination color at the intersection point.
    vec3 direct_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

    // Adjust the direct color by the reflectivity of the surface bz the output we are getting is very bright.
    vec3 adjusted_direct_color = (1 - reflectivity) * direct_color;

    return adjusted_direct_color;
}


