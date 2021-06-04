#ifndef STARFOX_LIGHT_H
#define STARFOX_LIGHT_H

#include <ecs/Component.h>
#include <OpenGL.h>

struct Light : public Component {
    Light(GLfloat r, GLfloat g, GLfloat b) :
            ambient{0, 0, 0, 1},
            diffuse{r, g, b, 1},
            specular{r, g, b, 1} {}

    Light() :
            ambient{0, 0, 0, 1},
            diffuse{1, 1, 1, 1},
            specular{1, 1, 1, 1} {}

    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    double attenuation = 1;

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


#endif //STARFOX_LIGHT_H
