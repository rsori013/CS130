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


// // current one im using
// vec3 Reflective_Shader::Shade_Surface(const Render_World& render_world, const Ray& ray, const Hit& hit, const vec3& intersection_point, const vec3& normal, int recursion_depth) const {
//     if (shader == nullptr) {
//         return {1, 0, 0};
//     }

//     vec3 direct_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

//     vec3 adjusted_direct_color = (1 - reflectivity) * direct_color;

//     if (recursion_depth > 0) {
//         vec3 reflected_direction = ray.direction - 2 * dot(ray.direction, normal) * normal;
//         Ray reflected_ray(intersection_point, reflected_direction);
//         vec3 reflected_color = render_world.Cast_Ray(reflected_ray, recursion_depth - 1);
        
//         adjusted_direct_color += reflectivity * reflected_color;
//     }

//     return adjusted_direct_color;
// }

// //closest to the acutal ones
// vec3 Reflective_Shader::Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
//     const vec3& intersection_point,const vec3& normal,int recursion_depth) const
// {
//     // First, calculate the color as if it was just a regular shader
//     vec3 color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

//     // If we haven't hit the recursion limit, calculate the reflection
//     if(recursion_depth < render_world.recursion_depth_limit)
//     {
//         // Calculate reflection direction
//         vec3 reflect_direction = ray.direction - 2 * dot(ray.direction, normal) * normal;

//         // Create reflection ray
//         Ray reflect_ray;
//         reflect_ray.endpoint = intersection_point;
//         reflect_ray.direction = reflect_direction;

//         // Cast the reflection ray
//         vec3 reflect_color = render_world.Cast_Ray(reflect_ray, recursion_depth + 1);

//         // Blend the color and the reflection
//         color = (1 - reflectivity) * color + reflectivity * reflect_color;
//     }
    
//     return color;
// }

// close but too dark 
// vec3 Reflective_Shader::Shade_Surface(const Render_World& render_world, const Ray& ray, const Hit& hit,
//     const vec3& intersection_point, const vec3& normal, int recursion_depth) const
// {
//     float exposure = 0.5f; // Adjust this value to your liking. Lower values will result in dimmer images.

//     // First, calculate the color as if it was just a regular shader
//     vec3 color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

//     // If we haven't hit the recursion limit, calculate the reflection
//     if(recursion_depth < render_world.recursion_depth_limit)
//     {
//         // Calculate reflection direction
//         vec3 reflect_direction = ray.direction - 2 * dot(ray.direction, normal) * normal;

//         // Create reflection ray
//         Ray reflect_ray;
//         reflect_ray.endpoint = intersection_point;
//         reflect_ray.direction = reflect_direction;

//         // Cast the reflection ray
//         vec3 reflect_color = render_world.Cast_Ray(reflect_ray, recursion_depth + 1);

//         // Blend the color and the reflection with respect to reflectivity
//         color = (1 - reflectivity) * color + reflectivity * reflect_color;
//     }

//     // Apply exposure
//     color = color * exposure;

//     return color;
// }

// vec3 Reflective_Shader::Shade_Surface(const Render_World& render_world, const Ray& ray, const Hit& hit,
//     const vec3& intersection_point, const vec3& normal, int recursion_depth) const
// {
//     // First, calculate the color as if it was just a regular shader
//     vec3 color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

//     if (shader == nullptr) {
//         // Handle the case where shader is null
//         // (for instance, return a default color)
//         return {1, 0, 0};
//     }
//     // If we haven't hit the recursion limit, calculate the reflection
//     if(recursion_depth < render_world.recursion_depth_limit)
//     {
//         // Calculate reflection direction
//         vec3 reflect_direction = ray.direction - 2 * dot(ray.direction, normal) * normal;

//         // Create reflection ray
//         Ray reflect_ray;
//         reflect_ray.endpoint = intersection_point;
//         reflect_ray.direction = reflect_direction;

//         // Cast the reflection ray
//         vec3 reflect_color = render_world.Cast_Ray(reflect_ray, recursion_depth + 1);

//         // Blend the color and the reflection with respect to reflectivity
//         color = (1 - reflectivity) * color + reflectivity * reflect_color;
//     }


//     return color;
// }

 // one ix passing the other 2 is off colors, 
// vec3 Reflective_Shader::Shade_Surface(const Render_World& render_world, const Ray& ray, const Hit& hit,
//     const vec3& intersection_point, const vec3& normal, int recursion_depth) const
// {
//     if (shader == nullptr) {
//         // Handle the case where shader is null
//         // (for instance, return a default color)
//         return {1, 0, 0};
//     }

//     // First, calculate the color as if it was just a regular shader
//     vec3 color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);
//     // Adjust the direct color by the reflectivity of the surface
//     vec3 adjusted_direct_color = (1 - reflectivity) * color;

//     // If we haven't hit the recursion limit, calculate the reflection
//     if(recursion_depth < render_world.recursion_depth_limit)
//     {
//         // Calculate reflection direction
//         vec3 reflect_direction = ray.direction - 2 * dot(ray.direction, normal) * normal;

//         // Create reflection ray
//         Ray reflect_ray;
//         reflect_ray.endpoint = intersection_point;
//         reflect_ray.direction = reflect_direction;

//         // Cast the reflection ray
//         vec3 reflect_color = render_world.Cast_Ray(reflect_ray, recursion_depth + 1);

//         // Blend the adjusted color and the reflection with respect to reflectivity
//         adjusted_direct_color = (1 - reflectivity) * adjusted_direct_color + reflectivity * reflect_color;
//     }

//     return adjusted_direct_color;
// }


vec3 Reflective_Shader::Shade_Surface(const Render_World& world, const Ray& ray, const Hit& hit, 
        const vec3& intersection_point, const vec3& normal, int recursion_depth) const 
{
    // get the color from the shader
    vec3 color = shader->Shade_Surface(world, ray, hit, intersection_point, normal, recursion_depth);
    vec3 reflected_color(0,0,0);

    if (recursion_depth < world.recursion_depth_limit) 
    {
        // calculate reflected ray direction
        vec3 reflected_dir = ray.direction - 2 * dot(ray.direction, normal) * normal;

        // make reflected ray
        Ray reflected_ray;
        reflected_ray.endpoint = intersection_point;
        reflected_ray.direction = reflected_dir;

        // get color from reflected ray
        reflected_color = world.Cast_Ray(reflected_ray, recursion_depth + 1);
    }

    // mix color and reflected color according to reflectivity
    vec3 result = (1 - reflectivity) * color + reflectivity * reflected_color;

    return result;
}
