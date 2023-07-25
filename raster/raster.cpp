#include <vector>
#include <string>
#include <iostream>
#include "common.h"

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris)
{
    // 2d z-buffer array
    double zBuffer[width][height];
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            zBuffer[i][j] = 1;
        }
    }

    for (auto i : tris) {
        //4d to 3d
        vec3 X (i.A[0] / i.A[3], i.A[1] / i.A[3], i.A[2] / i.A[3]);
        vec3 Y (i.B[0] / i.B[3], i.B[1] / i.B[3], i.B[2] / i.B[3]);
        vec3 Z (i.C[0] / i.C[3], i.C[1] / i.C[3], i.C[2] / i.C[3]);

        for (int j = 0; j < height; ++j) {
            for (int k = 0; k < width; ++k) {
                // barycentric coords
                vec3 pix(((k + 0.5)/ (double)width) * 2 - 1, ((j + 0.5) / (double)height) * 2 - 1, 0.0);
                vec3 vPvX = pix - X;
                vec3 vYvX = Y - X;
                vec3 vZvX = Z - X;
                double alpha, beta, gamma;
                double deno = vYvX[0] * vZvX[1] - vYvX[1] * vZvX[0];
                beta = (vPvX[0] * vZvX[1] - vPvX[1] * vZvX[0]) / deno;
                gamma = (vYvX[0] * vPvX[1] - vYvX[1] * vPvX[0]) / deno;
                alpha = 1.0 - beta - gamma;

                if (!(alpha < 0 || beta < 0 || gamma < 0)) {
                    // perspective correction
                    double cv = alpha * X[2] + beta * Y[2] + gamma * Z[2];
                    double corrected_alpha = alpha / (cv * i.A[3]);
                    double corrected_beta = beta / (cv * i.B[3]);
                    double corrected_gamma = gamma / (cv * i.C[3]);
                    double cbary = corrected_alpha + corrected_beta + corrected_gamma;
                    corrected_alpha /= cbary;
                    corrected_beta /= cbary;
                    corrected_gamma /= cbary;
                    // assign color and update z-buffer
                    vec3 color = corrected_alpha * i.Ca + corrected_beta * i.Cb + corrected_gamma * i.Cc;
                    if (cv < zBuffer[k][j]) {
                        zBuffer[k][j] = cv;
                        set_pixel(pixels, width, height, k, j, color);
                    }
                }
            }
        }
    }
}


