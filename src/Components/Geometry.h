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
    std::vector<Face> faces;
    std::vector<Vector2> uvs;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Material *> materials;
    std::vector<Shape> shapes;
};


#endif //STARFOX_GEOMETRY_H
