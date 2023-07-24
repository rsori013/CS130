#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <limits>
#include "common.h"
#include "vec.h"

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris) {
    std::vector<float> depthBuffer(width * height, std::numeric_limits<float>::infinity());

    // Clearing pixel buffer to a default color (black in this case).
    for (int i = 0; i < width * height; i++) {
        pixels[i] = Pixel_Color(vec3(0.0f, 0.0f, 0.0f)); // Assuming black is represented by vec3(0, 0, 0)
    }

    auto barycentric = [](const vec3& A, const vec3& B, const vec3& C, const vec2& P) -> vec3 {
        vec3 v0 = B - A, v1 = C - A, v2 = vec3(static_cast<float>(P[0]), static_cast<float>(P[1]), 0.0f) - A;
        float d00 = dot(v0, v0);
        float d01 = dot(v0, v1);
        float d11 = dot(v1, v1);
        float d20 = dot(v2, v0);
        float d21 = dot(v2, v1);
        float denom = d00 * d11 - d01 * d01;
        vec3 bary;
        bary[1] = (d11 * d20 - d01 * d21) / denom;
        bary[2] = (d00 * d21 - d01 * d20) / denom;
        bary[0] = 1.0f - bary[1] - bary[2];
        return bary;
    };

    for (const Triangle& tri : tris) {
        vec3 A = {tri.A[0] * 0.5f * width + 0.5f * width, tri.A[1] * 0.5f * height + 0.5f * height, tri.A[2]};
        vec3 B = {tri.B[0] * 0.5f * width + 0.5f * width, tri.B[1] * 0.5f * height + 0.5f * height, tri.B[2]};
        vec3 C = {tri.C[0] * 0.5f * width + 0.5f * width, tri.C[1] * 0.5f * height + 0.5f * height, tri.C[2]};

        int minX = std::clamp(static_cast<int>(std::min({A[0], B[0], C[0]})), 0, width - 1);
        int minY = std::clamp(static_cast<int>(std::min({A[1], B[1], C[1]})), 0, height - 1);
        int maxX = std::clamp(static_cast<int>(std::max({A[0], B[0], C[0]})), 0, width - 1);
        int maxY = std::clamp(static_cast<int>(std::max({A[1], B[1], C[1]})), 0, height - 1);

        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                vec2 P = vec2(x, y);
                vec3 baryCoords = barycentric(A, B, C, P);
                
                if (baryCoords[0] >= 0.0f && baryCoords[1] >= 0.0f && baryCoords[2] >= 0.0f) {
                    float depth = A[2]*baryCoords[0] + B[2]*baryCoords[1] + C[2]*baryCoords[2];
                    
                    if (depth < depthBuffer[y * width + x]) {
                        depthBuffer[y * width + x] = depth;
                        vec3 color = baryCoords[0] * tri.Ca + baryCoords[1] * tri.Cb + baryCoords[2] * tri.Cc;
                        
                        // Clamping the color values to the [0, 1] range
                        color[0] = std::clamp(color[0], 0.0, 1.0);
                        color[1] = std::clamp(color[1], 0.0, 1.0);
                        color[2] = std::clamp(color[2], 0.0, 1.0);

                        pixels[y * width + x] = Pixel_Color(color);
                    }
                }
            }
        }
    }
}
