#ifndef STARFOX_MESH_H
#define STARFOX_MESH_H

#include <OpenGL.h>
#include <Components/Geometry.h>

// A GPU mesh: uploads a Geometry into a VBO/VAO once, then draws it.
// Faces are expanded into independent vertices (interleaved position/normal/uv),
// which sidesteps the OBJ quirk of separate position vs uv indices per corner.
class Mesh {
public:
    Mesh() = default;
    ~Mesh();
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    void upload(const Geometry &geometry);
    void draw() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
};

#endif //STARFOX_MESH_H
