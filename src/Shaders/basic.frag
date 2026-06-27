// No #version line here: Shader.cpp prepends the right one (#version 330 core on
// desktop, #version 300 es on web), so this same file works for both targets.

in vec3 vNormal;
in vec2 vUV;

uniform sampler2D uTexture;
uniform int uHasTexture;

out vec4 fragColor;

void main() {
    if (uHasTexture == 1) {
        fragColor = texture(uTexture, vUV);
    } else {
        // Fallback so untextured meshes are still visible: show the normal as colour.
        fragColor = vec4(vNormal * 0.5 + 0.5, 1.0);
    }
}
