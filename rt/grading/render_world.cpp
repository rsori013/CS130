#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

// Student Name: [Shengkai Huang]
// Student ID: [862231625]

extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    Debug_Scope scope;
    Hit closest_hit = {-1,-1,{}};
    Shaded_Object obj;
    double min_t = std::numeric_limits<double>::max();
    for (long unsigned int i = 0; i < objects.size(); ++i) {
        Hit curr_hit = objects[i].object->Intersection(ray, -1);           //ignoring parts
        if (curr_hit.dist < min_t && curr_hit.dist >= small_t) {
            closest_hit = curr_hit;
            min_t = curr_hit.dist;
            obj = objects[i];
        }
        else {
            Pixel_Print("no intersection with ", objects[i].object->name);
        }
    }
    std::pair<Shaded_Object,Hit> h;
    h.first = obj;
    h.second = closest_hit;
    Pixel_Print("intersect test with ",h.first.object->name, "; hit: ", h.second);    
    Pixel_Print("closest intersection; obj: ", h.first.object->name, "; hit: ", h.second);
    return h;
}


// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
        Debug_Scope scope; 
        Pixel_Print("debug pixel: -x ", pixel_index[0], " -y ", pixel_index[1]);
    //TODO; // set up the initial view ray here
    //(end point, direction vector normalized)
    Ray ray(camera.position, (camera.World_Position(pixel_index) - camera.position).normalized());
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++){
        for(int i=0;i<camera.number_pixels[0];i++){
            Render_Pixel(ivec2(i,j));
        }
    }
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    Debug_Scope scope;
    Pixel_Print("cast ray ",ray);    
    vec3 color;
    // determine the color here
    std::pair<Shaded_Object,Hit> closest_hit = Closest_Intersection(ray);
    vec3 surface_point = ray.Point(closest_hit.second.dist);
    //object intersection detected 
    if (closest_hit.second.Valid()) {
        Pixel_Print("call Shade_Surface with location ", surface_point,"; normal: ", closest_hit.first.object->Normal(ray,closest_hit.second));
        color = closest_hit.first.shader->Shade_Surface(*this,ray,closest_hit.second,surface_point,closest_hit.first.object->Normal(ray,closest_hit.second),recursion_depth);
        Debug_Scope scope;
        Pixel_Print("flat shader; color: ", color);
    }
    //no intersection
    else {
        //if test case does not have background shader then return black background
        //else use background_shader
        if (background_shader == nullptr) {
            color = {0,0,0};
        }
        else {
            color = background_shader->Shade_Surface(*this, ray, closest_hit.second, ray.direction, ray.direction, recursion_depth);
        }
    }

    return color;
}
