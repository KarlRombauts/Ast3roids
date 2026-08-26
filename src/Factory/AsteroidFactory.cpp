#include <GameModel.h>
#include <Helpers.h>
#include <Globals.h>
#include <Components/Scale.h>
#include <Components/Asteroid.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Factory/Primatives/IcoSphere.h>
#include <Helpers/NoiseDistortion.h>
#include <Helpers/Normals.h>
#include <map>
#include <tuple>
#include <cmath>
#include <Components/Collision.h>
#include <Components/Health.h>
#include <Components/Kinematics.h>
#include <Components/SplitOnDeath.h>
#include <Platform/Time.h>
#include <Components/SharedMesh.h>
#include <Render/Mesh.h>
#include <memory>
#include <cstdio>
#include "AsteroidFactory.h"

Entity *
AsteroidFactory::create(EntityManager &entities, double radius) {
    Entity *asteroid = entities.create();
    asteroid->assign<Asteroid>(radius);
    asteroid->assign<Health>(radius * 10);

    // Assign Split Component to large asteroids
    if (radius > gameConfig.ASTEROID_MIN_SIZE) {
        asteroid->assign<SplitOnDeath>();
    }

    setTransformations(asteroid, radius);
    setGeometry(asteroid, radius);
    setCollisions(asteroid, radius);
    setKinematics(asteroid, radius);

    return asteroid;
}

void AsteroidFactory::setCollisions(Entity *asteroid, double radius) {
    asteroid->assign<Collision>(CollisionType::DYNAMIC);
    asteroid->assign<CircleCollision>(radius);
}

void AsteroidFactory::setKinematics(Entity *asteroid, double radius) {
    Kinematics kinematics;

    kinematics.velocity = Vector3::random(randf(10, 20));
    kinematics.mass = 4.0 / 3.0 * M_PI * pow(radius, 3);
    kinematics.angularVelocity = Vector3::random(
            randf(gameConfig.ASTEROID_MIN_ROTATION,
                  gameConfig.ASTEROID_MAX_ROTATION));

    asteroid->assign<Kinematics>(kinematics);
}

// Average normals across vertices that share a position. The UV sphere
// duplicates its seam column and pole vertices (for the UVs); without this they
// each average only half their triangle fan and leave a lighting crease.
/**
 * Normals for the displaced mesh, either smoothed across shared vertices or
 * faceted per triangle.
 *
 * Smooth is the game's long-standing behaviour: each face normal is accumulated
 * into its three shared vertices and normalised, so the lit surface
 * interpolates. It shows craters and lumps perfectly well - what it cannot show
 * is a hard crease.
 *
 * Faceted writes one normal per triangle onto the face itself, which is what
 * the write-up's demo shows. The usual way to do this is to give every triangle
 * its own three vertices, but that triples the vertex, normal and uv arrays -
 * ~19 MB per level 6 asteroid, copied again into every entity. It is also
 * unnecessary: Mesh::upload already writes three vertices per triangle into a
 * non-indexed buffer, so the split only ever served to make the normal lookup
 * land on a per-face value. Face::normal gets the same picture from the shared
 * vertices.
 */
static void computeNormals(Geometry &geometry, bool smooth) {
    if (smooth) {
        geometry.flatShaded = false;
        Normals::recalculate(geometry);
        return;
    }

    for (Face &face: geometry.faces) {
        Vector3 &v1 = geometry.vertices[face.vertIndices.v1];
        Vector3 &v2 = geometry.vertices[face.vertIndices.v2];
        Vector3 &v3 = geometry.vertices[face.vertIndices.v3];

        const Vector3 &edge1 = Vector3::fromTo(v1, v2);
        const Vector3 &edge2 = Vector3::fromTo(v1, v3);
        face.normal = edge1.cross(edge2).normalize();
    }
    geometry.flatShaded = true;
}

/**
 * A fixed set of pre-built asteroid meshes, generated once and reused.
 *
 * Building one is not cheap and the cost is dominated by the crater field, not
 * by the sphere: measured at -O2, level 6 is 73 ms to tessellate and 321 ms to
 * displace, ~407 ms all told, and a split builds two of them back to back on
 * the main thread while the game waits. Generating a handful up front and
 * picking one at random moves all of that to first use.
 *
 * Reuse is not visible in play because nothing about an asteroid's identity
 * lives in its mesh: setTransformations gives every one an independent random
 * position, velocity, spin and scale, so the same rock appears at a different
 * size and orientation each time.
 */
