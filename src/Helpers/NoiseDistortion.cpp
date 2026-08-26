#include "NoiseDistortion.h"
#include "Helpers/PerlinNoise.hpp"
#include <algorithm>
#include <cmath>

namespace {
    /**
     * A bare float triple for the crater code. The crater field is a straight
     * port of a GLSL vertex shader, and its hash is chaotic enough that the
     * extra mantissa of Vector3's doubles would give a different (equally valid,
     * but not the approved) scatter of impact sites. Kept in float so the
     * arithmetic matches the shader the shapes were approved in.
     */
    struct Vec3f {
        float x, y, z;
    };

    float fract(float v) {
        return v - std::floor(v);
    }

    float mix(float a, float b, float t) {
        return a + (b - a) * t;
    }

    float smoothstep(float edge0, float edge1, float x) {
        if (edge0 == edge1) {
            return x < edge0 ? 0.0f : 1.0f;
        }
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    /**
     * A cheap arithmetic hash for the crater cells. Deliberately not the Perlin
     * permutation table: a cell needs three values, and there are 27 cells per
     * vertex. Nothing here has to match the Perlin field.
     */
    Vec3f hash33(Vec3f p) {
        p = {fract(p.x * 0.1031f), fract(p.y * 0.1030f), fract(p.z * 0.0973f)};
        // dot(p, p.yxz + 33.33)
        float d = p.x * (p.y + 33.33f) + p.y * (p.x + 33.33f) + p.z * (p.z + 33.33f);
        p = {p.x + d, p.y + d, p.z + d};
        // fract((p.xxy + p.yxx) * p.zyx)
        return {fract((p.x + p.y) * p.z),
                fract((p.x + p.x) * p.y),
                fract((p.y + p.x) * p.x)};
    }

    /**
     * Smooth value noise off the same cheap hash, for roughening the craters.
     * Trilinear over the eight cell corners - eight hashes, against the fourteen
     * table lookups a Perlin octave would cost, and nothing here has to match the
     * Perlin field so the cheaper one is free to use.
     */
    float vnoise(Vec3f p) {
        Vec3f i = {std::floor(p.x), std::floor(p.y), std::floor(p.z)};
        Vec3f f = {p.x - i.x, p.y - i.y, p.z - i.z};
        f = {f.x * f.x * (3.0f - 2.0f * f.x),
             f.y * f.y * (3.0f - 2.0f * f.y),
             f.z * f.z * (3.0f - 2.0f * f.z)};

        float n000 = hash33({i.x, i.y, i.z}).x;
        float n100 = hash33({i.x + 1.0f, i.y, i.z}).x;
        float n010 = hash33({i.x, i.y + 1.0f, i.z}).x;
        float n110 = hash33({i.x + 1.0f, i.y + 1.0f, i.z}).x;
        float n001 = hash33({i.x, i.y, i.z + 1.0f}).x;
        float n101 = hash33({i.x + 1.0f, i.y, i.z + 1.0f}).x;
        float n011 = hash33({i.x, i.y + 1.0f, i.z + 1.0f}).x;
        float n111 = hash33({i.x + 1.0f, i.y + 1.0f, i.z + 1.0f}).x;

        float x00 = mix(n000, n100, f.x);
        float x10 = mix(n010, n110, f.x);
        float x01 = mix(n001, n101, f.x);
        float x11 = mix(n011, n111, f.x);
        return mix(mix(x00, x10, f.y), mix(x01, x11, f.y), f.z) * 2.0f - 1.0f;
    }

    /**
     * One crater's radial cross-section, in units of its own radius: -1 at the
     * floor, +0.40 at the rim, back to 0 by t = 1.6.
     *
     * The shape of this curve is what separates a crater from a dimple. A single
     * smoothstep gives a gentle dish that reads as a golf ball. A real simple
     * crater is nearly flat across the floor, turns up sharply into a steep
     * wall, and peaks in a tight rim - so the floor term is held near -1 until
     * t = 0.55 and the wall does its work between 0.42 and 1.0.
     *
     * Nothing survives past t = 1.6, which is what lets craterLayer get away
     * with searching only its 27 immediate neighbours: influence has to die
     * inside one cell of the centre or craters would be clipped at the search
     * boundary.
     */
    float craterProfile(float t, float depth, float rim) {
        float floorT = smoothstep(0.0f, 0.55f, t);
        float wall = smoothstep(0.42f, 1.0f, t);
        // depth and rim are multipliers on these terms, both 1.0 by default -
        // not replacements for the constants. Substituting depth for the outer
        // radius scaling instead makes a 1.2 setting three times too deep and
        // the rock comes out as radial gashes rather than craters.
        float bowl = -(1.0f - wall) * (0.74f + 0.26f * (1.0f - floorT)) * depth;
        float lip = std::exp(-std::pow((t - 1.0f) * 4.0f, 2.0f)) * 0.40f * rim;
        return bowl + lip;
    }

    /**
     * One crater population: a Worley grid of impact sites, accumulated rather
     * than nearest-point.
     *
     * Taking only the nearest feature point is the obvious implementation and it
     * is wrong here. The typical distance to the nearest point in a filled grid
     * is about half a cell, which is also about the crater radius, so nearly
     * every point on the surface lands on some crater's rim and the result is a
     * field of overlapping rings - crinkled foil, not craters. Leaving most
     * cells empty (`amount` is literally the fraction that are struck) and
     * summing what remains gives isolated bowls with untouched ground between
     * them, and lets later impacts overprint earlier ones where they do overlap.
     *
     * Depth scales with radius because that is how simple craters work: depth
     * over diameter holds roughly constant, so big craters are deep bowls and
     * small ones are shallow pocks rather than scaled copies of each other.
     *
     * `size` is the mean radius as a fraction of the body radius, and the cell
     * is sized to hold one with room to spare (2.2x). `seed` offsets the lattice
     * so stacked populations - and separate asteroids - do not share centres.
     */
    float craterLayer(const Vec3f &p, float size, float amount, float seed, float irregularity,
                      float depth, float rim) {
        float scale = 1.0f / (2.2f * std::max(size, 0.02f));

        // One noise sample, shared by every crater in this generation, at a
        // frequency tied to the crater size so the wobble stays in proportion. A
        // crater that is a perfect circle of perfectly even depth is half of why
        // large ones look comical; real rims are ragged and real floors uneven,
        // and both fall out of perturbing the same field. At irregularity = 0
        // this drops out entirely.
        float jit = vnoise({p.x * (3.5f * scale), p.y * (3.5f * scale), p.z * (3.5f * scale)}) * irregularity;

        Vec3f sp = {p.x * scale + seed, p.y * scale + seed, p.z * scale + seed};
        Vec3f ip = {std::floor(sp.x), std::floor(sp.y), std::floor(sp.z)};
        Vec3f fp = {sp.x - ip.x, sp.y - ip.y, sp.z - ip.z};

        float h = 0.0f;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                for (int z = -1; z <= 1; z++) {
                    Vec3f g = {(float) x, (float) y, (float) z};
                    Vec3f cell = {ip.x + g.x, ip.y + g.y, ip.z + g.z};
                    Vec3f o = hash33(cell);
                    Vec3f r = hash33({cell.x + 19.19f, cell.y + 19.19f, cell.z + 19.19f});

                    float struck = amount >= r.x ? 1.0f : 0.0f;
                    float radius = 0.45f * (0.7f + r.y * 0.6f); // 0.7-1.3x the mean

                    // Jitter the outline, but scale depth by the unjittered
                    // radius: the rim should wander, the crater should not get
                    // deeper where it happens to be wider.
                    float jittered = radius * (1.0f + jit * 0.5f);

                    float dx = g.x + o.x - fp.x;
                    float dy = g.y + o.y - fp.y;
                    float dz = g.z + o.z - fp.z;
                    float d = std::sqrt(dx * dx + dy * dy + dz * dz);

                    h += struck * craterProfile(d / jittered, depth, rim) * (1.0f + jit * 0.4f) * radius * 0.4f;
                }
            }
        }
        return h / scale; // cell units back to body radii
    }

    /**
     * Generations of craters stacked, coarsest first - the crater equivalent of
     * the fBm's octaves, and the reason size and detail are separate controls.
     * Size fixes the largest basins; detail decides how many finer generations
     * overprint them.
     *
     * Each generation is smaller and denser than the last, because small
     * impactors vastly outnumber large ones. Depth already tracks radius inside
     * craterLayer, so the fine generations come out as shallow pocks rather than
     * shrunken copies of the basins.
     *
     * The second weight is the important one. A generation finer than the mesh
     * can represent does not render as small craters - it aliases into crinkled
     * foil - so each is faded out as its radius approaches the vertex spacing.
     * On a coarse asteroid that correctly leaves fewer generations standing.
     */
    float craters(const Vec3f &p, const DistortionParams &params) {
        if (params.craterAmount <= 0.0f) {
            return 0.0f;
        }
        float generations = params.generations;
        float h = 0.0f;
        float size = params.craterSize;
        float amount = params.craterAmount;
        for (int i = 0; i < 4; i++) {
            float w = std::clamp(generations - (float) i, 0.0f, 1.0f);
            w *= smoothstep(params.minFeature * 2.5f, params.minFeature * 5.0f, size);
            // A fully gated generation contributes nothing, so skip its 27-cell
            // search rather than multiplying the result by zero. On a coarse
            // mesh that is most of them.
            if (w > 0.0f) {
                h += w * craterLayer(p, size, amount, (float) i * 71.3f + params.craterSeed,
                                     params.irregularity, params.craterDepth, params.craterRim);
            }
            size *= params.craterFalloff;
            amount = std::min(amount * 1.5f, 1.0f);
        }
        return h;
    }

    /**
     * The octave count is the detail knob rather than a constant. The weight
     * fades the final octave in fractionally, so moving detail is a continuous
     * change rather than a pop each time a whole octave appears. At detail = 1/3
     * the weights come out 1,1,1,1,0,0 - four octaves at gain 0.5.
     *
     * Deliberately *not* divided by the sum of the amplitudes: normalising that
     * way drags typical output down to a few percent and the rock renders as a
     * smooth ball.
     */
    double fbm(const siv::PerlinNoise &perlin, const Vector3 &p, const DistortionParams &params) {
        float octaves = params.octaves;
        double sum = 0;
        double amp = 1;
        double freq = 1;
        for (int i = 0; i < 6; i++) {
            float w = std::clamp(octaves - (float) i, 0.0f, 1.0f);
            if (w > 0) {
                sum += w * perlin.noise3D(p.x * freq, p.y * freq, p.z * freq) * amp;
            }
            amp *= 0.5;
            freq *= 2;
        }
        return sum;
    }
}

