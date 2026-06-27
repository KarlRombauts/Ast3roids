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

    // Draws one entity (assumes the shared shader/camera/light uniforms are set).
    void drawEntity(Entity *entity);

    // Draws the skybox centred on the camera (rotation-only view), unlit.
    void drawSkybox(EntityManager &entities);

    bool initialised = false;
    Shader shader;
};

#endif //UNTITLED_RENDERSYSTEM_H
