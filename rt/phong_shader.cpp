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

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color;
    //3 components (specular, ambient, diffuse) make up phong reflection
    vec3 specular;
    vec3 ambient;
    vec3 diffuse;
    //light direction
    vec3 light_direction;

    light_direction = ray.direction;

    //loop for light array
    for (const Light* l : render_world.lights) {
        //check if shadow is enabled or disabled
        if (render_world.enable_shadows == false) {
            //diffuse calculation
            diffuse = color_diffuse->Get_Color(hit.uv);
            diffuse *= std::max(dot((l->position - intersection_point).normalized(),normal),0.0); 
            diffuse *= l->Emitted_Light(l->position - intersection_point);
            color += diffuse;
            //specular calculation
            specular = color_specular->Get_Color(hit.uv) * std::pow(std::max(dot((2 * dot((l->position - intersection_point),normal) * normal - (l->position - intersection_point)).normalized(),-light_direction),0.0),specular_power) * l->Emitted_Light(l->position - intersection_point);
            color += specular;
        }
        else if (render_world.enable_shadows == true){
            //check for shadow if enabled
            Ray rshadow(intersection_point, l->position - intersection_point);
            std::pair<Shaded_Object,Hit> hshadow = render_world.Closest_Intersection(rshadow);
            if (hshadow.second.dist > (l->position - intersection_point).magnitude() ||!hshadow.second.Valid()) {
                //diffuse calculation
                diffuse = color_diffuse->Get_Color(hit.uv) * std::max(dot((l->position - intersection_point).normalized(),normal),0.0) * l->Emitted_Light(l->position - intersection_point);
                color += diffuse;
                //specular calculation
                specular = color_specular->Get_Color(hit.uv) * std::pow(std::max(dot((2 * dot((l->position - intersection_point),normal) * normal - (l->position - intersection_point)).normalized(),-light_direction),0.0),specular_power) * l->Emitted_Light(l->position - intersection_point);
                color += specular;
            }
        }
    }
    if (render_world.ambient_color != nullptr) {
        ambient = color_ambient->Get_Color(hit.uv) * render_world.ambient_intensity * render_world.ambient_color->Get_Color(hit.uv);
    }
    else if (render_world.ambient_color == nullptr) {
        ambient = color_ambient->Get_Color(hit.uv) * render_world.ambient_intensity;
    }
    color += ambient;

    return color;
}
