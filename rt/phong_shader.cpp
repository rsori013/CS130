#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

// vec3 Phong_Shader::
// Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
//     const vec3& intersection_point,const vec3& normal,int recursion_depth) const
// {
//     vec3 color;
//     TODO; //determine the color
//     return color;
// }

// STILL WRONG GIVING ME ERRORS::::
// vec3 Phong_Shader::Shade_Surface(const Render_World& world, const Ray& ray, const Hit& hit,
//     const vec3& intersection_point, const vec3& normal, int recursion_depth) const
// {
//     vec3 color = color_ambient * vec3(world.ambient_color) * world.ambient_intensity;

//     for (auto light : world.lights) {
//         vec3 light_direction = light->position - intersection_point;
//         double distance_to_light = light_direction.magnitude();
//         light_direction = light_direction.normalized();

//         Ray shadow_ray;
//         shadow_ray.endpoint = intersection_point;
//         shadow_ray.direction = light_direction;

//         std::pair<Shaded_Object, Hit> shadow_hit = world.Closest_Intersection(shadow_ray);
//         if (!shadow_hit.first && shadow_hit.second.dist < distance_to_light) {
//             vec3 r = (2 * dot(light_direction, normal) * normal - light_direction).normalized();
//             vec3 v = -ray.direction;

//             color += color_diffuse * vec3(light->Emitted_Light(shadow_ray.direction)) * std::max(0.0, dot(light_direction, normal));
//             color += color_specular * vec3(light->Emitted_Light(shadow_ray.direction)) * std::pow(std::max(0.0, dot(r, v)), specular_power);
//         }
//     }

//     return color;
// }

