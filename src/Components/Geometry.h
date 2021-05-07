#ifndef STARFOX_GEOMETRY_H
#define STARFOX_GEOMETRY_H

#include <vector>
#include <Vector3.h>
#include <OpenGL/OpenGL.h>
#include <ecs/Component.h>

struct TriangleIndices {
    TriangleIndices(GLuint v1 = 0, GLuint v2 = 0, GLuint v3 = 0)
            : v1(v1), v2(v2), v3(v3) {}

    GLuint v1;
    GLuint v2;
    GLuint v3;

};

struct QuadIndices {
    QuadIndices(GLuint v1 = 0, GLuint v2 = 0, GLuint v3 = 0, GLuint v4 = 0)
            : v1(v1), v2(v2), v3(v3), v4(v4) {}

    GLuint v1;
    GLuint v2;
    GLuint v3;
    GLuint v4;
};

struct Geometry : public Component {
    std::vector<TriangleIndices> triangles;
    std::vector<QuadIndices> quads;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
};


#endif //STARFOX_GEOMETRY_H
