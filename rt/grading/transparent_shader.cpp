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
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    TODO;
    return {};
}
// vec3 Transparent_Shader::
// Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
//     const vec3& intersection_point,const vec3& normal,int recursion_depth) const
// {
//     vec3 out_color;
//     vec3 r = ray.direction - 2 * dot(ray.direction, normal) * normal; // Reflection direction
//     vec3 refraction_dir;
//     double n1 = 1.0; // index of refraction of the medium the ray is currently in (assuming air)
//     double n2 = index_of_refraction; // index of refraction of the medium the ray is entering (object material)

//     double cos_i = -dot(normal, ray.direction); // cos of the angle between the ray direction and the normal
//     double sin_t2 = n1*n1*(1.0 - cos_i*cos_i) / (n2*n2); // sin^2 of the refraction angle

//     if (sin_t2 <= 1.0) { // total internal reflection did not occur
//         double cos_t = sqrt(1.0 - sin_t2);
//         refraction_dir = (n1 / n2) * ray.direction + (n1*cos_i - n2*cos_t) * normal;
//     } else { // total internal reflection, no refraction
//         refraction_dir = r;
//     }

//     // recursively trace reflection
//     Ray reflection_ray(intersection_point, r);
//     vec3 reflection_color = render_world.Cast_Ray(reflection_ray, recursion_depth - 1);

//     // recursively trace refraction
//     Ray refraction_ray(intersection_point, refraction_dir);
//     vec3 refraction_color = render_world.Cast_Ray(refraction_ray, recursion_depth - 1);

//     // Schlick's approximation
//     double R0 = (n1 - n2) / (n1 + n2);
//     R0 = R0*R0;
//     double R_theta = R0 + (1 - R0) * pow(1 - cos_i, 5);

//     // output color is a mix of reflection and refraction
//     out_color = R_theta * reflection_color + (1 - R_theta) * refraction_color;

//     return out_color;
// }
