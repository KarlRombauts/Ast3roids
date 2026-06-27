#ifndef UNTITLED_RENDERSYSTEM_H
#define UNTITLED_RENDERSYSTEM_H

#include "System.h"
#include "../Render/Shader.h"
#include "../Matrix4.h"

// Task 3: render every opaque entity that has a Geometry through the shader
// pipeline, using the real camera (view) and perspective (projection).
class RenderSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;

private:
    void ensureInitialised();

    // The camera's view matrix = inverse of the camera's world transform.
    Matrix4 viewMatrix() const;

    bool initialised = false;
    Shader shader;
};

#endif //UNTITLED_RENDERSYSTEM_H
