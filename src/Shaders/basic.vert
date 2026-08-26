// No #version line here: Shader.cpp prepends the right one (#version 330 core on
// desktop, #version 300 es on web), so this same file works for both targets.

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
// The crater displacement this vertex was moved by. The fragment shader tones
// the rock with it rather than re-deriving where the craters are.
layout(location = 3) in float aMeshCrater;

uniform mat4 uModel;     // object -> world
uniform mat4 uViewProj;  // world -> clip (projection * view)

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec2 vUV;
// Object space, not world: the asteroid's detail field is a function of
// position on the un-transformed body, so it has to be sampled somewhere the
// rock's own spin does not move it. Sampling in world space would make the
// surface texture swim as the asteroid tumbles.
out vec3 vObjectPos;
out vec3 vObjectNormal;
out float vMeshCrater;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    // Transform the normal by the model's rotation/scale (mat3 drops translation).
    // Exact only for uniform scale; we re-normalise in the fragment shader.
    vWorldNormal = mat3(uModel) * aNormal;

    vUV = aUV;
    vObjectPos = aPos;
    vObjectNormal = aNormal;
    vMeshCrater = aMeshCrater;
    gl_Position = uViewProj * worldPos;
}
