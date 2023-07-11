//Tri Tran & Rovin Soriano
#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

Hit Plane::Intersection(const Ray& ray, int part) const
{
    double denominator = dot(normal, ray.direction);

    // If denominator is zero, ray is parallel to plane and does not intersect
    if (denominator == 0)
    {
        return { -1, -1, {0, 0}}; // invalid hit
    }

    double t = dot(normal, (x - ray.endpoint)) / denominator;

    // If t is negative, ray intersection is behind the ray origin
    if (t < small_t)
    {
        return { -1, -1, {0, 0}}; // invalid hit
    }

    return { t, -1, {0, 0}}; // valid hit
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