// The parameters ride with the mesh. A shader that continues this same crater
// field into the finer generations needs the exact numbers the vertices were
// built from - including the per-variant seed, which is what decides where the
// craters actually are.
struct Variant {
    Geometry geometry;
    DistortionParams distortion;
    // Uploaded once, by whichever entity draws this variant first, and then
    // shared by every asteroid built from it. unique_ptr because Mesh owns GL
    // buffers and so is non-copyable, and because the address has to stay put
    // while the vector grows - entities hold it.
    std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh());
};

/**
 * The pool, deliberately leaked.
 *
 * It holds GL buffers now that a variant carries its own Mesh, and a file-scope
 * container would be destroyed after main returns - calling glDeleteBuffers with
 * no context. Never destroying it is the fix: the process is exiting, the driver
 * reclaims everything, and the alternative is ordering static teardown against
 * the window's.
 */
static std::map<int, std::vector<Variant>> &pool() {
    static std::map<int, std::vector<Variant>> *instance =
            new std::map<int, std::vector<Variant>>();
    return *instance;
}


/**
 * The six looks, straight off the lab's contact sheet.
 *
 * Bodies stay within about 15% of spherical on purpose: collision treats an
 * asteroid as a CircleCollision of the nominal radius, so a genuinely elongated
 * one would be hit from directions that look clear and miss from ones that look
 * certain. All the variety therefore has to come from the surface, which is why
 * these differ mostly in crater population and grain rather than in outline.
 */
static AsteroidPreset makePreset(const char *name,
                                 float sx, float sy, float sz,
                                 float frequency, float amount, float gain, float octaves,
                                 float craterAmount, float craterSize, float generations,
                                 float craterFalloff, float craterRim, float craterDepth,
                                 float irregularity,
                                 float detailScale, float detailStrength, float detailRidge) {
    AsteroidPreset p;
    p.name = name;
    p.mesh.stretchX = sx; p.mesh.stretchY = sy; p.mesh.stretchZ = sz;
    p.mesh.frequency = frequency; p.mesh.amount = amount;
    p.mesh.gain = gain; p.mesh.octaves = octaves;
    p.mesh.craterAmount = craterAmount; p.mesh.craterSize = craterSize;
    p.mesh.generations = generations; p.mesh.craterFalloff = craterFalloff;
    p.mesh.craterRim = craterRim; p.mesh.craterDepth = craterDepth;
    p.mesh.irregularity = irregularity;
    p.detailScale = detailScale;
    p.detailStrength = detailStrength;
    p.detailRidge = detailRidge;
    return p;
}

const std::vector<AsteroidPreset> &AsteroidFactory::presets() {
    static const std::vector<AsteroidPreset> table = {
        //          name        stretch x/y/z      freq  amt  gain oct  cover size  gens  fall  rim  depth edge  d.scale d.str d.ridge
        makePreset("pocked",   0.92f, 1.06f, 1.00f, 0.81f, 0.13f, 0.58f, 5, 0.92f, 0.22f, 2.99f, 0.46f, 0.36f, 1.24f, 0.68f, 12.0f, 0.18f, 0.11f),
        makePreset("basined",  1.08f, 1.00f, 1.00f, 1.06f, 0.23f, 0.50f, 5, 0.51f, 0.53f, 3.44f, 0.62f, 0.43f, 1.16f, 0.69f, 16.0f, 0.32f, 0.07f),
        makePreset("cratered", 0.90f, 1.00f, 1.12f, 1.16f, 0.19f, 0.53f, 4, 0.93f, 0.37f, 2.32f, 0.49f, 0.39f, 1.47f, 0.24f,  9.0f, 0.30f, 0.37f),
        makePreset("ridged",   1.07f, 1.00f, 1.01f, 1.74f, 0.17f, 0.66f, 5, 0.43f, 0.31f, 1.83f, 0.61f, 0.50f, 1.22f, 0.44f, 20.0f, 0.29f, 0.55f),
        makePreset("coarse",   0.97f, 1.00f, 1.05f, 1.78f, 0.18f, 0.59f, 5, 0.35f, 0.36f, 2.04f, 0.58f, 0.52f, 1.31f, 0.45f, 14.0f, 0.37f, 0.25f),
        makePreset("smooth",   1.02f, 1.11f, 1.00f, 1.20f, 0.11f, 0.50f, 5, 0.29f, 0.34f, 2.00f, 0.65f, 0.54f, 1.12f, 0.35f, 11.0f, 0.24f, 0.39f),
    };
    return table;
}

