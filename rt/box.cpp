#include <limits>
#include "box.h"

// Return whether the ray intersects this box.
std::pair<bool,double> Box::Intersection(const Ray& ray) const
{
    double tmin = (lo[0] - ray.endpoint[0]) / ray.direction[0];
    double tmax = (hi[0] - ray.endpoint[0]) / ray.direction[0];

    if (tmin > tmax) std::swap(tmin, tmax);

    for (int i = 1; i < 3; i++) {
        double tmin_i = (lo[i] - ray.endpoint[i]) / ray.direction[i];
        double tmax_i = (hi[i] - ray.endpoint[i]) / ray.direction[i];

        if (tmin_i > tmax_i) std::swap(tmin_i, tmax_i);

        if ((tmin > tmax_i) || (tmin_i > tmax)) return {false, 0};

        if (tmin_i > tmin) tmin = tmin_i;
        if (tmax_i < tmax) tmax = tmax_i;
    }

    return {true, tmin};
}

// Compute the smallest box that contains both *this and bb.
Box Box::Union(const Box& bb) const
{
    Box box;
    for (int i = 0; i < 3; i++) {
        box.lo[i] = std::min(lo[i], bb.lo[i]);
        box.hi[i] = std::max(hi[i], bb.hi[i]);
    }
    return box;
}

// Compute the smallest box that contains both *this and bb.
Box Box::Intersection(const Box& bb) const
{
   Box box;
    for (int i = 0; i < 3; i++) {
        box.lo[i] = std::max(lo[i], bb.lo[i]);
        box.hi[i] = std::min(hi[i], bb.hi[i]);
        // if the boxes don't overlap, return an empty box
        if (box.lo[i] > box.hi[i]) {
            box.Make_Empty();
            return box;
        }
    }
    return box;
}

// Enlarge this box (if necessary) so that pt also lies inside it.
void Box::Include_Point(const vec3& pt)
{
     for (int i = 0; i < 3; i++) {
        lo[i] = std::min(lo[i], pt[i]);
        hi[i] = std::max(hi[i], pt[i]);
    }
}

// Create a box to which points can be correctly added using Include_Point.
void Box::Make_Empty()
{
    lo.fill(std::numeric_limits<double>::infinity());
    hi=-lo;
}

// Create a box that contains everything.
void Box::Make_Full()
{
    hi.fill(std::numeric_limits<double>::infinity());
    lo=-hi;
}

bool Box::Test_Inside(const vec3& pt) const
{
    for(int i=0;i<3;i++)
        if(pt[i]<lo[i] || pt[i]>hi[i])
            return false;
    return true;
}

// Useful for debugging
std::ostream& operator<<(std::ostream& o, const Box& b)
{
    return o << "(lo: " << b.lo << "; hi: " << b.hi << ")";
}
