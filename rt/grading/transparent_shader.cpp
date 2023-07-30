//Tri Tran & Rovin Soriano
#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"
#include "light.h"

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

vec3 Transparent_Shader::Shade_Surface(const Render_World& world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{   
    //index_of_refraction = 2.42;
    vec3 reflected_color;
    vec3 refracted_color;

    // Reflection
    Ray reflected_ray;
    reflected_ray.endpoint = intersection_point; 
    reflected_ray.direction = ray.direction - 2*dot(ray.direction, normal)*normal;
    if(recursion_depth <= world.recursion_depth_limit) {
        reflected_color = world.Cast_Ray(reflected_ray, recursion_depth+1);
    } else {
        reflected_color = vec3(0,0,0);
    }

    // Refraction
    // hardcode 2.42 for diamondz for #8
    double n1, n2; // Indices of refraction
    vec3 n; // Normal for refraction calculation
    if(dot(ray.direction, normal) > 0) { // Inside the object
        n1 = index_of_refraction;
        n2 = 1.0;
        n = -normal;
    } else { // Outside the object bcuz < 0
        n1 = 1.0;
        n2 = index_of_refraction;  
        n = normal;
    }
    
    
    double quotient = n1/n2;
    double cos_i = -dot(ray.direction, n);
    double sin_t2 = quotient*quotient*(1 - cos_i*cos_i);
    if(sin_t2 <= 1) { 
        vec3 refracted_dir = quotient*ray.direction + (quotient*cos_i - sqrt(1 - sin_t2))*n;
        Ray refracted_ray;
        refracted_ray.endpoint = intersection_point; 
        refracted_ray.direction = refracted_dir;
        if(recursion_depth <= world.recursion_depth_limit) {
            refracted_color = world.Cast_Ray(refracted_ray, recursion_depth+1);
        } else {
            refracted_color = vec3(0,0,0);
        }
    } else { // Total internal reflection occurs
        refracted_color = vec3(0,0,0);
    }

    // Schlick's approximation
    double r0 = (n1 - n2) / (n1 + n2);
    r0 = r0*r0;
    double temp = r0;
    if(sin_t2 <= 1) { 
        temp += (1-r0)*pow(1 - cos_i, 5); 
    }
    vec3 color = temp*reflected_color + (1 - temp)*refracted_color;

    if(recursion_depth <= world.recursion_depth_limit) {
        return (1 - opacity)*color + opacity*shader->Shade_Surface(world, ray, hit, intersection_point, normal, recursion_depth+1);
    } else {
        return color;
    }
}