/**
 * One Material per preset, cloned from the library's asteroid and finished with
 * that preset's shader settings.
 *
 * The game shared a single asteroid Material until now, which is exactly why the
 * looks could not differ: the mesh is only half of what makes one, and the other
 * half is uniform state.
 *
 * The ambient and diffuse are the lab's, not the library's. The library's
 * asteroid carries ambient (1,1,1), which was fine while a texture was
 * modulating it; against the white fallback it blows the surface flat and hides
 * the shading these were picked for. Approving a look under one set of material
 * terms and shipping it under another is not shipping the look.
 */
Material *AsteroidFactory::presetMaterial(std::size_t index) {
    static std::vector<Material *> materials;
    const std::vector<AsteroidPreset> &table = presets();
    if (materials.empty()) {
        materials.reserve(table.size());
        for (const AsteroidPreset &preset : table) {
            Material *m = new Material(*materialLibrary.ASTEROID);
            m->setAmbient(0.22f, 0.21f, 0.20f);
            m->setDiffuse(0.74f, 0.71f, 0.68f);
            m->detailNormals = true;
            m->detailScale = preset.detailScale;
            m->detailStrength = preset.detailStrength;
            m->detailRidge = preset.detailRidge;
            materials.push_back(m);
        }
    }
    return materials[index % materials.size()];
}

/**
 * One built mesh per preset, generated on first use and reused after.
 *
 * Reuse is not visible in play because nothing about an asteroid's identity
 * lives in its mesh: setTransformations gives every one an independent random
 * position, velocity, spin and scale, so the same rock appears at a different
 * size and orientation each time.
 */
// Set by pooledGeometry alongside its return value, so setGeometry can hand the
// entity the pooled mesh without the pool having to know about entities.
static Mesh *pickedMesh = nullptr;

// `material` is used only on the override path; the preset path gives each
// variant its own, since that is half of what makes the looks differ.
static const Geometry &pooledGeometry(int subdivisions, Material *material) {
    std::vector<Variant> &variants = pool()[subdivisions];

    if (variants.empty()) {
        // distortionOverride is the lab driving this from its panel: one rock,
        // its parameters, rebuilt on every slider release. The preset table is
        // what the game itself uses.
        if (AsteroidFactory::distortionOverride != nullptr) {
            const int count = AsteroidFactory::poolVariants;
            variants.reserve(count);
            for (int i = 0; i < count; i++) {
                Geometry geometry = IcoSphere::create(subdivisions, material);
                DistortionParams distortion = *AsteroidFactory::distortionOverride;
                distortion.minFeature = meanEdgeLength(geometry.faces.size());
                distortion.craterSeed = randf(0, 1000);
                distortMesh(geometry.vertices, distortion, &geometry.craterHeights);
                computeNormals(geometry, false);
                variants.push_back({std::move(geometry), distortion});
            }
        } else {
            const std::vector<AsteroidPreset> &table = AsteroidFactory::presets();
            variants.reserve(table.size());
            for (std::size_t i = 0; i < table.size(); i++) {
                // Each variant carries its own Material, which is what lets the
                // presets differ in grain and tone as well as in shape.
                Geometry geometry = IcoSphere::create(subdivisions,
                                                      AsteroidFactory::presetMaterial(i));
                DistortionParams distortion = table[i].mesh;
                distortion.minFeature = meanEdgeLength(geometry.faces.size());
                distortion.craterSeed = randf(0, 1000);
                distortMesh(geometry.vertices, distortion, &geometry.craterHeights);
                // Faceted, matching the write-up's demo. Pass true for smooth.
                computeNormals(geometry, false);
                variants.push_back({std::move(geometry), distortion});
            }
        }
    }

    Variant &picked = variants[randInt(0, (int) variants.size() - 1)];
    AsteroidFactory::lastDistortion = picked.distortion;
    pickedMesh = picked.mesh.get();
    return picked.geometry;
}

