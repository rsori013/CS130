//Tri Tran & Rovin Soriano
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
    Hit hit;
    hit.dist = std::numeric_limits<double>::max();
    hit.triangle = -1;
    for (size_t i = 0; i < triangles.size(); i++)
    {
        Hit hit2 = Intersect_Triangle(ray, i);
        if (hit2.dist > small_t && hit2.dist < hit.dist)
        {
            hit = hit2;
            hit.triangle = i;
        }
    }
    return hit;
}



//Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    ivec3 triangle = triangles[hit.triangle];
    vec3 A = vertices[triangle[0]];
    vec3 B = vertices[triangle[1]];
    vec3 C = vertices[triangle[2]];
    vec3 edge1 = B - A;
    vec3 edge2 = C - A;
    vec3 normal = cross(edge1, edge2).normalized();
    return normal;
}


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
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const //updated for texture Zzzz tired asf
{
    // Retrieve the triangle vertices.
    vec3 vA = vertices[triangles[tri][0]];
    vec3 vB = vertices[triangles[tri][1]];
    vec3 vC = vertices[triangles[tri][2]];

    // Compute triangle edges.
    vec3 vBvA = vB - vA;
    vec3 vCvA = vC - vA;
    
    // Compute the triangle's normal.
    vec3 normal = cross(vBvA, vCvA).normalized();

    // Check if the ray is parallel to the triangle.
    if (abs(dot(ray.direction, normal)) < weight_tolerance)
    {
        return Hit(); // No intersection.
    }

    // Calculate the distance to the intersection point.
    double dist = dot(normal, (vA - ray.endpoint)) / dot(normal, ray.direction);
    vec3 intersection_point = ray.Point(dist);

    // Calculate the barycentric coordinates.
    double beta = dot(cross((intersection_point - vA), (vC - vA)), normal) / cross(vCvA, vBvA).magnitude();
    double alpha = dot(cross((vB - intersection_point), (vC - intersection_point)), normal) / cross(vCvA, vBvA).magnitude();
    double gamma = 1.0 - beta - alpha;

    // Validate the intersection.
    if (alpha >= 0.0 && beta >= 0.0 && gamma >= 0.0 && dist > small_t)
    {
        Hit hit;
        hit.dist = dist;
        hit.triangle = tri;

        // Ensure the triangle has texture coordinates.
        if (tri < triangle_texture_index.size() &&
            triangle_texture_index[tri][0] < uvs.size() &&
            triangle_texture_index[tri][1] < uvs.size() &&
            triangle_texture_index[tri][2] < uvs.size())
        {
            // Interpolate the texture coordinates using barycentric coordinates.
            hit.uv = alpha * uvs[triangle_texture_index[tri][0]] + 
                     beta * uvs[triangle_texture_index[tri][1]] + 
                     gamma * uvs[triangle_texture_index[tri][2]];
        }
        else
        {
            // If no texture coordinates, use the computed (alpha, beta) directly.
            hit.uv = vec2(alpha, beta);
        }

        return hit;
    }

    return Hit(); // No intersection.
}



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
