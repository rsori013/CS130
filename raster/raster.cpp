#include <vector>
#include <string>
#include <iostream>
#include <algorithm> // For std::min and std::max
#include "common.h"

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris) {
    auto toScreenSpace = [&](float normalized, float size) {
        float result = 0.5f * (normalized + 1.0f) * size;
        if (result < 0) {
            std::cout << "Warning: Negative screen space coordinate: " << result << "\n";
        } else if (result >= size) {
            std::cout << "Warning: Out-of-bounds screen space coordinate: " << result << "\n";
        }
        return result;
    };

    auto insideTriangle = [](float x, float y, const vec3& A, const vec3& B, const vec3& C, float& alpha, float& beta, float& gamma) -> bool {
        float Ax = A[0], Ay = A[1];
        float Bx = B[0], By = B[1];
        float Cx = C[0], Cy = C[1];

        float detT = (By - Cy) * (Ax - Cx) + (Cx - Bx) * (Ay - Cy);
        if (std::abs(detT) < 1e-6) return false;

        alpha = ((By - Cy) * (x - Cx) + (Cx - Bx) * (y - Cy)) / detT;
        beta = ((Cy - Ay) * (x - Cx) + (Ax - Cx) * (y - Cy)) / detT;
        gamma = 1.0f - alpha - beta;

        return (alpha >= 0 && alpha <= 1) && (beta >= 0 && beta <= 1) && (gamma >= 0 && gamma <= 1);
    };

    auto set_pixel = [&](int x, int y, const vec3& color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * width + x] = Pixel_Color(color);
        }
    };

    int triangleCount = 0;
    for (const Triangle& tri : tris) {
        triangleCount++;

        vec3 A = {toScreenSpace(tri.A[0], width), toScreenSpace(tri.A[1], height), 0};
        vec3 B = {toScreenSpace(tri.B[0], width), toScreenSpace(tri.B[1], height), 0};
        vec3 C = {toScreenSpace(tri.C[0], width), toScreenSpace(tri.C[1], height), 0};

        int minX = static_cast<int>(std::min({A[0], B[0], C[0]})); 
        int minY = static_cast<int>(std::min({A[1], B[1], C[1]}));
        int maxX = static_cast<int>(std::max({A[0], B[0], C[0]}));
        int maxY = static_cast<int>(std::max({A[1], B[1], C[1]}));

        // Clipping to screen size
        minX = std::max(0, minX);
        minY = std::max(0, minY);
        maxX = std::min(width - 1, maxX);
        maxY = std::min(height - 1, maxY);

        std::cout << "Triangle #" << triangleCount << " Bounding Box: "
                  << "Min: (" << minX << ", " << minY << ") "
                  << "Max: (" << maxX << ", " << maxY << ")\n";

        int pixelCount = 0;
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                float alpha, beta, gamma;
                if (insideTriangle(x, y, A, B, C, alpha, beta, gamma)) {
                    pixelCount++;
                    vec3 color = static_cast<double>(alpha) * tri.Ca + static_cast<double>(beta) * tri.Cb + static_cast<double>(gamma) * tri.Cc;        
                    set_pixel(x, y, color);
                }
            }
        }

        if (pixelCount == 0) {
            std::cout << "Warning: Triangle #" << triangleCount << " has no rasterized pixels.\n";
        } else {
            std::cout << "Triangle #" << triangleCount << " has " << pixelCount << " rasterized pixels.\n";
        }
    }
}
