#ifndef UNTITLED_RENDERSYSTEM_H
#define UNTITLED_RENDERSYSTEM_H

#include "System.h"
#include "../Render/Shader.h"
#include "../Render/Mesh.h"

// Task 2: minimal shader-based renderer. Draws a single test mesh to prove the
// GLES3/core pipeline (Shader + VBO/VAO + Matrix4 MVP). Real entity/scene
// rendering is rebuilt on top of this in Tasks 3-6.
class RenderSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;

private:
    void ensureInitialised();

    bool initialised = false;
    Shader shader;
    Mesh testMesh;
};

#endif //UNTITLED_RENDERSYSTEM_H
