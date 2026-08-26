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
in vec3 vObjectPos;
in vec3 vObjectNormal;
in float vMeshCrater; // crater displacement the vertices were moved by

// Declared in both stages: same name, same program, one location. Needed here
// to carry the object-space perturbed normal back out to world space.
uniform mat4 uModel;

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

// Procedural surface detail. Off (0) for every material but the asteroid.
uniform int uDetailNormals;
uniform float uDetailScale;    // features per unit of object space
uniform float uDetailStrength; // how hard the perturbation bends the normal
uniform float uDetailRidge;    // 0 = plain fbm, 1 = fully ridged

uniform float uDetailAlbedo;   // 0 = flat colour, 1 = full procedural rock tone

// Albedo, on its own octave stack rather than sharing the normals'. Colour and
// relief want different frequencies: the shape reads best when the fine bands
// dominate, the tone when the coarse ones do, and one scale cannot serve both.
uniform float uAlbedoScale;    // features per unit of object space
uniform float uAlbedoGain;     // amplitude falloff per octave: low = broad patches
uniform float uAlbedoContrast; // how far the tone swings between weathered and fresh
uniform float uCraterTone;     // how hard the baked crater height drives the tone
uniform vec3 uRockTint;        // multiplies the finished rock colour
uniform int uDetailDebug;      // 1 = show the albedo unlit

/**
 * Surface detail as a computed function rather than a stored map.
 *
 * The asteroid's shape is already a pure function of 3D position, so there is
 * nothing to parameterise and nothing to store: no uv seam, no pole pinch, no
 * tangent basis, no texture memory, and no resolution ceiling. The mesh carries
 * the low frequencies - the lumps that change the silhouette and that collision
 * has to agree with - and this carries the high ones the triangles are too
 * coarse to hold.
 */
/**
 * Random gradient per lattice point.
 *
 * highp is guaranteed here (Shader.cpp emits `precision highp float` on ES and
 * 330 core is highp by default), so the sin-based mix is safe from the
 * precision collapse that makes it band on mediump hardware.
 */
