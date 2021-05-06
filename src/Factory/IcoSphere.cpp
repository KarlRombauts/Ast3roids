#include "IcoSphere.h"
#include <vector>
#include <Vector3.h>
#include <cmath>

Geometry IcoSphere::create() {
    double t = (1.0 + sqrt(5.0)) / 2.0;

    Geometry geometry;

    geometry.vertices.emplace_back(-1, t, 0);
    geometry.vertices.emplace_back(1, t, 0);
    geometry.vertices.emplace_back(-1, -t, 0);
    geometry.vertices.emplace_back(1, -t, 0);

    geometry.vertices.emplace_back(0, -1, t);
    geometry.vertices.emplace_back(0, 1, t);
    geometry.vertices.emplace_back(0, -1, -t);
    geometry.vertices.emplace_back(0, 1, -t);

    geometry.vertices.emplace_back(t, 0, -1);
    geometry.vertices.emplace_back(t, 0, 1);
    geometry.vertices.emplace_back(-t, 0, -1);
    geometry.vertices.emplace_back(-t, 0, 1);

    // 5 faces around point 0
    geometry.triangles.emplace_back(0, 11, 5);
    geometry.triangles.emplace_back(0, 5, 1);
    geometry.triangles.emplace_back(0, 1, 7);
    geometry.triangles.emplace_back(0, 7, 10);
    geometry.triangles.emplace_back(0, 10, 11);

    // 5 adjacent faces
    geometry.triangles.emplace_back(1, 5, 9);
    geometry.triangles.emplace_back(5, 11, 4);
    geometry.triangles.emplace_back(11, 10, 2);
    geometry.triangles.emplace_back(10, 7, 6);
    geometry.triangles.emplace_back(7, 1, 8);

    // 5 faces around point 3
    geometry.triangles.emplace_back(3, 9, 4);
    geometry.triangles.emplace_back(3, 4, 2);
    geometry.triangles.emplace_back(3, 2, 6);
    geometry.triangles.emplace_back(3, 6, 8);
    geometry.triangles.emplace_back(3, 8, 9);

    // 5 adjacent faces
    geometry.triangles.emplace_back(4, 9, 5);
    geometry.triangles.emplace_back(2, 4, 11);
    geometry.triangles.emplace_back(6, 2, 10);
    geometry.triangles.emplace_back(8, 6, 7);
    geometry.triangles.emplace_back(9, 8, 1);

    IcoSphere::subdivide(geometry, 2);
    return geometry;
}

void IcoSphere::subdivide(Geometry &geometry, int subdivision) {
    std::vector<Vector3> tmpVertices;
    std::vector<TriangleIndices> tmpTriangles;
    unsigned int index;

    for (int i = 1; i <= subdivision; ++i) {
        // copy prev vertex/index arrays and clear
        tmpVertices = geometry.vertices;
        tmpTriangles = geometry.triangles;
        geometry.vertices.clear();
        geometry.triangles.clear();

        index = 0;

        // perform subdivision for each triangle
        for (int j = 0; j < tmpTriangles.size(); j++) {
            // get 3 vertices of a triangle
            Vector3 &v1 = tmpVertices[tmpTriangles[j].v1];
            Vector3 &v2 = tmpVertices[tmpTriangles[j].v2];
            Vector3 &v3 = tmpVertices[tmpTriangles[j].v3];

            Vector3 newV1 = computeHalfVertex(v1, v2);
            Vector3 newV2 = computeHalfVertex(v2, v3);
            Vector3 newV3 = computeHalfVertex(v1, v3);

            // add 4 new triangles to vertex array
            geometry.vertices.push_back(v1);        // index + 0
            geometry.vertices.push_back(newV1);     // index + 1

            geometry.vertices.push_back(v2);        // index + 2
            geometry.vertices.push_back(newV2);     // index + 3

            geometry.vertices.push_back(v3);        // index + 4
            geometry.vertices.push_back(newV3);     // index + 5

            geometry.triangles.emplace_back(index, index + 1, index + 5);
            geometry.triangles.emplace_back(index + 1, index + 2, index + 3);
            geometry.triangles.emplace_back(index + 5, index + 3, index + 4);
            geometry.triangles.emplace_back(index + 1, index + 3, index + 5);
            index += 6;    // next index
        }
    }
}

Vector3 IcoSphere::computeHalfVertex(const Vector3 &v1, const Vector3 &v2) {
    Vector3 newV;
    newV.x = v1.x + v2.x;
    newV.y = v1.y + v2.y;
    newV.z = v1.z + v2.z;

    return newV.normalize().scale(v1.magnitude());
}
