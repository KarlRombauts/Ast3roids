#ifndef STARFOX_LINEMESH_H
#define STARFOX_LINEMESH_H

#include <OpenGL.h>
#include <Vector3.h>
#include <vector>

// A GPU mesh of GL_LINES (consecutive point pairs form line segments). Used for
// the arena's wireframe walls, and for the HUD, which re-uploads its geometry
// every frame.
class LineMesh {
public:
    LineMesh() = default;
    ~LineMesh();
    LineMesh(const LineMesh &) = delete;
    LineMesh &operator=(const LineMesh &) = delete;

    // Replaces the mesh's contents. The GL buffers are created on the first
    // call and re-used after that, so a mesh that is rebuilt every frame (the
    // HUD) costs one upload rather than a fresh VAO/VBO pair each time.
    void upload(const std::vector<Vector3> &points);
    void draw() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
};

#endif //STARFOX_LINEMESH_H
