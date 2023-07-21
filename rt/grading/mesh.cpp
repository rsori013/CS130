//Tri Tran & Rovin Soriano
#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>
#include "vec.h"

static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse* parse, std::istream& in)
{
    std::string file;
    in >> name >> file;
    Read_Obj(file.c_str());
}

void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if (!fin)
    {
        exit(EXIT_FAILURE);
    }

    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;

    while (fin)
    {
        getline(fin, line);

        if (sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        else if (sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        else if (sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for (int i = 0; i < 3; i++) e[i]--;
            triangles.push_back(e);
            for (int i = 0; i < 3; i++) t[i]--;
            triangle_texture_index.push_back(t);
        }

        else if (sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for (int i = 0; i < 3; i++) e[i]--;
            triangles.push_back(e);
            // Default UVs for triangles without UVs
            triangle_texture_index.push_back(ivec3(-1, -1, -1));
        }
    }
    num_parts = triangles.size();
}

Hit Mesh::Intersection(const Ray& ray, int part) const
{
    Hit min_hit;
    min_hit.dist = std::numeric_limits<double>::max();
    min_hit.triangle = -1;

    for (size_t i = 0; i < triangles.size(); i++)
    {
        Hit hit = Intersect_Triangle(ray, i);
        if (hit.dist > small_t && hit.dist < min_hit.dist)
        {
            min_hit = hit;
            min_hit.triangle = i;
        }
    }

    return min_hit;
}

vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    assert(hit.triangle >= 0);
    ivec3 triangle = triangles[hit.triangle];
    vec3 A = vertices[triangle[0]];
    vec3 B = vertices[triangle[1]];
    vec3 C = vertices[triangle[2]];
    vec3 edge1 = B - A;
    vec3 edge2 = C - A;
    vec3 normal = cross(edge1, edge2).normalized();
    return normal;
}

Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    Hit hit;
    hit.dist = -1;  // Default to no hit

    ivec3 triangleIndices = triangles[tri];
    vec3 vertexA = vertices[triangleIndices[0]];
    vec3 vertexB = vertices[triangleIndices[1]];
    vec3 vertexC = vertices[triangleIndices[2]];

    vec3 edge1 = vertexB - vertexA;
    vec3 edge2 = vertexC - vertexA;
    vec3 h = cross(ray.direction, edge2);
    double a = dot(edge1, h);

    if (a > -weight_tolerance && a < weight_tolerance)
        return hit;  // Ray is parallel to the triangle.

    double f = 1.0/a;
    vec3 s = ray.endpoint - vertexA;  // FIXED: ray.origin replaced by ray.endpoint
    double u = f * dot(s, h);

    if (u < 0.0 || u > 1.0)
        return hit;

    vec3 q = cross(s, edge1);
    double v = f * dot(ray.direction, q);

    if (v < 0.0 || u + v > 1.0)
        return hit;

    // Compute intersection distance t
    double t = f * dot(edge2, q);

    if (t > weight_tolerance) // We've got a hit
    {
        hit.dist = t;
        hit.uv = vec2(u, v);  // Barycentric coordinates

        if (triangle_texture_index.size() > static_cast<size_t>(tri)) 
        {
            ivec3 uvIndices = triangle_texture_index[tri];
            vec2 uvA = (uvIndices[0] != -1) ? uvs[uvIndices[0]] : vec2();
            vec2 uvB = (uvIndices[1] != -1) ? uvs[uvIndices[1]] : vec2();
            vec2 uvC = (uvIndices[2] != -1) ? uvs[uvIndices[2]] : vec2();

            hit.uv = uvA * (1.0 - u - v) + uvB * u + uvC * v;  // Interpolate UV based on barycentric coords
        }
    }

    return hit;
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
