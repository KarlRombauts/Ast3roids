#include "IcoSphere.h"
#include <vector>
#include <Vector3.h>
#include <cmath>
#include <Helpers/Normals.h>
#include <Helpers/TextureLoader.h>

Geometry IcoSphere::create(int subdivisions, Material *material) {
    double t = (1.0 + sqrt(5.0)) / 2.0;

    Geometry geometry;
    geometry.shapes.emplace_back("icosphere");
    geometry.materials.emplace_back(material);

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

    for(Vector3 &vert : geometry.vertices) {
        vert = vert.normalize();
    }

    // 5 faces around point 0
    geometry.faces.emplace_back(TriangleIndices(0, 11, 5), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(0, 5, 1), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(0, 1, 7), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(0, 7, 10), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(0, 10, 11), geometry.materials[0]);

    // 5 adjacent faces
    geometry.faces.emplace_back(TriangleIndices(1, 5, 9), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(5, 11, 4), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(11, 10, 2), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(10, 7, 6), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(7, 1, 8), geometry.materials[0]);

    // 5 faces around point 3
    geometry.faces.emplace_back(TriangleIndices(3, 9, 4), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(3, 4, 2), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(3, 2, 6), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(3, 6, 8), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(3, 8, 9), geometry.materials[0]);

    // 5 adjacent faces
    geometry.faces.emplace_back(TriangleIndices(4, 9, 5), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(2, 4, 11), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(6, 2, 10), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(8, 6, 7), geometry.materials[0]);
    geometry.faces.emplace_back(TriangleIndices(9, 8, 1), geometry.materials[0]);

    IcoSphere::subdivide(geometry, subdivisions);
    Normals::recalculate(geometry);

    double piInv = 1 / M_PI;
    double twoPiInv = 1 / (M_PI * 2);

    // Add basic uvs
    for (Face &face: geometry.faces) {
        for (int i = 0; i < 3; i++) {
            Vector3 vert = geometry.vertices[face.vertIndices[i]].normalize();
            double u = 0.5 + atan2(vert.z, vert.x) * twoPiInv;
            double v = 0.5 - asin(vert.y) * piInv;
            geometry.uvs.emplace_back(u, v);
            face.uvIndices[i] = geometry.uvs.size() - 1;
        }
    }

    // Fix the uvs
    for (Face &face: geometry.faces) {
        Vector2 &uv1 = geometry.uvs[face.uvIndices[0]];
        Vector2 &uv2 = geometry.uvs[face.uvIndices[1]];
        Vector2 &uv3 = geometry.uvs[face.uvIndices[2]];

        // Fix the zigzag seam
        if ((uv2 - uv1).cross(uv3 - uv1) < 0) {
            if (uv1.x < 0.25) {
               uv1.x += 1;
            }
            if (uv2.x < 0.25) {
                uv2.x += 1;
            }
            if (uv3.x < 0.25) {
                uv3.x += 1;
            }
        }

        // Fix the uvs for the poles
        if (uv1.y == 0 || uv1.y == 1) {
            uv1.x = (uv2.x + uv3.x) / 2;
        }
        if (uv2.y == 0 || uv2.y == 1) {
            uv2.x = (uv1.x + uv3.x) / 2;

        }
        if (uv3.y == 0 || uv3.y == 1) {
            uv3.x = (uv2.x + uv1.x) / 2;
        }
    }

    return geometry;
}

void IcoSphere::subdivide(Geometry &geometry, int subdivision) {
    std::vector<Vector3> tmpVertices;
    std::vector<Face> tmpFaces;
    unsigned int index;

    for (int i = 1; i <= subdivision; ++i) {
        // copy prev vertex/index arrays and clear
//        tmpVertices = geometry.vertices;
        tmpFaces = geometry.faces;
//        geometry.vertices.clear();
        geometry.faces.clear();


        // perform subdivision for each triangle
        for (int j = 0; j < tmpFaces.size(); j++) {
            TriangleIndices &indices = tmpFaces[j].vertIndices;
            // get 3 vertices of a triangle
            index = geometry.vertices.size();

            GLuint i1 = indices.v1;
            GLuint i2 = indices.v2;
            GLuint i3 = indices.v3;

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

            geometry.faces.emplace_back(TriangleIndices(i1, i4, i6), tmpFaces[j].material);
            geometry.faces.emplace_back(TriangleIndices(i4, i2, i5), tmpFaces[j].material);
            geometry.faces.emplace_back(TriangleIndices(i6, i5, i3), tmpFaces[j].material);
            geometry.faces.emplace_back(TriangleIndices(i4, i5, i6), tmpFaces[j].material);
        }
    }
}

Vector3 IcoSphere::computeHalfVertex(const Vector3 &v1, const Vector3 &v2) {
    Vector3 newV;
    newV.x = v1.x + v2.x;
    newV.y = v1.y + v2.y;
    newV.z = v1.z + v2.z;

    return newV.normalize();
}
