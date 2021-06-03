#include "NoiseDistortion.h"
#include "Helpers/PerlinNoise.hpp"

std::vector<Vector3> & distortMesh(std::vector<Vector3> &vertices, float amount, float frequency) {
    const siv::PerlinNoise perlin(rand());
    for (Vector3 &vertex: vertices) {
        float halfAmount = amount / 2;

        double amount = perlin.accumulatedOctaveNoise3D(
                vertex.x * frequency,
                vertex.y * frequency,
                vertex.z * frequency,
                8);

        amount = map(amount, {0, 1}, {1 - halfAmount, 1 + halfAmount});
        vertex = vertex.scale(amount);
    }
    return vertices;
}