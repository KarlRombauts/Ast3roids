#include "IcoSphere.h"
#include <vector>
#include <Vector3.h>
#include <cmath>
#include <Helpers/Normals.h>

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

    IcoSphere::subdivide(geometry, 1);
    Normals::recalculate(geometry);
    return geometry;
}

void IcoSphere::subdivide(Geometry &geometry, int subdivision) {
    std::vector<Vector3> tmpVertices;
    std::vector<TriangleIndices> tmpTriangles;
    unsigned int index;

    for (int i = 1; i <= subdivision; ++i) {
        // copy prev vertex/index arrays and clear
//        tmpVertices = geometry.vertices;
        tmpTriangles = geometry.triangles;
//        geometry.vertices.clear();
        geometry.triangles.clear();


        // perform subdivision for each triangle
        for (int j = 0; j < tmpTriangles.size(); j++) {
            // get 3 vertices of a triangle
            index = geometry.vertices.size();

            GLuint i1 = tmpTriangles[j].v1;
            GLuint i2 = tmpTriangles[j].v2;
            GLuint i3 = tmpTriangles[j].v3;

            Vector3 &v1 = geometry.vertices[i1];
            Vector3 &v2 = geometry.vertices[i2];
            Vector3 &v3 = geometry.vertices[i3];

            Vector3 v4 = computeHalfVertex(v1, v2);
            Vector3 v5 = computeHalfVertex(v2, v3);
            Vector3 v6 = computeHalfVertex(v1, v3);

            GLuint i4 = -1;
            GLuint i5 = -1;
            GLuint i6 = -1;

            for (int k = 0; k < geometry.vertices.size(); k++) {
                Vector3 &currentVector = geometry.vertices[k];
                if (currentVector.doubleEquals(v4)) {
                    i4 = k;
                } else if (currentVector.doubleEquals(v5)) {
                    i5 = k;
                } else if (currentVector.doubleEquals(v6)) {
                    i6 = k;
                }

                if (i4 != -1 && i5 != -1 && i6 != -1) {
                    break;
                }
            }

            if (i4 == -1) {
                geometry.vertices.push_back(v4);     // index + 0
                i4 = index++;
            }

            if (i5 == -1) {
                geometry.vertices.push_back(v5);     // index + 1
                i5 = index++;
            }
            if (i6 == -1) {
                geometry.vertices.push_back(v6);     // index + 2
                i6 = index++;
            }

//               v1
//               /\
//           v4 /__\ v6
//             /\  /\
//         v2 /__\/__\ v3
//               v5

            geometry.triangles.emplace_back(i1, i4, i6);
            geometry.triangles.emplace_back(i4, i2, i5);
            geometry.triangles.emplace_back(i6, i5, i3);
            geometry.triangles.emplace_back(i4, i5, i6);
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
