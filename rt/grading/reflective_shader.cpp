//Tri Tran & Rovin Soriano
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


vec3 Reflective_Shader::Shade_Surface(const Render_World& world, const Ray& ray, const Hit& hit, 
        const vec3& intersection_point, const vec3& normal, int recursion_depth) const 
{
    // get the color from the shader
    vec3 color = shader->Shade_Surface(world, ray, hit, intersection_point, normal, recursion_depth);
    vec3 reflected_color;

    if (recursion_depth < world.recursion_depth_limit) 
    {
        // ~calculate reflected ray direction
        vec3 reflected_dir = ray.direction - 2 * dot(ray.direction, normal) * normal;

        // make reflected ray
        Ray reflected_ray;
        reflected_ray.endpoint = intersection_point;
        reflected_ray.direction = reflected_dir;

        // get color from reflected ray
        reflected_color = world.Cast_Ray(reflected_ray, recursion_depth + 1);
    }

    // mix color and reflected color passing tests now lets go
    vec3 result = (1 - reflectivity) * color + reflectivity * reflected_color;

    return result;
}

