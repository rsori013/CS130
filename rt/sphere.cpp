//Tri Tran & Rovin Soriano
#include "sphere.h"
#include "ray.h"

Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}


// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    vec3 diff = ray.endpoint - center;
    double a = dot(ray.direction, ray.direction);
    double b = 2 * dot(ray.direction, diff);
    double c = dot(diff, diff) - radius * radius;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        // no intersection
        return {-1, -1, {0, 0}}; // Invalid hit
    }
    else {
        double t1 = (-b - sqrt(discriminant)) / (2 * a);
        double t2 = (-b + sqrt(discriminant)) / (2 * a);

        if (t1 > small_t && t1 < t2) {
            return {t1, -1, {0, 0}}; // Valid hit
        }
        else if (t2 > small_t) {
            return {t2, -1, {0, 0}}; // Valid hit
        }
    }

    return {-1, -1, {0, 0}}; // Invalid hit
}


vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 point = ray.Point(hit.dist); // calculate intersection point
    vec3 normal = point - center; // vector from center to point
    normal = normal.normalized(); // normalize the vector
    return normal;
}



std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}
