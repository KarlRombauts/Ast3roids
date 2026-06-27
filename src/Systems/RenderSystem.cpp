#include "RenderSystem.h"
#include "../OpenGL.h"
#include "../GameModel.h"
#include "../Globals.h"
#include "../MaterialLibrary.h"
#include <Factory/Primatives/PlaneFactory.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Components/Scale.h>
#include <Components/Material.h>
#include <Components/RenderMesh.h>
#include <Components/Light.h>
#include <Components/Transparency.h>
#include <Components/AnimatedTexture.h>
#include <Components/Skybox.h>
#include <Components/SpaceShip.h>
#include <Components/Wall.h>
#include <Components/Plane.h>
#include <Components/WallMesh.h>
#include <Helpers.h>
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
    glowMesh.upload(PlaneFactory::create(nullptr));

    // 1x1 white texture used as a fallback so sampler units are never bound to
    // an empty texture (which triggers driver warnings / undefined sampling).
    unsigned char white[4] = {255, 255, 255, 255};
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Seed every sampler unit with a valid texture up front.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);

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
    shader.setInt("uTexture", 0);  // diffuse on texture unit 0
    shader.setInt("uSpecMap", 1);  // spec map on texture unit 1
    shader.setInt("uUnlit", 0);
    shader.setVec3("uColor", 1.0f, 1.0f, 1.0f);
    shader.setInt("uFog", 0);

    // Seed the spec-map unit with a valid texture so the skybox/glow draws (which
    // never bind it) don't leave the sampler pointed at an empty unit.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glActiveTexture(GL_TEXTURE0);
    shader.setVec3("uViewPos", camPos.x, camPos.y, camPos.z);
    shader.setVec3("uGlobalAmbient", 0.32f, 0.32f, 0.34f);

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

    // Background first (it sets its own rotation-only view), then restore the
    // full view-projection for the world.
    drawSkybox(entities);
    shader.setMat4("uViewProj", viewProj);

    // Opaque pass: write depth as usual.
    for (Entity *entity : entities.getEntitiesWith<Position, Rotation, Scale, Geometry>()) {
        if (entity->has<Transparency>()) {
            continue;
        }
        drawEntity(entity);
    }

    drawWalls(entities);

    // Transparent pass: still depth-test against the opaque scene, but don't
    // write depth, so overlapping sprites (glow particles, explosions) blend
    // instead of punching holes in each other.
    glDepthMask(GL_FALSE);
    for (Entity *entity : entities.getEntitiesWith<Position, Rotation, Scale, Geometry, Transparency>()) {
        drawEntity(entity);
    }

    // Engine glow on top, additive (alpha-weighted) so it brightens whatever is
    // behind it like a real light bloom.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (Entity *ship : entities.getEntitiesWith<SpaceShip, Position, Rotation, Scale>()) {
        drawEngineGlow(ship);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_TRUE);
}

