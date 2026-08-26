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
    unsigned int specTextureId = 0;   // map_Ks: per-pixel specular intensity

    /**
     * Procedural surface detail, computed in the fragment shader rather than
     * sampled from a normal map. Off for every material that does not opt in.
     *
     * detailScale is features per unit of object space; detailStrength is how
     * hard the field's gradient bends the shading normal.
     */
    bool detailNormals = false;
    float detailScale = 14.0f;
    float detailStrength = 0.06f;
    float detailRidge = 0.0f;

    float detailAlbedo = 1.0f;

    /**
     * Rock tone. Its own octave stack rather than the normals', because colour
     * and relief want different frequencies, and driven where it matters by the
     * crater height the mesh baked - see basic.frag's rockAlbedo.
     */
    float albedoScale = 5.0f;
    float albedoGain = 0.75f;
    float albedoContrast = 0.85f;
    float craterTone = 0.45f;

    /**
     * Multiplies the finished rock colour. The weathered/fresh pair in the
     * shader is one warm stone; asteroids are not one colour - carbonaceous
     * bodies are near-black, stony ones tan, metallic ones a cool grey - and
     * that difference reads from further away than any amount of relief.
     */
    float rockTint[3] = {1.0f, 1.0f, 1.0f};
    int detailDebug = 0; // 1 = show the albedo unlit
    unsigned int normalTextureId = 0; // map_Bump: normal/height map (Stage 2)

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