vec3 dhash3(vec3 p) {
    p = vec3(dot(p, vec3(127.1, 311.7, 74.7)),
             dot(p, vec3(269.5, 183.3, 246.1)),
             dot(p, vec3(113.5, 271.9, 124.6)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

/**
 * Gradient noise, not value noise.
 *
 * Value noise interpolates a random number per lattice cell, so every cell
 * becomes a rounded lump and the sum reads as a field of repeated blobs no
 * matter how good the hash is or how the octaves are rotated - the motif is the
 * cell itself. Gradient noise stores a random *direction* per lattice point and
 * takes its value from the dot product with the offset, so it is zero at every
 * lattice point and its structure comes from how the directions disagree. That
 * is what makes it read as organic rather than as spots.
 */
float dgnoise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = p - i;
    vec3 u = f * f * (3.0 - 2.0 * f);

    float n000 = dot(dhash3(i + vec3(0.0, 0.0, 0.0)), f - vec3(0.0, 0.0, 0.0));
    float n100 = dot(dhash3(i + vec3(1.0, 0.0, 0.0)), f - vec3(1.0, 0.0, 0.0));
    float n010 = dot(dhash3(i + vec3(0.0, 1.0, 0.0)), f - vec3(0.0, 1.0, 0.0));
    float n110 = dot(dhash3(i + vec3(1.0, 1.0, 0.0)), f - vec3(1.0, 1.0, 0.0));
    float n001 = dot(dhash3(i + vec3(0.0, 0.0, 1.0)), f - vec3(0.0, 0.0, 1.0));
    float n101 = dot(dhash3(i + vec3(1.0, 0.0, 1.0)), f - vec3(1.0, 0.0, 1.0));
    float n011 = dot(dhash3(i + vec3(0.0, 1.0, 1.0)), f - vec3(0.0, 1.0, 1.0));
    float n111 = dot(dhash3(i + vec3(1.0, 1.0, 1.0)), f - vec3(1.0, 1.0, 1.0));

    float x00 = mix(n000, n100, u.x);
    float x10 = mix(n010, n110, u.x);
    float x01 = mix(n001, n101, u.x);
    float x11 = mix(n011, n111, u.x);
    // 3D gradient noise lands in roughly [-0.7, 0.7]; scale to about [-1, 1].
    return mix(mix(x00, x10, u.y), mix(x01, x11, u.y), u.z) * 1.4;
}

/**
 * Rotation applied between octaves.
 *
 * Value noise lives on an axis-aligned cubic lattice, so without this every
 * octave lines its features up with the same three directions and the sum comes
 * out visibly gridded. Turning the domain by an angle that is not a fraction of
 * a right angle means no two octaves share an axis.
 */
const float MAX_TILT = 0.7; // tan of the largest bump tilt, ~35 degrees

const mat3 OCTAVE_ROT = mat3(0.00, 0.80, 0.60,
                            -0.80, 0.36, -0.48,
                            -0.60, -0.48, 0.64);

/**
 * Octave sum, band-limited to the pixel.
 *
 * This is the part a stored texture would get for free and a computed one does
 * not. Hardware picks a mip level from the screen-space derivatives, so a
 * distant surface samples a pre-averaged blur; a computed texel has nothing to
 * average, so every octave finer than the pixel turns into crawling noise as
 * the rock rotates - worse than having no detail at all. `footprint` is how
 * much of the field one pixel covers, and any octave below it is faded out.
 *
 * It is the same rule the mesh uses to gate its own displacement against edge
 * length, moved from the triangle to the pixel.
 */
float detailHeight(vec3 p, float footprint) {
    float sum = 0.0;
    float amp = 1.0;
    float freq = 1.0;
    vec3 q = p;
    for (int i = 0; i < 6; i++) {
        float feature = 1.0 / (freq * uDetailScale);
        float visible = smoothstep(footprint * 1.5, footprint * 4.0, feature);
        // Cheap early out once an octave is finer than the pixel. Everything
        // after it is finer still, so the whole tail is invisible - which is
        // what makes seven octaves affordable: a distant rock evaluates one or
        // two, and only a close-up one pays for all of them.
        if (visible <= 0.001) break;

        float n = dgnoise(q * freq * uDetailScale);
        q = OCTAVE_ROT * q;
        // Ridging, dialled rather than decided. Folding the field about zero
        // turns its zero crossings into creases - but the zero crossings of a
        // smooth field are continuous *curves*, so every one becomes a line
        // running across the surface. Fully ridged reads as worms and veins,
        // which is wrong for regolith; a little adds fracture, and zero is a
        // perfectly good rock. Only the finer octaves are folded, so the big
        // shapes stay smooth whatever this is set to.
        if (i >= 2) {
            n = mix(n, (1.0 - 2.0 * abs(n)) * 0.7, uDetailRidge);
        }
        // Amplitude tied to the octave's own feature width. A fixed amplitude
        // makes slope scale with frequency, so coarse octaves come out nearly
        // flat and fine ones come out as cliffs; tying the two together gives
        // every octave a comparable slope, which is what "self-similar" is
        // supposed to mean and what makes one strength value work at any scale.
        sum += visible * amp * n * feature;
        amp *= 0.55;
        freq *= 2.0;
    }
    // Normalised, so the field lands in about [-1, 1] whatever the octave count
    // is. Without this the amplitude - and so the gradient, and so how hard the
    // normal bends - moves every time the octaves change.
    // uDetailStrength is the one term here that is a judgement call rather than
    // a measurement, so it is the only thing scaling the result.
    return sum * uDetailStrength;
}

/**
 * Bend the normal by the gradient of the detail field.
 *
 * Finite differences along two tangents rather than an analytic gradient: the
 * field is a sum of octaves of value noise, and differencing it costs two more
 * evaluations against writing and maintaining a derivative for every term.
 */
/**
 * Tonal fbm - the same octave machinery detailHeight uses, for colour.
 *
 * Two hand-picked frequencies is what this was, and two frequencies is a blotch
 * pattern: one size of patch with one size of patch inside it, and nothing at
 * any other scale. Rock has tone at every scale, and the reason a full octave
 * stack matters more for colour than for relief is that colour has no lighting
 * to rescue it - a shading normal at least gets a highlight and a terminator out
 * of the light, while a flat albedo just looks painted.
 *
 * Band-limited to the pixel for the same reason the height field is, and
 * normalised by the amplitudes that survived, so the tone does not drift
 * brighter or darker as octaves fade out with distance.
 */
float albedoFbm(vec3 p, float footprint) {
    float sum = 0.0;
    float norm = 0.0;
    float amp = 1.0;
    float freq = 1.0;
    vec3 q = p;
    for (int i = 0; i < 7; i++) {
        float feature = 1.0 / (freq * uAlbedoScale);
        float visible = smoothstep(footprint * 1.5, footprint * 4.0, feature);
        if (visible <= 0.001) break; // finer than a pixel, and so is all the rest

        sum += visible * amp * dgnoise(q * freq * uAlbedoScale);
        norm += visible * amp;
        q = OCTAVE_ROT * q;
        amp *= uAlbedoGain;
        freq *= 2.0;
    }
    return norm > 1e-6 ? sum / norm : 0.0;
}

/**
 * Rock colour.
 *
 * Without this the surface is one flat tone and every bit of variation has to
 * come from the lighting, which is why it reads as a shaded ball rather than as
 * rock. Real asteroid regolith is not uniform: space weathering darkens an
 * exposed surface over time, and anything that digs into it - an impact, a
 * slumping wall - turns up brighter material that has not been out there as
 * long. So tone follows the terrain rather than being sprinkled over it.
 *
 * The crater term is the one that does the work, and it costs nothing. Where the
 * craters are is a question the vertices already answered; vMeshCrater is that
 * answer interpolated, so the tone lands on the geometry exactly, with no field
 * to re-evaluate and no gradient to keep in step. Floors go dark - they are the
 * oldest surface on the rock and the place fine regolith settles - and rims and
 * walls come up bright.
 *
 * Then the fbm above for tone at every other scale, and a shallow occlusion term
 * so a pit sits in its own shadow, which the lighting model has no way to work
 * out for itself.
 */
vec3 rockAlbedo(vec3 p, float footprint) {
    float tone = albedoFbm(p, footprint);
    // Scaled to saturate on a typical crater floor, the same 12x the debug paint
    // used - that view was legible precisely because it clipped.
    float crater = clamp(vMeshCrater * 12.0, -1.0, 1.0);

    vec3 weathered = vec3(0.30, 0.27, 0.25);
    vec3 fresh = vec3(0.80, 0.78, 0.74);

    float freshness = clamp(0.45 + tone * uAlbedoContrast + crater * uCraterTone, 0.0, 1.0);
    vec3 c = mix(weathered, fresh, freshness);

    // Occlusion, not material: a separate multiplier because a crater floor is
    // both older *and* shadowed, and folding the two together makes the pair
    // impossible to dial apart.
    c *= 1.0 + clamp(vMeshCrater * 5.0, -0.30, 0.08);
    c *= uRockTint;
    return mix(vec3(0.72, 0.70, 0.67) * uRockTint, c, uDetailAlbedo);
}

vec3 applyDetail(vec3 n, vec3 objectPos) {
    // Sample the field where it is actually defined: on the undisplaced unit
    // sphere. vObjectPos is the *displaced* vertex, and distortMesh moved it
    // radially - so it sits at a different radius from the one the grain was
    // scaled in, which would make the texture stretch over the lumps and shrink
    // in the hollows. Because the displacement is purely radial, normalize()
    // undoes it exactly.
    vec3 p = normalize(objectPos);

    float footprint = max(length(fwidth(p)), 1e-6);
    // Step with the pixel, not a constant. A fixed epsilon is either larger
    // than the pixel when close (smearing the detail away) or far smaller when
    // distant (differencing pure noise).
    float e = max(footprint, 1e-4);

    // Frame built on the sphere normal, and the offsets pushed back onto the
    // sphere, so both samples stay in the field's own domain.
    //
    // Branchless rather than crossed against a fixed up vector. That version has
    // a singularity wherever p lines up with the vector it crosses against - the
    // tangent goes to zero length, normalising it amplifies whatever noise is
    // left, and the gradient sprays outward from that point. It renders as a
    // starburst of dark streaks converging on one spot of the rock, which drifts
    // around the surface as the body tumbles because the pole is fixed in object
    // space. Swapping to a second up vector near the pole only trades it for a
    // seam at the swap. Duff et al., "Building an Orthonormal Basis, Revisited":
    // stable over the whole sphere, sign flip and all.
    float sgn = p.z >= 0.0 ? 1.0 : -1.0;
    float a = -1.0 / (sgn + p.z);
    float bb = p.x * p.y * a;
    vec3 t = vec3(1.0 + sgn * p.x * p.x * a, sgn * bb, -sgn * p.x);
    vec3 b = vec3(bb, sgn + p.y * p.y * a, -p.y);

    float h = detailHeight(p, footprint);
    float ht = detailHeight(normalize(p + t * e), footprint);
    float hb = detailHeight(normalize(p + b * e), footprint);

    // No uDetailScale divisor and no strength multiplier: the field is a real
    // height in body radii, so its gradient is already the physically correct
    // amount to bend the normal by.
    vec3 grad = ((ht - h) * t + (hb - h) * b) / e;

    // Clamp how far the normal may tilt. normalize(n - grad) turns the normal
    // by atan(|grad|), so a gradient of 1 is 45 degrees and anything much past
    // that swings it over the horizon to face away from the surface - which
    // renders as a raised blister with a lit top, not as the pit that actually
    // produced it. A ridged octave's crease is easily steep enough to do this,
    // so the tilt is capped rather than trusted.
    float steep = length(grad);
    if (steep > MAX_TILT) {
        grad *= MAX_TILT / steep;
    }
    return normalize(n - grad);
}

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

    if (uDetailDebug == 1) {
        // Albedo with the lighting taken away. Tuning a texture under a
        // three-point rig is guesswork: a tone looks wrong on the dark side and
        // right on the lit side, and there is no way to tell which half is the
        // texture's fault.
        vec3 fieldPos = normalize(vObjectPos);
        float fp = max(length(fwidth(fieldPos)), 1e-6);
        fragColor = vec4(rockAlbedo(fieldPos, fp), 1.0);
        return;
    }

    vec3 N = normalize(vWorldNormal);
    if (uDetailNormals == 1) {
        vec3 fieldPos = normalize(vObjectPos);
        float fp = max(length(fwidth(fieldPos)), 1e-6);
        tex *= rockAlbedo(fieldPos, fp);
        // Perturb in object space - where the field is defined and where the
        // rock's tumble cannot move it - then carry the result back out with
        // the same transform the vertex shader used on the mesh normal.
        vec3 detailN = applyDetail(normalize(vObjectNormal), vObjectPos);
        N = normalize(mat3(uModel) * detailN);
    }
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