void RenderSystem::drawEntity(Entity *entity) {
    Vector3 &position = entity->get<Position>()->position;
    Vector3 &scale = entity->get<Scale>()->scale;
    Quaternion &rotation = entity->get<Rotation>()->rotation;

    // Model matrix places the object in the world. Read right-to-left, a
    // vertex is rotated, then scaled, then translated into position.
    Matrix4 entityModel = Matrix4::translation(position)
                          * Matrix4::scale(scale)
                          * Matrix4::fromQuaternion(rotation);

    Geometry *geometry = entity->get<Geometry>();

    // Sprite-sheet animation (e.g. the explosion) shifts the UVs to one tile;
    // everything else uses the whole texture.
    if (entity->has<AnimatedTexture>()) {
        AnimatedTexture *anim = entity->get<AnimatedTexture>();
        shader.setVec2("uUvOffset", (float) anim->colOffset, (float) anim->rowOffset);
        shader.setVec2("uUvScale", 1.0f / (float) anim->cols, 1.0f / (float) anim->rows);
    } else {
        shader.setVec2("uUvOffset", 0.0f, 0.0f);
        shader.setVec2("uUvScale", 1.0f, 1.0f);
    }

    // Upload the mesh to the GPU the first time we see this entity; it is
    // owned by the entity and freed when the entity is destroyed.
    if (!entity->has<RenderMesh>()) {
        entity->assign<RenderMesh>();
        entity->get<RenderMesh>()->mesh.upload(*geometry);
    }

    // Thrusting boosts the ship's emission. Only the engine-glow material has
    // non-zero emission, so this lights up just the engines.
    float emissionScale = 1.0f;
    if (entity->has<SpaceShip>()) {
        emissionScale = 1.0f + (float) entity->get<SpaceShip>()->thrust * 1.5f;
    }

    // Draw each (shape, material) group. The shape transform is read fresh each
    // frame, so animating a shape (e.g. the X-Wing wings) moves it.
    entity->get<RenderMesh>()->mesh.draw([&](const Mesh::SubMesh &sub) {
        Matrix4 model = entityModel;
        if (sub.shapeIndex >= 0 && sub.shapeIndex < (int) geometry->shapes.size()) {
            const Shape &shape = geometry->shapes[sub.shapeIndex];
            model = entityModel
                    * Matrix4::translation(shape.position)
                    * Matrix4::scale(shape.scale)
                    * Matrix4::fromQuaternion(shape.rotation);
        }
        shader.setMat4("uModel", model);

        const Material &m = (sub.material != nullptr) ? *sub.material : DEFAULT_MATERIAL;
        shader.setVec3("uMatAmbient", m.ambient[0], m.ambient[1], m.ambient[2]);
        shader.setVec3("uMatDiffuse", m.diffuse[0], m.diffuse[1], m.diffuse[2]);
        shader.setVec3("uMatSpecular", m.specular[0], m.specular[1], m.specular[2]);
        shader.setVec3("uMatEmission",
                       m.emission[0] * emissionScale,
                       m.emission[1] * emissionScale,
                       m.emission[2] * emissionScale);
        shader.setFloat("uShininess", m.shininess);

        shader.setInt("uHasTexture", m.textureId != 0 ? 1 : 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m.textureId != 0 ? m.textureId : whiteTexture);

        shader.setInt("uHasSpecMap", m.specTextureId != 0 ? 1 : 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m.specTextureId != 0 ? m.specTextureId : whiteTexture);
    });
}

