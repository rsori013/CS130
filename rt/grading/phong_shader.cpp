//Tri Tran & Rovin Soriano
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

//updated for texture
vec3 Phong_Shader::Shade_Surface(const Render_World& world,const Ray& incoming_ray, const Hit& hit, 
        const vec3& intersection_point, const vec3& normal, int recursion_depth) const 
{   

    Ray ray_to_light;
    vec3 emitted_light_color;
    vec3 final_color = vec3(0,0,0);

    // Compute ambient color contribution
    vec3 ambient_color = color_ambient ? color_ambient->Get_Color(hit.uv) : vec3(1,1,1);
    vec3 global_ambient_color = world.ambient_color ? world.ambient_color->Get_Color(hit.uv) : vec3(1,1,1);
    final_color += ambient_color * (global_ambient_color * world.ambient_intensity);

    // Loop through each light in the scene
    for(auto& light : world.lights) {
        ray_to_light.direction = light->position - intersection_point; // Correcting direction
        double distance_to_light_squared = ray_to_light.direction.magnitude_squared();
        ray_to_light.direction = ray_to_light.direction.normalized();
        ray_to_light.endpoint = intersection_point; // ray starts from intersection point

        if(world.enable_shadows) {
            Ray intersection_to_light(intersection_point, ray_to_light.direction);
            auto closest_obj_hit = world.Closest_Intersection(intersection_to_light);

            // If there's an object obstructing the light
            if(closest_obj_hit.first.IsValid() && closest_obj_hit.second.dist * closest_obj_hit.second.dist < distance_to_light_squared){
                continue; // Skip adding diffuse and specular contributions for this light
            }
        }

        // Compute and add diffuse contribution to final color
        emitted_light_color = light->Emitted_Light(ray_to_light.direction);
        emitted_light_color /= distance_to_light_squared;
        double diff_intensity = std::max(dot(ray_to_light.direction, normal) , 0.0);
        vec3 diffuse_color = color_diffuse ? color_diffuse->Get_Color(hit.uv) : vec3(1,1,1);
        final_color += (emitted_light_color * diffuse_color * diff_intensity);

        // Compute and add specular contribution to final color
        vec3 reflected_dir = (2 * dot(ray_to_light.direction, normal) * normal) - ray_to_light.direction;
        vec3 opposite_ray_dir = -incoming_ray.direction.normalized();
        double spec_intensity = std::pow(std::max(dot(reflected_dir, opposite_ray_dir), 0.0), specular_power);
        vec3 specular_color = color_specular ? color_specular->Get_Color(hit.uv) : vec3(1,1,1);
        final_color += (emitted_light_color * specular_color * spec_intensity);
    }

    return final_color;
}
