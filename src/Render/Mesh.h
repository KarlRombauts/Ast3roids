#ifndef STARFOX_MESH_H
#define STARFOX_MESH_H

#include <OpenGL.h>
#include <Components/Geometry.h>
#include <vector>
#include <functional>

class Material;

// A GPU mesh: uploads a Geometry into a VBO/VAO once, then draws it.
// Faces are expanded into independent vertices (interleaved position/normal/uv),
// which sidesteps the OBJ quirk of separate position vs uv indices per corner.
// Faces are grouped by material into contiguous sub-meshes so each can be drawn
// with its own texture.
class Mesh {
public:
    struct SubMesh {
        int shapeIndex;          // which Geometry shape these faces belong to
        const Material *material;
        GLint start;
        GLsizei count;
    };

    Mesh() = default;
    ~Mesh();
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    void upload(const Geometry &geometry);

    // Draws the mesh one (shape, material) group at a time, calling setup(sub)
    // before each group so the caller can apply that shape's transform and bind
    // that material's texture.
    void draw(const std::function<void(const SubMesh &)> &setup) const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    std::vector<SubMesh> submeshes;
};

#endif //STARFOX_MESH_H
