#ifndef STARFOX_GEOMETRY_H
#define STARFOX_GEOMETRY_H

#include <utility>
#include <vector>
#include <Vector3.h>
#include <OpenGL.h>
#include <ecs/Component.h>
#include <Vector2.h>
#include "Helpers/StbImage.h"
#include "Material.h"
#include <iostream>
#include <Quaternion.h>

struct TriangleIndices {
    TriangleIndices(GLuint v1 = 0, GLuint v2 = 0, GLuint v3 = 0)
            : v1(v1), v2(v2), v3(v3) {}

    GLuint v1;
    GLuint v2;
    GLuint v3;

    GLuint & operator[](int index) const {
        return ((GLuint *) this)[index];
    }
};


struct Face {
    Face(TriangleIndices indices, Material *material, int shapeIndex = 0)
            : vertIndices(indices),
              uvIndices(0, 0, 0),
              material(material),
              shapeIndex(shapeIndex) {}

    Face(TriangleIndices indices, TriangleIndices uvIndices, Material *material, int shapeIndex = 0)
            : vertIndices(indices),
              uvIndices(uvIndices),
              material(material),
              shapeIndex(shapeIndex) {}

    TriangleIndices vertIndices;
    TriangleIndices uvIndices;
    Material *material;
    int shapeIndex;

    /** Flat-shading normal for this triangle. Used only when Geometry::flatShaded. */
    Vector3 normal = Vector3(0, 0, 0);

};

struct Shape {
    Shape(std::string name) :
        name(std::move(name)),
        position(Vector3(0, 0, 0)),
        scale(Vector3(1, 1, 1)),
        rotation(Quaternion()) {}

    std::string name;
    Vector3 position;
    Vector3 scale;
    Quaternion rotation;
};

struct Geometry : public Component {
    /**
     * Shade from Face::normal rather than from the per-vertex normals.
     *
     * Faceting is usually done by splitting every triangle into its own three
     * vertices so each can hold the face normal, which triples the vertex,
     * normal and uv arrays. That is wasted here: Mesh::upload already walks
     * faces and writes three vertices per triangle into a non-indexed buffer,
     * so the split only ever existed to make the normal lookup land on a
     * per-face value. Storing that value on the face instead gets identical
     * output from the shared vertices.
     */
    bool flatShaded = false;

    std::vector<Face> faces;
    std::vector<Vector2> uvs;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;

    /**
     * Per-vertex crater displacement, parallel to `vertices`. Empty for
     * everything that is not an asteroid.
     *
     * Colour, not shape. Where a crater is, is a question the mesh has already
     * answered - so the shader asks it rather than recomputing the field, which
     * costs one interpolated float against a 27-cell lattice search per pixel
     * and is exactly aligned with the geometry instead of approximately.
     */
    std::vector<float> craterHeights;

    std::vector<Material *> materials;
    std::vector<Shape> shapes;
};


#endif //STARFOX_GEOMETRY_H
