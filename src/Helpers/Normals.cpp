
#include "Normals.h"

void Normals::recalculate(Geometry &geometry) {
    geometry.normals.resize(geometry.vertices.size());

    for (TriangleIndices indices: geometry.triangles) {
        Vector3 &v1 = geometry.vertices[indices.v1];
        Vector3 &v2 = geometry.vertices[indices.v2];
        Vector3 &v3 = geometry.vertices[indices.v3];

        const Vector3 &edge1 = Vector3::fromTo(v1, v2);
        const Vector3 &edge2 = Vector3::fromTo(v1, v3);

        Vector3 normal = edge1.cross(edge2);

        geometry.normals[indices.v1] += normal;
        geometry.normals[indices.v2] += normal;
        geometry.normals[indices.v3] += normal;
    }

    for (Vector3 &normal: geometry.normals) {
        normal = normal.normalize();
    }
}
