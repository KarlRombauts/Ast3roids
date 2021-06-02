#include "NoiseDistortion.h"

std::vector<Vector3> & distortMesh(std::vector<Vector3> &vertices, float amount) {
    for (Vector3 &vertex: vertices) {
        float halfAmount = amount / 2;
        vertex = vertex.scale(randf(1 - halfAmount, 1 + halfAmount));
    }
    return vertices;
}