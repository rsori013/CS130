#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>
#include "vec.h"

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse* parse, std::istream& in)
{
    std::string file;
    in>>name>>file;
    Read_Obj(file.c_str());
}

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }

        if(sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        if(sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
            for(int i=0;i<3;i++) t[i]--;
            triangle_texture_index.push_back(t);
        }
    }
    num_parts=triangles.size();
}

//Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    TODO;
    return {};
}
//having lots of errors:
// Hit Mesh::Intersection(const Ray& ray, int part) const
// {
//     if(part >= 0){
//         return Intersect_Triangle(ray, part);
//     }else{
//         // Check all triangles
//         double min_t = std::numeric_limits<double>::max();
//         int min_i = -1;
//         for(int i = 0; i < num_parts; i++){
//             Hit hit = Intersect_Triangle(ray, i);
//             if(hit.Valid() && hit.dist < min_t){ // Modify this line
//                 min_t = hit.dist;
//                 min_i = i;
//             }
//         }
//         if(min_i != -1){
//             return Intersect_Triangle(ray, min_i);
//         }
//     }
//     return {std::numeric_limits<double>::quiet_NaN(), -1, vec2()}; // Return invalid hit
// }


//Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    assert(hit.triangle>=0);
    TODO;
    return vec3();
}
//having lots of errors:
// vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
// {
//     assert(hit.triangle>=0);
//     ivec3 e = triangles[hit.triangle];
//     vec3 edge1 = vertices[e[1]] - vertices[e[0]];
//     vec3 edge2 = vertices[e[2]] - vertices[e[0]];
//     vec3 normal = edge1.cross(edge2);
//     return normal.normalized();
// }


// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    TODO;
    return {};
}
//having lots of errrors not sure what we are doing
// Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
// {
//     const double SMALL_NUM = 0.00000001; // to prevent divide by zero
//     vec3 edge1, edge2, h, s, q;
//     double a,f,u,v;

//     ivec3 e = triangles[tri];
//     vec3 vertex0 = vertices[e[0]];
//     vec3 vertex1 = vertices[e[1]];
//     vec3 vertex2 = vertices[e[2]];

//     edge1 = vertex1 - vertex0;
//     edge2 = vertex2 - vertex0;
//     h = ray.direction.cross(edge2);
//     a = edge1.dot(h);

//     if (a > -SMALL_NUM && a < SMALL_NUM)
//         return {std::numeric_limits<double>::quiet_NaN(), -1, vec2()};    // This ray is parallel to this triangle.

//     f = 1.0/a;
//     s = ray.endpoint - vertex0;
//     u = f * (s.dot(h));
//     if (u < 0.0 || u > 1.0)
//         return {std::numeric_limits<double>::quiet_NaN(), -1, vec2()};

//     q = s.cross(edge1);
//     v = f * ray.direction.dot(q);
//     if (v < 0.0 || u + v > 1.0)
//         return {std::numeric_limits<double>::quiet_NaN(), -1, vec2()};

//     // At this stage we can compute t to find out where the intersection point is on the line.
//     double t = f * edge2.dot(q);

//     if (t > SMALL_NUM) // ray intersection
//     {
//         return {t, tri, vec2(u,v)}; // we've hit the triangle
//     }
//     else // This means that there is a line intersection but not a ray intersection.
//         return {std::numeric_limits<double>::quiet_NaN(), -1, vec2()};
// }

std::pair<Box,bool> Mesh::Bounding_Box(int part) const
{
    if(part<0)
    {
        Box box;
        box.Make_Empty();
        for(const auto& v:vertices)
            box.Include_Point(v);
        return {box,false};
    }

    ivec3 e=triangles[part];
    vec3 A=vertices[e[0]];
    Box b={A,A};
    b.Include_Point(vertices[e[1]]);
    b.Include_Point(vertices[e[2]]);
    return {b,false};
}
