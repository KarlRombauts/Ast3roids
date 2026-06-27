#include "RenderSystem.h"
#include "../OpenGL.h"
#include "../GameModel.h"
#include <Matrix4.h>
#include <Factory/Primatives/PlaneFactory.h>

// Minimal shaders for Task 2. The vertex stage applies the MVP; the fragment
// stage visualises the surface normal as colour so the mesh is visible without
// textures or lighting (those arrive in Tasks 3-4).
static const char *VERT_SRC = R"(
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uMVP;

out vec3 vNormal;
out vec2 vUV;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vNormal = aNormal;
    vUV = aUV;
}
)";

static const char *FRAG_SRC = R"(
in vec3 vNormal;
in vec2 vUV;

out vec4 fragColor;

void main() {
    fragColor = vec4(vNormal * 0.5 + 0.5, 1.0);
}
)";

void RenderSystem::ensureInitialised() {
    if (initialised) {
        return;
    }
    shader.loadFromSource(VERT_SRC, FRAG_SRC);
    testMesh.upload(PlaneFactory::create(nullptr));
    initialised = true;
}

void RenderSystem::update(EntityManager &entities, double dt) {
    ensureInitialised();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("uMVP", Matrix4::identity());
    testMesh.draw();
}