float meanEdgeLength(std::size_t faceCount) {
    if (faceCount == 0) {
        return 0.0f;
    }
    return (float) std::sqrt(16.0 * M_PI / (std::sqrt(3.0) * (double) faceCount));
}

std::vector<Vector3> & distortMesh(std::vector<Vector3> &vertices, float amount, float frequency) {
    const siv::PerlinNoise perlin(rand());
    for (Vector3 &vertex: vertices) {
        float halfAmount = amount / 2;

        double amount = perlin.accumulatedOctaveNoise3D(
                vertex.x * frequency,
                vertex.y * frequency,
                vertex.z * frequency,
                8);

        amount = map(amount, {0, 1}, {1 - halfAmount, 1 + halfAmount});
        vertex = vertex.scale(amount);
    }
    return vertices;
}

/**
 * Radial displacement in one pass: stacked crater generations, plus the fBm
 * lumps on top.
 *
 * The craters are layered on rather than folded into the fBm. fBm is symmetric
 * about zero, so its bumps and dents come out the same size and the surface
 * reads as melted; a crater is a bowl with a *raised rim*, and that
 * non-monotonic profile is the shape signature no amount of tuning gets out of
 * fBm.
 *
 * Expects unit-length vertices - both sphere primitives produce them - since the
 * crater lattice and the fBm are both scaled in body radii.
 */
std::vector<Vector3> & distortMesh(std::vector<Vector3> &vertices, const DistortionParams &params,
                                   std::vector<float> *craterHeights) {
    const siv::PerlinNoise perlin(rand());
    if (craterHeights != nullptr) {
        craterHeights->clear();
        craterHeights->reserve(vertices.size());
    }
    for (Vector3 &vertex: vertices) {
        Vec3f p = {(float) vertex.x, (float) vertex.y, (float) vertex.z};

        float craterHeight = craters(p, params);
        if (craterHeights != nullptr) {
            craterHeights->push_back(craterHeight);
        }

        double radius = 1.0 + craterHeight;
        if (params.amount > 0) {
            Vector3 scaled = vertex.scale(params.frequency);
            radius += fbm(perlin, scaled, params) * params.amount * 0.5;
        }
        vertex = vertex.scale(radius);
        // Last, and componentwise. The normals are recomputed from the finished
        // vertices, so a non-uniform scale here stays correct rather than
        // needing the inverse-transpose a scale in the matrix would.
        vertex.x *= params.stretchX;
        vertex.y *= params.stretchY;
        vertex.z *= params.stretchZ;
    }
    return vertices;
}
