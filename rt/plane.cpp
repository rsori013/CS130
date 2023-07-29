#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

// Student Name: [Shengkai Huang]
// Student ID: [862231625]

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the plane.  The plane's normal points outside.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    //TODO;
    double t;
    double descrim = dot(normal,ray.direction);
    if (descrim != 0) {
        t = (dot((x - ray.endpoint),normal))/ dot(normal,ray.direction);
        if (t > small_t) {
            return {t, part, {}};
        }
        else {
            return {-1, part, {}};
        }
    }
    return {-1, part, {}};
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}
