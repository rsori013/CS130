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
//}

vec3 Phong_Shader::Shade_Surface(const Render_World& world, const Ray& ray, const Hit& hit, const vec3& intersection_point, const vec3& normal, int recursion_depth) const
{   
    // Initialize color with ambient light
    vec3 color = color_ambient->Get_Color(vec2(0,0)) * world.ambient_color->Get_Color(vec2(0,0)) * world.ambient_intensity;

    // Iterate over each light source in the world
    for(auto light : world.lights){
        // Calculate direction vector from intersection point to the light source and normalize it
        vec3 light_direction = (light->position - intersection_point).normalized();

        // Calculate the distance from intersection point to the light source
        double distance_to_light = (light->position - intersection_point).magnitude();

        // Initialize a new shadow ray starting at the intersection point and moving towards the light
        Ray shadow_ray;
        shadow_ray.endpoint = intersection_point;
        shadow_ray.direction = light_direction;

        // Calculate the closest intersection with the shadow ray
        auto shadow_hit = world.Closest_Intersection(shadow_ray);

        // Check if there is any object between the intersection point and the light
        if(shadow_hit.second.dist < distance_to_light && shadow_hit.first.object != nullptr)
            continue;   // Skip to the next light source if the light is blocked

        // Calculate reflected ray and normalize it
        vec3 reflected = (2*dot(light_direction, normal)*normal - light_direction).normalized();

        // Calculate view vector (from intersection point to the camera) and normalize it
        vec3 view = (world.camera.position - intersection_point).normalized();

        // Calculate the diffuse and specular components
        double diffuse = std::max(0.0, dot(normal, light_direction));
        double specular = std::pow(std::max(0.0, dot(view, reflected)), specular_power);

        // Add the contribution from this light source to the color
        color += color_diffuse->Get_Color(vec2(0,0)) * light->Emitted_Light(shadow_ray.direction) * diffuse;
        color += color_specular->Get_Color(vec2(0,0)) * light->Emitted_Light(shadow_ray.direction) * specular;
    }

    return color;
}







