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
     // UV Coordinates Check
    for (const auto& uv : uvs) {
        std::cout << "UV: " << uv[0] << ", " << uv[1] << std::endl;
    }
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
    //added:
    for (auto& uv : uvs) {
    if (uv[0] < 0) {  // Check the 'u' coordinate
        uv[0] = 1 + uv[0];  // Wraps the UV around
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
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    // Check for valid 'tri' index
    if (tri < 0 || tri >= triangles.size()) {
        std::cerr << "Error: Invalid triangle index " << tri << std::endl;
        return Hit();
    }

    // Retrieve the triangle from the index.
    ivec3 triangle = triangles[tri];

    // Check for valid vertices indices
    for (int i = 0; i < 3; i++) {
        if (triangle[i] < 0 || triangle[i] >= vertices.size()) {
            std::cerr << "Error: Invalid vertex index " << triangle[i] << " for triangle " << tri << std::endl;
            return Hit();
        }
    }

    // Get the vertices of the triangle.
    vec3 vertex0 = vertices[triangle[0]];
    vec3 vertex1 = vertices[triangle[1]];
    vec3 vertex2 = vertices[triangle[2]];

    // Compute triangle edges.
    vec3 edge1 = vertex1 - vertex0;
    vec3 edge2 = vertex2 - vertex0;

    // Continue with the existing intersection computation...
    vec3 h = cross(ray.direction, edge2);
    double a = dot(edge1, h);

    if (a > -weight_tolerance && a < weight_tolerance) 
    {
        return Hit();  // Ray is parallel to the triangle.
    }

    double f = 1.0 / a;
    vec3 s = ray.endpoint - vertex0;
    double u = f * dot(s, h);

    if (u < 0.0 || u > 1.0)
    {
        return Hit();  // Intersection is outside of the triangle.
    }

    vec3 q = cross(s, edge1);
    double v = f * dot(ray.direction, q);

    if (v < 0.0 || u + v > 1.0)
    {
        return Hit();  // Intersection is outside of the triangle.
    }

    // Compute the distance from the ray origin to the intersection point.
    double t = f * dot(edge2, q);

    if (t <= small_t)
    {
        return Hit();  // No intersection.
    }

    // Only check for texture indices if the triangle_texture_index vector isn't empty and the index is valid
    if (!triangle_texture_index.empty() && tri < triangle_texture_index.size()) {
        ivec3 triangleTextureIndices = triangle_texture_index[tri];
        
        // Validate triangle texture indices
        for (int i = 0; i < 3; i++) {
            if (triangleTextureIndices[i] < 0 || triangleTextureIndices[i] >= uvs.size()) {
                std::cerr << "Error: Invalid UV index " << triangleTextureIndices[i] << " for triangle " << tri << std::endl;
                return Hit();
            }
        }

        // Compute the interpolated UV using barycentric coordinates
        vec2 interpolatedUV = u * uvs[triangleTextureIndices[0]] + v * uvs[triangleTextureIndices[1]] + (1.0 - u - v) * uvs[triangleTextureIndices[2]];

        // Initialize the Hit object with UV data.
        Hit hit;
        hit.dist = t;
        hit.triangle = tri;
        hit.uv = interpolatedUV; // Set the interpolated UV coords for texturing
        return hit;
    } else {
        // Non-textured triangle.
        Hit hit;
        hit.dist = t;
        hit.triangle = tri;
        return hit;
    }
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