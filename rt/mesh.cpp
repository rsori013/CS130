#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>
#include "plane.h"
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

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    // Check for an intersection in the range [small_t,inf).  If there are
    // multiple intersections, return the closest one.  If there are no
    // intersections, return a Hit structure with dist<0.  If possible
    // also compute hit.uv.  If part>=0, intersect only against part of the
    // primitive.  This is only used for meshes, where part is the triangle
    // index.  If part<0, intersect against all triangles.  For other
    // primitives, part is ignored.
    Hit hit = {-1,-1,{}};
    Hit closest_hit = {-1,-1,{}};
    if (part >= 0) {
        hit = Intersect_Triangle(ray,part);
    }
    else if (part < 0) {
        double min_t = std::numeric_limits<double>::max();
        for (long unsigned int i = 0; i < triangles.size(); ++i) 
        {
            hit = Intersect_Triangle(ray,i);
            if (hit.Valid()) 
            {
                if (hit.dist < min_t) {
                    closest_hit = hit;
                    closest_hit.triangle = i;
                    min_t = hit.dist;
                }
            }
        }
        hit = closest_hit;
    }
    return hit;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    assert(hit.triangle>=0);
    vec3 normal = cross((vertices[triangles[hit.triangle][0]] - vertices[triangles[hit.triangle][1]]),(vertices[triangles[hit.triangle][0]] - vertices[triangles[hit.triangle][2]])).normalized();
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
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    //vertices A, B, C
    //sides of triangle B - A, C - A
    vec3 vA = vertices[triangles[tri][0]];
    vec3 vB = vertices[triangles[tri][1]];
    vec3 vC = vertices[triangles[tri][2]];
    vec3 vBvA = vB - vA;
    vec3 vCvA = vC - vA;
    vec3 normal = cross(vBvA,vCvA).normalized();
    vec3 ray_direction = ray.direction;
    vec3 ray_endpoint = ray.endpoint;
    Hit hit;
    //check if ray is parallel to the triangle meaning no hit
    if (!dot(ray_direction, normal)) {
        return {-1,-1,{}};
    }
    double dist = dot(normal,(vA - ray_endpoint)) / dot(normal,ray_direction);
    vec3 intersection_point = ray.Point(dist);
    //alpha + beta + gamma = 1
    double beta = dot(cross((intersection_point - vA),(vC-vA)), normal) / cross(vCvA, vBvA).magnitude();            // 1/2 gets cancelled out
    double alpha = dot(cross((vB - intersection_point), (vC - intersection_point)), normal) / cross(vCvA, vBvA).magnitude(); // 1/2 gets cancelled out
    double gamma = 1 - beta - alpha;
    // Safety checks
    if(triangle_texture_index.size() <= tri) {
        // This triangle doesn't have a corresponding texture index
        return {-1,-1,{}};
    }

    for(int i = 0; i < 3; i++) {
        if(triangle_texture_index[tri][i] >= uvs.size()) {
            // Texture index out of range for UVs
            return {-1,-1,{}};
        }
    }

   
    hit.uv = alpha * uvs[triangle_texture_index[tri][0]] + beta * uvs[triangle_texture_index[tri][1]] + gamma * uvs[triangle_texture_index[tri][2]];
     if ((alpha > -weight_tolerance) && (beta > -weight_tolerance) && (gamma > -weight_tolerance) && (dist > small_t)) {
        // Check if there are texture coordinates for the triangle
        if (tri < triangle_texture_index.size()) {
            hit.uv = alpha * uvs[triangle_texture_index[tri][0]] + beta * uvs[triangle_texture_index[tri][1]] + gamma * uvs[triangle_texture_index[tri][2]];
        }
        return {dist, tri, hit.uv};
    }
    return {-1,-1,{}};
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
