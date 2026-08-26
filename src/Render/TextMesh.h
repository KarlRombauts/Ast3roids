#ifndef STARFOX_TEXTMESH_H
#define STARFOX_TEXTMESH_H

#include <OpenGL.h>
#include <vector>

/**
 * A dynamic GL_TRIANGLES mesh of position + UV vertices, rebuilt every frame.
 * The HUD's text is quads out of a glyph atlas, which is the one thing on
 * screen the line renderer cannot draw.
 *
 * Vertices are 5 floats: x, y, z, u, v. Only attributes 0 (position) and 2
 * (UV) are enabled - the shader's unlit branch reads nothing else - so this
 * feeds the same program the rest of the game draws through.
 */
class TextMesh {
public:
    TextMesh() = default;
    ~TextMesh();
    TextMesh(const TextMesh &) = delete;
    TextMesh &operator=(const TextMesh &) = delete;

    void upload(const std::vector<float> &vertices);
    void draw() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
};

#endif //STARFOX_TEXTMESH_H
