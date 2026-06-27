#include "RenderSystem.h"
#include "../OpenGL.h"
#include "../GameModel.h"
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Components/Scale.h>
#include <Components/Material.h>
#include <Components/RenderMesh.h>
#include <Components/Light.h>
#include <algorithm>
#include <string>

static const int MAX_LIGHTS = 8;

// Used when a face has no material assigned.
static const Material DEFAULT_MATERIAL;

void RenderSystem::ensureInitialised() {
    if (initialised) {
        return;
    }
    shader.loadFromFiles("Shaders/basic.vert", "Shaders/basic.frag");
    initialised = true;
}

Matrix4 RenderSystem::viewMatrix() const {
    Vector3 &pos = gameModel.activeCamera->get<Position>()->position;
    Quaternion &rot = gameModel.activeCamera->get<Rotation>()->rotation;

    // Looking "from" the camera means transforming the world by the inverse of
    // the camera's transform: undo its rotation (conjugate) then undo its
    // translation. Right-to-left, the translation is applied first.
    return Matrix4::fromQuaternion(rot.conjugate())
           * Matrix4::translation(Vector3(-pos.x, -pos.y, -pos.z));
}

void RenderSystem::update(EntityManager &entities, double dt) {
    ensureInitialised();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // No camera yet (e.g. the start menu) -> nothing in the world to draw.
    if (gameModel.activeCamera == nullptr) {
        return;
    }

    Matrix4 view = viewMatrix();
    Matrix4 viewProj = gameModel.projection * view;
    Vector3 &camPos = gameModel.activeCamera->get<Position>()->position;

    shader.use();
    shader.setInt("uTexture", 0); // sampler reads from texture unit 0
    shader.setMat4("uViewProj", viewProj);
    shader.setVec3("uViewPos", camPos.x, camPos.y, camPos.z);
    shader.setVec3("uGlobalAmbient", 0.2f, 0.2f, 0.2f);

    // Upload each light's data once per frame (positions/colours/attenuation).
    std::vector<Entity *> lights = entities.getEntitiesWith<Light, Position>();
    int lightCount = std::min((int) lights.size(), MAX_LIGHTS);
    shader.setInt("uLightCount", lightCount);
    for (int i = 0; i < lightCount; i++) {
        Light *light = lights[i]->get<Light>();
        Vector3 &lp = lights[i]->get<Position>()->position;
        std::string base = "uLights[" + std::to_string(i) + "].";
        shader.setVec3(base + "position", lp.x, lp.y, lp.z);
        shader.setVec3(base + "ambient", light->ambient[0], light->ambient[1], light->ambient[2]);
        shader.setVec3(base + "diffuse", light->diffuse[0], light->diffuse[1], light->diffuse[2]);
        shader.setVec3(base + "specular", light->specular[0], light->specular[1], light->specular[2]);
        shader.setFloat(base + "attenuation", (float) light->attenuation);
    }

    for (Entity *entity : entities.getEntitiesWith<Position, Rotation, Scale, Geometry>()) {
        Vector3 &position = entity->get<Position>()->position;
        Vector3 &scale = entity->get<Scale>()->scale;
        Quaternion &rotation = entity->get<Rotation>()->rotation;

        // Model matrix places the object in the world. Read right-to-left, a
        // vertex is rotated, then scaled, then translated into position.
        Matrix4 model = Matrix4::translation(position)
                        * Matrix4::scale(scale)
                        * Matrix4::fromQuaternion(rotation);

        shader.setMat4("uModel", model);

        // Upload the mesh to the GPU the first time we see this entity; it is
        // owned by the entity and freed when the entity is destroyed.
        if (!entity->has<RenderMesh>()) {
            entity->assign<RenderMesh>();
            entity->get<RenderMesh>()->mesh.upload(*entity->get<Geometry>());
        }

        // Draw each material group with its texture and lighting properties.
        entity->get<RenderMesh>()->mesh.draw([this](const Material *material) {
            const Material &m = (material != nullptr) ? *material : DEFAULT_MATERIAL;

            shader.setVec3("uMatAmbient", m.ambient[0], m.ambient[1], m.ambient[2]);
            shader.setVec3("uMatDiffuse", m.diffuse[0], m.diffuse[1], m.diffuse[2]);
            shader.setVec3("uMatSpecular", m.specular[0], m.specular[1], m.specular[2]);
            shader.setVec3("uMatEmission", m.emission[0], m.emission[1], m.emission[2]);
            shader.setFloat("uShininess", m.shininess);

            shader.setInt("uHasTexture", m.textureId != 0 ? 1 : 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m.textureId);
        });
    }
}
