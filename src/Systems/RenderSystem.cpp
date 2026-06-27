#include "RenderSystem.h"
#include "../OpenGL.h"
#include "../GameModel.h"
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Components/Scale.h>
#include <Components/Material.h>
#include <Components/RenderMesh.h>

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

    shader.use();
    shader.setInt("uTexture", 0); // sampler reads from texture unit 0

    for (Entity *entity : entities.getEntitiesWith<Position, Rotation, Scale, Geometry>()) {
        Vector3 &position = entity->get<Position>()->position;
        Vector3 &scale = entity->get<Scale>()->scale;
        Quaternion &rotation = entity->get<Rotation>()->rotation;

        // Model matrix places the object in the world. Read right-to-left, a
        // vertex is rotated, then scaled, then translated into position.
        Matrix4 model = Matrix4::translation(position)
                        * Matrix4::scale(scale)
                        * Matrix4::fromQuaternion(rotation);

        shader.setMat4("uMVP", gameModel.projection * view * model);

        Geometry *geometry = entity->get<Geometry>();
        GLuint textureId = 0;
        if (!geometry->materials.empty() && geometry->materials[0] != nullptr) {
            textureId = geometry->materials[0]->textureId;
        }
        shader.setInt("uHasTexture", textureId != 0 ? 1 : 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Upload the mesh to the GPU the first time we see this entity; it is
        // owned by the entity and freed when the entity is destroyed.
        if (!entity->has<RenderMesh>()) {
            entity->assign<RenderMesh>();
            entity->get<RenderMesh>()->mesh.upload(*geometry);
        }
        entity->get<RenderMesh>()->mesh.draw();
    }
}