void RenderSystem::drawSkybox(EntityManager &entities) {
    std::vector<Entity *> skyboxes = entities.getEntitiesWith<Skybox, Geometry>();
    if (skyboxes.empty()) {
        return;
    }
    Entity *skybox = skyboxes[0];

    // Rotation-only view: the skybox follows the camera's orientation but never
    // its position, so it stays centred on the camera and looks infinitely far.
    Quaternion &camRot = gameModel.activeCamera->get<Rotation>()->rotation;
    shader.setMat4("uViewProj", gameModel.projection * Matrix4::fromQuaternion(camRot.conjugate()));
    shader.setMat4("uModel", Matrix4::identity());
    shader.setVec2("uUvOffset", 0.0f, 0.0f);
    shader.setVec2("uUvScale", 1.0f, 1.0f);
    shader.setInt("uUnlit", 1);

    // Drawn first with depth testing off, so everything else renders over it.
    glDisable(GL_DEPTH_TEST);

    if (!skybox->has<RenderMesh>()) {
        skybox->assign<RenderMesh>();
        skybox->get<RenderMesh>()->mesh.upload(*skybox->get<Geometry>());
    }
    skybox->get<RenderMesh>()->mesh.draw([this](const Mesh::SubMesh &sub) {
        GLuint textureId = (sub.material != nullptr) ? sub.material->textureId : 0;
        shader.setInt("uHasTexture", textureId != 0 ? 1 : 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
    });

    glEnable(GL_DEPTH_TEST);
    shader.setInt("uUnlit", 0);
}

void RenderSystem::drawEngineGlow(Entity *ship) {
    double thrust = ship->get<SpaceShip>()->thrust;
    if (thrust < 0.01) {
        return;
    }

    Vector3 &position = ship->get<Position>()->position;
    Vector3 &scale = ship->get<Scale>()->scale;
    Quaternion &rotation = ship->get<Rotation>()->rotation;
    Matrix4 shipModel = Matrix4::translation(position)
                        * Matrix4::scale(scale)
                        * Matrix4::fromQuaternion(rotation);

    Quaternion &camRot = gameModel.activeCamera->get<Rotation>()->rotation;
    Geometry *geometry = ship->get<Geometry>();

    // Engine exhaust positions in model-local space, each tagged with the wing
    // shape it belongs to, so the glow follows the wing as it opens/closes.
    struct EngineGlow {
        int shape;
        Vector3 offset;
    };
    static const EngineGlow engines[4] = {
            {0, Vector3(-3.861, -1.911, 8.363)},
            {0, Vector3(3.860, 1.911, 8.363)},
            {2, Vector3(-3.861, 1.911, 8.363)},
            {2, Vector3(3.860, -1.911, 8.363)},
    };

    shader.setInt("uUnlit", 1);
    shader.setVec2("uUvOffset", 0.0f, 0.0f);
    shader.setVec2("uUvScale", 1.0f, 1.0f);
    GLuint glowTexture = materialLibrary.GLOW_PARTICLE->textureId;
    shader.setInt("uHasTexture", glowTexture != 0 ? 1 : 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glowTexture);

    float size = (float) thrust * 1.5f;
    for (const EngineGlow &engine : engines) {
        // Apply the wing's shape transform so the glow tracks the open wings.
        Matrix4 engineSpace = shipModel;
        if (engine.shape >= 0 && engine.shape < (int) geometry->shapes.size()) {
            const Shape &shape = geometry->shapes[engine.shape];
            engineSpace = shipModel
                          * Matrix4::translation(shape.position)
                          * Matrix4::scale(shape.scale)
                          * Matrix4::fromQuaternion(shape.rotation);
        }
        // Nudge the glow back along the exhaust (+z) so the flat billboard sits
        // just behind the nacelle opening instead of clipping into it.
        Vector3 pushed(engine.offset.x, engine.offset.y, engine.offset.z + 0.4);
        Vector3 world = engineSpace.transformPoint(pushed);

        // Billboard the glow quad to face the camera.
        Matrix4 model = Matrix4::translation(world)
                        * Matrix4::fromQuaternion(camRot)
                        * Matrix4::scale(Vector3(size, size, size));
        shader.setMat4("uModel", model);
        glowMesh.draw([](const Mesh::SubMesh &) {});
    }

    shader.setInt("uUnlit", 0);
}

// Builds the wireframe grid (vertical + horizontal lines) spanning a plane's corners.
static std::vector<Vector3> buildGridLines(const Plane &plane) {
    const int numLines = 10;
    std::vector<Vector3> points;
    for (int i = 0; i <= numLines; i++) {
        double t = (double) i / numLines;
        points.push_back(Vector3::lerp(plane.bottomLeft, plane.bottomRight, t));
        points.push_back(Vector3::lerp(plane.topLeft, plane.topRight, t));
    }
    for (int i = 0; i <= numLines; i++) {
        double t = (double) i / numLines;
        points.push_back(Vector3::lerp(plane.bottomLeft, plane.topLeft, t));
        points.push_back(Vector3::lerp(plane.bottomRight, plane.topRight, t));
    }
    return points;
}

void RenderSystem::drawWalls(EntityManager &entities) {
    std::vector<Entity *> walls = entities.getEntitiesWith<Wall, Plane, Position, Rotation, Scale>();
    if (walls.empty()) {
        return;
    }

    // Unlit, untextured wireframe. The colour is the wall's material emission,
    // which WarningSystem drives: faint white normally, red as the ship nears.
    shader.setInt("uUnlit", 1);
    shader.setInt("uHasTexture", 0);
    shader.setVec2("uUvOffset", 0.0f, 0.0f);
    shader.setVec2("uUvScale", 1.0f, 1.0f);
    shader.setInt("uFog", 1);
    shader.setVec2("uFogRange", 40.0f, 250.0f);

    for (Entity *wall : walls) {
        if (!wall->has<WallMesh>()) {
            wall->assign<WallMesh>();
            wall->get<WallMesh>()->mesh.upload(buildGridLines(*wall->get<Plane>()));
        }

        Material *material = wall->get<Material>();
        shader.setVec3("uColor", material->emission[0], material->emission[1], material->emission[2]);

        Vector3 &position = wall->get<Position>()->position;
        Vector3 &scale = wall->get<Scale>()->scale;
        Quaternion &rotation = wall->get<Rotation>()->rotation;
        shader.setMat4("uModel", Matrix4::translation(position)
                                 * Matrix4::scale(scale)
                                 * Matrix4::fromQuaternion(rotation));
        wall->get<WallMesh>()->mesh.draw();
    }

    shader.setVec3("uColor", 1.0f, 1.0f, 1.0f);
    shader.setInt("uUnlit", 0);
}
