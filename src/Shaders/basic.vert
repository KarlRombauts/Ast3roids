// No #version line here: Shader.cpp prepends the right one (#version 330 core on
// desktop, #version 300 es on web), so this same file works for both targets.

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
