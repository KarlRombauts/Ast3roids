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
    vec3 tex = (uHasTexture == 1) ? texture(uTexture, vUV).rgb : vec3(1.0);
    vec3 N = normalize(vWorldNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    // Accumulate the full Phong colour first...
    vec3 lit = uMatEmission + uGlobalAmbient * uMatAmbient;

    for (int i = 0; i < uLightCount; i++) {
        vec3 L = normalize(uLights[i].position - vWorldPos);
        float atten = 1.0 / max(uLights[i].attenuation, 0.0001);

        vec3 ambient = uLights[i].ambient * uMatAmbient;
        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = uLights[i].diffuse * uMatDiffuse * diff;

        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(V, R), 0.0), uShininess);
        vec3 specular = uLights[i].specular * uMatSpecular * spec;

        lit += atten * (ambient + diffuse + specular);
    }

    // ...then modulate by the texture (old GL_MODULATE behaviour). Emissive
    // textured sprites become texture*1.0 = the texture itself (a glow).
    fragColor = vec4(tex * lit, 1.0);
}
