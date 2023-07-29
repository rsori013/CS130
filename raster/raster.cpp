// Tri Tran & Rovin Soriano
#include <vector>
#include <string>
#include <iostream>
#include "common.h"

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris) {
    // Initialize Z-buffer making it 2D 
    double zBuffer[width][height];
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            zBuffer[x][y] = 1.0; // intialized to 1.0
        }
    }

    // Process each triangle
    for (const auto& triangle : tris) {
        // Convert from 4D to 3D doing the homogenous stuff 
        vec3 vertA(triangle.A[0] / triangle.A[3], triangle.A[1] / triangle.A[3], triangle.A[2] / triangle.A[3]);
        vec3 vertB(triangle.B[0] / triangle.B[3], triangle.B[1] / triangle.B[3], triangle.B[2] / triangle.B[3]);
        vec3 vertC(triangle.C[0] / triangle.C[3], triangle.C[1] / triangle.C[3], triangle.C[2] / triangle.C[3]);

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                // Calculate barycentric coordinates
                vec3 pixelCoordinate(((col + 0.5) / static_cast<double>(width)) * 2 - 1, ((row + 0.5) / static_cast<double>(height)) * 2 - 1, 0.0);
                vec3 deltaPA = pixelCoordinate - vertA;
                vec3 deltaBA = vertB - vertA;
                vec3 deltaCA = vertC - vertA;

                double denominator = deltaBA[0] * deltaCA[1] - deltaBA[1] * deltaCA[0];
                double beta = (deltaPA[0] * deltaCA[1] - deltaPA[1] * deltaCA[0]) / denominator;
                double gamma = (deltaBA[0] * deltaPA[1] - deltaBA[1] * deltaPA[0]) / denominator;
                double alpha = 1.0 - beta - gamma;

                // Check if inside the triangle
                if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                    // Apply perspective correction
                    double currentDepth = alpha * vertA[2] + beta * vertB[2] + gamma * vertC[2];
                    double correctAlpha = alpha / (currentDepth * triangle.A[3]);
                    double correctBeta = beta / (currentDepth * triangle.B[3]);
                    double correctGamma = gamma / (currentDepth * triangle.C[3]);
                    double baryTotal = correctAlpha + correctBeta + correctGamma;

                    correctAlpha /= baryTotal;
                    correctBeta /= baryTotal;
                    correctGamma /= baryTotal;

                    // Calculate interpolated color
                    vec3 interpolatedColor = correctAlpha * triangle.Ca + correctBeta * triangle.Cb + correctGamma * triangle.Cc;

                    // Update pixel color if the current depth is closer
                    if (currentDepth < zBuffer[col][row]) {
                        zBuffer[col][row] = currentDepth;
                        set_pixel(pixels, width, height, col, row, interpolatedColor);
                    }
                }
            }
        }
    }
}

