#ifndef STARFOX_MATERIAL_H
#define STARFOX_MATERIAL_H

#include <ecs/Component.h>
#include <OpenGL.h>
#include <string>

struct Material : public Component {
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

#endif //STARFOX_MATERIAL_H
