#ifndef STARFOX_GEOMETRY_H
#define STARFOX_GEOMETRY_H

#include <utility>
#include <vector>
#include <Vector3.h>
#include <OpenGL/OpenGL.h>
#include <ecs/Component.h>
#include <Vector2.h>
#include "Helpers/StbImage.h"
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

struct Material {
    Material() :
            name("untitled"),
            ambient{0.5, 0.5, 0.5, 1},
            diffuse{0.5, 0.5, 0.5, 1},
            specular{1, 1, 1, 1},
            emission{0, 0, 0, 1},
            shininess(50),
            textureId(0) {}

    std::string name;
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat emission[4];
    GLfloat shininess;
    unsigned int textureId;

    void setEmission(GLfloat r, GLfloat g, GLfloat b) {
        emission[0] = r;
        emission[1] = g;
        emission[2] = b;
    }

    void setDiffuse(GLfloat r, GLfloat g, GLfloat b) {
        diffuse[0] = r;
        diffuse[1] = g;
        diffuse[2] = b;
    }

    void setAmbient(GLfloat r, GLfloat g, GLfloat b) {
        ambient[0] = r;
        ambient[1] = g;
        ambient[2] = b;
    }

    void setSpecular(GLfloat r, GLfloat g, GLfloat b) {
        specular[0] = r;
        specular[1] = g;
        specular[2] = b;
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
