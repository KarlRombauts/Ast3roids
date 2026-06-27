// No #version line here: Shader.cpp prepends the right one (#version 330 core on
// desktop, #version 300 es on web), so this same file works for both targets.

const int MAX_LIGHTS = 8;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float attenuation; // constant attenuation: intensity is divided by this
};

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vUV;

uniform sampler2D uTexture;
uniform int uHasTexture;
uniform int uUnlit; // 1 = show the texture fullbright (e.g. the skybox)

uniform sampler2D uSpecMap;
uniform int uHasSpecMap; // per-pixel specular intensity (map_Ks)

uniform vec3 uColor; // flat tint (e.g. the wireframe walls); default white = no-op
uniform int uFog;        // 1 = fade unlit output to black with distance (the grid)
uniform vec2 uFogRange;  // (near, far) distances for the fade

// Sprite-sheet animation: pick a sub-tile of the texture. Defaults (0,0)/(1,1)
// select the whole texture.
uniform vec2 uUvOffset;
uniform vec2 uUvScale;

uniform Light uLights[MAX_LIGHTS];
uniform int uLightCount;
uniform vec3 uViewPos;       // camera position in world space (for specular)
uniform vec3 uGlobalAmbient; // scene-wide ambient (matches old GL_LIGHT_MODEL_AMBIENT)

uniform vec3 uMatAmbient;
uniform vec3 uMatDiffuse;
uniform vec3 uMatSpecular;
uniform vec3 uMatEmission;
uniform float uShininess;

out vec4 fragColor;

void main() {
    vec2 uv = (vUV + uUvOffset) * uUvScale;
    vec4 texSample = (uHasTexture == 1) ? texture(uTexture, uv) : vec4(1.0);
    vec3 tex = texSample.rgb;

    // Unlit surfaces (skybox, walls) show the texture directly, no lighting.
    if (uUnlit == 1) {
        vec3 c = tex * uColor;
        if (uFog == 1) {
            // Fade toward black with distance so the grid recedes into space.
            float dist = length(uViewPos - vWorldPos);
            float fade = clamp((uFogRange.y - dist) / (uFogRange.y - uFogRange.x), 0.0, 1.0);
            c *= fade;
        }
        fragColor = vec4(c, texSample.a);
        return;
    }

    vec3 N = normalize(vWorldNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    vec3 specMap = (uHasSpecMap == 1) ? texture(uSpecMap, uv).rgb : vec3(1.0);

    // The scene light is dim, so scale up specular to let the spec map's shiny
    // areas (window glass, metal) glint. SHININESS_SCALE tightens the highlight
    // (the materials' Ns is only ~10, too broad for glass).
    const float SPECULAR_STRENGTH = 1.8;
    const float SHININESS_SCALE = 6.0;

    // Accumulate the full Phong colour first...
    vec3 lit = uMatEmission + uGlobalAmbient * uMatAmbient;

    for (int i = 0; i < uLightCount; i++) {
        vec3 L = normalize(uLights[i].position - vWorldPos);
        float atten = 1.0 / max(uLights[i].attenuation, 0.0001);

        vec3 ambient = uLights[i].ambient * uMatAmbient;
        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = uLights[i].diffuse * uMatDiffuse * diff;

        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(V, R), 0.0), uShininess * SHININESS_SCALE);
        vec3 specular = uLights[i].specular * uMatSpecular * specMap * spec * SPECULAR_STRENGTH;

        lit += atten * (ambient + diffuse + specular);
    }

    // ...then modulate by the texture (old GL_MODULATE behaviour). Emissive
    // textured sprites become texture*1.0 = the texture itself (a glow).
    fragColor = vec4(tex * lit * uColor, texSample.a);
}
