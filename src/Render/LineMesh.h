#ifndef STARFOX_LINEMESH_H
#define STARFOX_LINEMESH_H

#include <OpenGL.h>
#include <Vector3.h>
#include <vector>

// A GPU mesh of GL_LINES (consecutive point pairs form line segments). Used for
// the arena's wireframe walls.
class LineMesh {
public:
    LineMesh() = default;
    ~LineMesh();
    LineMesh(const LineMesh &) = delete;
    LineMesh &operator=(const LineMesh &) = delete;

    void upload(const std::vector<Vector3> &points);
    void draw() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
};

#endif //STARFOX_LINEMESH_H
