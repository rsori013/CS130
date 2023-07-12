#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
// vec3 Transparent_Shader::
// Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
//     const vec3& intersection_point,const vec3& normal,int recursion_depth) const
// {
//     TODO;
//     return {};
// }


// vec3 Transparent_Shader::
// Shade_Surface(const Render_World& world,const Ray& ray,const Hit& hit,
//     const vec3& intersection_point,const vec3& normal,int recursion_depth) const
// {
//     vec3 reflected_color, refracted_color;

//     // Reflection
//     Ray reflected_ray;
//     reflected_ray.endpoint = intersection_point;
//     reflected_ray.direction = ray.direction - 2*dot(ray.direction, normal)*normal;
//     reflected_color = world.Cast_Ray(reflected_ray, recursion_depth-1);

//     // Refraction
//     double n1, n2; // Indices of refraction
//     vec3 n; // Normal for refraction calculation
//     if(dot(ray.direction, normal) > 0) { // Inside the object
//         n1 = index_of_refraction;
//         n2 = 1.0;
//         n = -normal;
//     } else { // Outside the object
//         n1 = 1.0;
//         n2 = index_of_refraction;
//         n = normal;
//     }
//     double ratio = n1/n2;
//     double cos_i = -dot(ray.direction, n);
//     double sin_t2 = ratio*ratio*(1 - cos_i*cos_i);
//     if(sin_t2 <= 1) { // Total internal reflection does not occur
//         vec3 refracted_dir = ratio*ray.direction + (ratio*cos_i - sqrt(1 - sin_t2))*n;
//         Ray refracted_ray;
//         refracted_ray.endpoint = intersection_point;
//         refracted_ray.direction = refracted_dir;
//         refracted_color = world.Cast_Ray(refracted_ray, recursion_depth-1);
//     }

//     // Fresnel
//     double r0 = (n1 - n2) / (n1 + n2);
//     r0 = r0*r0;
//     double r_theta = r0 + (1-r0)*pow(1 - cos_i, 5); // Schlick's approximation
//     vec3 color = r_theta*reflected_color + (1 - r_theta)*refracted_color;

//     return (1 - opacity)*color + opacity*shader->Shade_Surface(world, ray, hit, intersection_point, normal, recursion_depth);
// }
vec3 Transparent_Shader::
Shade_Surface(const Render_World& world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 reflected_color, refracted_color;

    const double bias = 0.0001; // Adjust as needed

    // Reflection
    Ray reflected_ray;
    reflected_ray.endpoint = intersection_point;
    reflected_ray.direction = ray.direction - 2*dot(ray.direction, normal)*normal;
    reflected_ray.endpoint += bias * reflected_ray.direction; // Add bias
    reflected_color = world.Cast_Ray(reflected_ray, recursion_depth-1);

    // Refraction
    double n1, n2; // Indices of refraction
    vec3 n; // Normal for refraction calculation
    if(dot(ray.direction, normal) > 0) { // Inside the object
        n1 = index_of_refraction;
        n2 = 1.0;
        n = -normal;
    } else { // Outside the object
        n1 = 1.0;
        n2 = index_of_refraction;
        n = normal;
    }
    double ratio = n1/n2;
    double cos_i = -dot(ray.direction, n);
    double sin_t2 = ratio*ratio*(1 - cos_i*cos_i);
    if(sin_t2 <= 1) { // Total internal reflection does not occur
        vec3 refracted_dir = ratio*ray.direction + (ratio*cos_i - sqrt(1 - sin_t2))*n;
        Ray refracted_ray;
        refracted_ray.endpoint = intersection_point;
        refracted_ray.direction = refracted_dir;
        refracted_ray.endpoint += bias * refracted_ray.direction; // Add bias
        refracted_color = world.Cast_Ray(refracted_ray, recursion_depth-1);
    }

    // Fresnel
    double r0 = (n1 - n2) / (n1 + n2);
    r0 = r0*r0;
    double r_theta;
    if(cos_i == 0) { // Handle edge case
        r_theta = r0;
    } else {
        r_theta = r0 + (1-r0)*pow(1 - cos_i, 5); // Schlick's approximation
    }
    vec3 color = r_theta*reflected_color + (1 - r_theta)*refracted_color;

    if(recursion_depth <= 0) { // Add absorption
        return opacity*color + (1-opacity)*vec3(0,0,0);
    }
    else {
        return (1 - opacity)*color + opacity*shader->Shade_Surface(world, ray, hit, intersection_point, normal, recursion_depth);
    }
}
