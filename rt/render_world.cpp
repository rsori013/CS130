//Tri Tran & Rovin Soriano
#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"
#include "misc.h" // added this 

extern bool enable_acceleration;


Render_World::~Render_World() {
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}



std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    Shaded_Object closest_object;
    Hit closest_hit;
    double min_t = std::numeric_limits<double>::max();
    
   for(const auto& shaded_object : objects) {
    if (shaded_object.object != nullptr) {
        Hit hit = shaded_object.object->Intersection(ray, small_t); // use Intersection method
        if(hit.dist < min_t && hit.dist >= small_t) {
            min_t = hit.dist;
            closest_hit = hit;
            closest_object = shaded_object;
        }
    } 
}

    
    return {closest_object, closest_hit};
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    // //TODO; // set up the initial view ray here
    //  Debug_Scope scope;
    //  Pixel_Print("Render Pixel: ", pixel_index[0], " ", pixel_index[1]);

    // Ray ray;
    // vec3 color=Cast_Ray(ray,1);
    // camera.Set_Pixel(pixel_index,Pixel_Color(color));

    // set up the initial view ray here
    Ray ray;
    ray.endpoint = camera.position;
    ray.direction = (camera.World_Position(pixel_index) - camera.position).normalized();

    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}


vec3 Render_World::Cast_Ray(const Ray& ray, int recursion_depth) const
{
    if (recursion_depth <= 0) {
        return {0, 0, 0}; // return black if recursion depth limit is reached
    }
    
    std::pair<Shaded_Object,Hit> closest = Closest_Intersection(ray);
    
    if (!closest.first.IsValid()) {
        // If no intersection, use background shader
        if(background_shader) {
            return background_shader->Shade_Surface(*this, ray, closest.second, vec3(0,0,0), vec3(0,0,0), recursion_depth);
        } else {
            return {0, 0, 0}; // return black if no background shader is set
        }
    } else {
        // Calculate intersection point and normal
        vec3 intersection = ray.Point(closest.second.dist);
        vec3 normal = closest.first.object->Normal(ray, closest.second);


        // Use shader associated with object
        return closest.first.shader->Shade_Surface(*this, ray, closest.second, intersection, normal, recursion_depth);
    }
}