DistortionParams AsteroidFactory::lastDistortion;
const DistortionParams *AsteroidFactory::distortionOverride = nullptr;
int AsteroidFactory::poolVariants = 4;

void AsteroidFactory::clearGeometryPool() {
    pool().clear();
}

/**
 * Builds every pool the game can ask for, so the one-off cost lands here rather
 * than on the first asteroid of each size. The levels come from getSubdivisions
 * rather than being listed again, so this cannot drift away from it.
 */
void AsteroidFactory::warmGeometryPool() {
    int warmStartedAt = Time::millis();
    pooledGeometry(getSubdivisions(gameConfig.ASTEROID_MAX_START_RADIUS), materialLibrary.ASTEROID);
    pooledGeometry(getSubdivisions(gameConfig.ASTEROID_MIN_SIZE), materialLibrary.ASTEROID);
    // Only when it actually built something. warmGeometryPool is idempotent and
    // runs at every wave, so an unconditional line repeats "built" eleven times
    // for eleven waves that built nothing - which is how a log stops being read.
    // Kept for the one time it does fire: this is the only stall the game takes
    // on purpose, and a preset added without noticing its cost is exactly how it
    // creeps back up.
    int warmMillis = Time::millis() - warmStartedAt;
    if (warmMillis > 0) {
        std::printf("[asteroids] %d meshes built in %d ms\n",
                    (int) presets().size(), warmMillis);
        std::fflush(stdout);
    }
}

void AsteroidFactory::setGeometry(Entity *asteroid, double radius, int subdivisions) {
    // An icosphere, not a UV sphere: the crater field is sampled by 3D position
    // at a single scale, so it needs triangles that are the same size all over.
    int level = subdivisions < 0 ? getSubdivisions(radius) : subdivisions;
    asteroid->assign<Geometry>(pooledGeometry(level, materialLibrary.ASTEROID));
    // The mesh belongs to the pool, not to this asteroid. assign overwrites, so
    // a rebuild (the lab, on every slider release) repoints rather than leaks.
    asteroid->assign<SharedMesh>(pickedMesh);
}

/**
 * Icosphere subdivision level for an asteroid of this radius, including the
 * fragments splitting produces - every asteroid is built through here, so the
 * level and the mean edge length the crater gating uses can never disagree.
 *
 * Level 6 on the big ones is a crater-resolution decision, not a fidelity one.
 * Each crater generation is faded out as its radius approaches the mean edge
 * length, so tessellation is what decides how many survive. At L5 only the
 * coarsest generation renders (gate 0.64) and everything finer is gated to
 * zero; L6 halves the edge length and brings gen0 to full strength with a
 * quarter of gen1 behind it, which is the demo's approved look. The cost is
 * real - 81,920 triangles against 20,480 - so it is spent only where the rock
 * is big enough on screen to show it.
 */
/**
 * One level for every asteroid, whatever its size.
 *
 * Large rocks were level 6 and small ones level 5, which meant two pools - and
 * with one mesh per preset that is twelve level-6-and-5 builds before the first
 * wave, about four seconds on the main thread. It also meant a split could land
 * on a pool that had not been warmed.
 *
 * Level 6 is not buying anything any more. The fragment shader carries the fine
 * relief now, so the triangles only have to hold the craters and the silhouette,
 * and a side-by-side of the presets at 5 and 6 is indistinguishable at any
 * distance the game shows a rock from. Level 5 is a quarter of the triangles, a
 * quarter of the build, and a quarter of the memory.
 */
int AsteroidFactory::getSubdivisions(double radius) {
    (void) radius;
    return 5;
}

double AsteroidFactory::setTransformations(Entity *asteroid, double radius) {
    double l = gameModel.arenaSize;
    asteroid->assign<Position>(Vector3(randf(-l,l), randf(-l,l), randf(-l,l)));

    asteroid->assign<Scale>(radius);
    asteroid->assign<Rotation>();
    return radius;
}
