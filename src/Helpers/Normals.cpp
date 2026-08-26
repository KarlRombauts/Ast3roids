
#include "Normals.h"

void Normals::recalculate(Geometry &geometry) {
    // assign(), not resize(). resize() only value-initialises *new* elements and
    // leaves existing ones alone, so calling this a second time accumulated onto
    // whatever was already there. Both sphere primitives call it once at build
    // time, so a mesh that was displaced and then recalculated ended up with the
    // stale, undisplaced normals still dominating: cross() is left unnormalised,
    // giving each face a magnitude of 2*area, and at 20k triangles that is ~0.007
    // against a leftover unit normal of 1.0. The surface lit as a smooth sphere
    // however far the vertices had actually moved.
    geometry.normals.assign(geometry.vertices.size(), Vector3(0, 0, 0));

    for (Face &face: geometry.faces) {
        TriangleIndices &indices = face.vertIndices;
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
