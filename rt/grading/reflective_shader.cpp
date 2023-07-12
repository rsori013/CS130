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

//pass one test case with this one
// vec3 Reflective_Shader::
// Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
//     const vec3& intersection_point,const vec3& normal,int recursion_depth) const
// {
//     if (shader == nullptr) {
//         // Handle the case where shader is null
//         // (for instance, return a default color)
//         return {1, 0, 0};
//     }

//     // Calculate the direct illumination color at the intersection point.
//     vec3 direct_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

//     // Adjust the direct color by the reflectivity of the surface bz the output we are getting is very bright.
//     vec3 adjusted_direct_color = (1 - reflectivity) * direct_color;

//     return adjusted_direct_color;
// }



vec3 Reflective_Shader::Shade_Surface(const Render_World& render_world, const Ray& ray, const Hit& hit, const vec3& intersection_point, const vec3& normal, int recursion_depth) const {
    if (shader == nullptr) {
        return {1, 0, 0};
    }

    vec3 direct_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

    vec3 adjusted_direct_color = (1 - reflectivity) * direct_color;

    if (recursion_depth > 0) {
        vec3 reflected_direction = ray.direction - 2 * dot(ray.direction, normal) * normal;
        Ray reflected_ray(intersection_point, reflected_direction);
        vec3 reflected_color = render_world.Cast_Ray(reflected_ray, recursion_depth - 1);

        // Debugging output
       // std::cout << "Reflected Color: (" << reflected_color[0] << ", " << reflected_color[1] << ", " << reflected_color[2] << ")\n";
        
        adjusted_direct_color += reflectivity * reflected_color;
    }

    return adjusted_direct_color;
}

