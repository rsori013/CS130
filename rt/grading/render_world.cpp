#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"
#include "misc.h" // added this 

extern bool enable_acceleration;

// addded a SolidColor class definition
class SolidColor : public Color {
public:
    vec3 color;

    SolidColor(const vec3& color) : color(color) {}

    virtual vec3 Get_Color(const vec2& uv) const override {
        return color;
    }
};

//added this because of the segfault we getting for 05.txt
Render_World::Render_World() : ambient_color(new SolidColor({1.0f, 1.0f, 1.0f})), ambient_intensity(1.0) {}



Render_World::~Render_World()
{   
    //added this
    delete this->ambient_color;

    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
// std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
// {
//     TODO;
//     return {};
// }
//added
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    Shaded_Object closest_object;
    Hit closest_hit;
    double min_t = std::numeric_limits<double>::max();
    
    for(const auto& shaded_object : objects) {
        Hit hit = shaded_object.object->Intersection(ray, small_t); // use Intersection method
        if(hit.dist < min_t && hit.dist >= small_t) {
            min_t = hit.dist;
            closest_hit = hit;
            closest_object = shaded_object;
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

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
// vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
// {
//     vec3 color;
//     //TODO; // determine the color here
//     Debug_Scope scope; // add a Debug_Scope
//     Pixel_Print("Cast Ray: ", ray, " recursion depth: ", recursion_depth); // add a Pixel_Print

    
//     return color;
// }
//added
vec3 Render_World::Cast_Ray(const Ray& ray, int recursion_depth) const
{
    if (recursion_depth <= 0) {
        return {0, 0, 0}; // return black if recursion depth limit is reached
    }
    
    std::pair<Shaded_Object,Hit> closest = Closest_Intersection(ray);
    
    if (closest.first.object == nullptr) {
        // If no intersection, use background shader
        if(background_shader) {
            return background_shader->Shade_Surface(*this, ray, closest.second, vec3(0,0,0), vec3(0,0,0), recursion_depth-1);
        } else {
            return {0, 0, 0}; // return black if no background shader is set
        }
    } else {
        // Calculate intersection point and normal
        vec3 intersection = ray.Point(closest.second.dist);
        vec3 normal = closest.first.object->Normal(ray, closest.second); // pass Ray and Hit objects
        
        // Use shader associated with object
        return closest.first.shader->Shade_Surface(*this, ray, closest.second, intersection, normal, recursion_depth-1);
    }
}