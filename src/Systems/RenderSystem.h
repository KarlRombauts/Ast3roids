#ifndef UNTITLED_RENDERSYSTEM_H
#define UNTITLED_RENDERSYSTEM_H

#include "System.h"
#include "../Render/Shader.h"
#include "../Render/Mesh.h"
#include "../Matrix4.h"

// Task 3: render every opaque entity that has a Geometry through the shader
// pipeline, using the real camera (view) and perspective (projection).
class RenderSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;

    /**
     * Scene-wide ambient - the shader's uGlobalAmbient. A variable rather than
     * a literal only so the standalone asteroid lab can put it on a slider; the
     * value here is the one the renderer has always used, and nothing in the
     * game writes to it, so StarFox_run is unaffected.
     */
    static float globalAmbient[3];

private:
    void ensureInitialised();

    // Draws one entity (assumes the shared shader/camera/light uniforms are set).
    void drawEntity(Entity *entity);

    // Draws the skybox centred on the camera (rotation-only view), unlit.
    void drawSkybox(EntityManager &entities);

    // Draws additive glow billboards at the ship's engines, sized by thrust.
    void drawEngineGlow(Entity *ship);

    // Draws the arena's wireframe walls.
    void drawWalls(EntityManager &entities);

    bool initialised = false;
    Shader shader;
    Mesh glowMesh;          // a unit quad reused for every engine-glow billboard
    GLuint whiteTexture = 0; // 1x1 white, bound to map units that have no texture
};

#endif //UNTITLED_RENDERSYSTEM_H
