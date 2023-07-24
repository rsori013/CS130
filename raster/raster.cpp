#include <vector>
#include <string>
#include <iostream>
#include <algorithm> // For std::min and std::max
#include "common.h"

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris) {
    auto toScreenSpace = [&](float normalized, float size) {
        return 0.5f * (normalized + 1.0f) * size;
    };

    auto insideTriangle = [](float x, float y, const vec3& A, const vec3& B, const vec3& C, float& alpha, float& beta, float& gamma) -> bool {
        float detT = (B[1] - C[1]) * (A[0] - C[0]) + (C[0] - B[0]) * (A[1] - C[1]);
        if (std::abs(detT) < 1e-6) return false;

        alpha = ((B[1] - C[1]) * (x - C[0]) + (C[0] - B[0]) * (y - C[1])) / detT;
        beta = ((C[1] - A[1]) * (x - C[0]) + (A[0] - C[0]) * (y - C[1])) / detT;
        gamma = 1.0f - alpha - beta;

        return (alpha >= 0) && (alpha <= 1) && (beta >= 0) && (beta <= 1) && (gamma >= 0) && (gamma <= 1);
    };

    for (const Triangle& tri : tris) {
        vec3 A = {toScreenSpace(tri.A[0], width), toScreenSpace(tri.A[1], height), tri.A[2]};
        vec3 B = {toScreenSpace(tri.B[0], width), toScreenSpace(tri.B[1], height), tri.B[2]};
        vec3 C = {toScreenSpace(tri.C[0], width), toScreenSpace(tri.C[1], height), tri.C[2]};

        int minX = static_cast<int>(std::min({A[0], B[0], C[0]}));
        int minY = static_cast<int>(std::min({A[1], B[1], C[1]}));
        int maxX = static_cast<int>(std::max({A[0], B[0], C[0]}));
        int maxY = static_cast<int>(std::max({A[1], B[1], C[1]}));

        // Clipping to screen size
        minX = std::max(0, minX);
        minY = std::max(0, minY);
        maxX = std::min(width - 1, maxX);
        maxY = std::min(height - 1, maxY);

        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                float alpha, beta, gamma;
                if (insideTriangle(static_cast<float>(x), static_cast<float>(y), A, B, C, alpha, beta, gamma)) {
                    // Perspective correction
                    double w_reciprocal = static_cast<double>(alpha) / A[2] + static_cast<double>(beta) / B[2] + static_cast<double>(gamma) / C[2];
                    double perspectiveAlpha = (static_cast<double>(alpha) / A[2]) / w_reciprocal;
                    double perspectiveBeta = (static_cast<double>(beta) / B[2]) / w_reciprocal;
                    double perspectiveGamma = (static_cast<double>(gamma) / C[2]) / w_reciprocal;

                    vec3 color = perspectiveAlpha * tri.Ca + perspectiveBeta * tri.Cb + perspectiveGamma * tri.Cc;

                    if (x >= 0 && x < width && y >= 0 && y < height) {
                        pixels[y * width + x] = Pixel_Color(color);
                    }
                }
            }
        }
    }
}
