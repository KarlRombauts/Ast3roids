#ifndef STARFOX_GEOMETRY_H
#define STARFOX_GEOMETRY_H

#include <vector>
#include <Vector3.h>
#include <OpenGL/OpenGL.h>
#include <ecs/Component.h>
#include <Vector2.h>
#include "Helpers/StbImage.h"
#include <iostream>

struct TriangleIndices {
    TriangleIndices(GLuint v1 = 0, GLuint v2 = 0, GLuint v3 = 0)
            : v1(v1), v2(v2), v3(v3) {}

    GLuint v1;
    GLuint v2;
    GLuint v3;

    unsigned int operator[](int index) const {
        switch (index) {
            case 0:
                return v1;
            case 1:
                return v2;
            case 2:
                return v3;
            default:
                return 0;
        }
    }
};

struct Material {
    Material() :
            name("untitled"),
            ambient{0.5, 0.5, 0.5, 1},
            diffuse{0.5, 0.5, 0.5, 1},
            specular{1, 1, 1, 1},
            shininess(50),
            textureId(0) {}

    void setAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
        ambient[0] = r;
        ambient[1] = g;
        ambient[2] = b;
        ambient[3] = a;
    }

    void setDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
        diffuse[0] = r;
        diffuse[1] = g;
        diffuse[2] = b;
        diffuse[3] = a;
    }

    void setSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
        specular[0] = r;
        specular[1] = g;
        specular[2] = b;
        specular[3] = a;
    }

    void setShininess(GLfloat shininess) {
        this->shininess = shininess;
    }

    std::string name;
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;
    unsigned int textureId;
};


struct Face {
    Face(TriangleIndices indices, Material *material)
            : vertIndices(indices),
              uvIndices(0, 0, 0),
              material(material) {}

    Face(TriangleIndices indices, TriangleIndices uvIndices, Material *material)
            : vertIndices(indices),
              uvIndices(uvIndices),
              material(material) {}

    TriangleIndices vertIndices;
    TriangleIndices uvIndices;
    Material *material;
};

struct Geometry : public Component {
    std::vector<Face> faces;
    std::vector<Vector2> uvs;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Material *> materials;
};


#endif //STARFOX_GEOMETRY_H
