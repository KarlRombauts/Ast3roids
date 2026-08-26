//
// Created by Karl Rombauts on 5/6/21.
//

#ifndef STARFOX_ASTEROIDFACTORY_H
#define STARFOX_ASTEROIDFACTORY_H


#include "ecs/EntityManager.h"
#include <Components/Material.h>
#include <Helpers/NoiseDistortion.h>
#include <vector>

/**
 * One approved asteroid look: the displacement the vertices are built from, and
 * the shader settings that finish it.
 *
 * The two halves have to travel together. A crater field is only half a rock -
 * how fine the surface grain is and how hard the craters darken the tone are
 * what separate two bodies built from similar geometry - and those live on the
 * Material, not in DistortionParams. Keeping them in one struct is what makes a
 * look something that can be approved in the lab and used in the game, rather
 * than a set of numbers spread across two systems that drift apart.
 */
struct AsteroidPreset {
    const char *name;
    DistortionParams mesh;

    // Material fields. Tone is deliberately identical across the set - these
    // were picked on shape alone, under one colour.
    float detailScale;
    float detailStrength;
    float detailRidge;
};

class AsteroidFactory {

public:
    static Entity *create(EntityManager &entities, double radius);

    static double setTransformations(Entity *asteroid, double radius);

    static int getSubdivisions(double radius);

    /**
     * @param subdivisions icosphere level to build at, or -1 (the default, and
     *        the only value the game passes) for getSubdivisions(radius). The
     *        override exists for the standalone asteroid lab, which needs to
     *        vary the tessellation independently of the body size.
     */
    static void setGeometry(Entity *asteroid, double radius, int subdivisions = -1);

    /** Build the reusable asteroid meshes up front, before play starts. */
    static void warmGeometryPool();

    /**
     * The approved looks, one mesh variant each.
     *
     * Picked in the standalone lab from a contact sheet of sixty, judged on
     * shape alone under one fixed colour and rig. The flag line the lab prints
     * for a look maps one for one onto the fields here, so a new one can be
     * dialled there and pasted in without translation.
     */
    static const std::vector<AsteroidPreset> &presets();

    /** The Material for preset `index`, built once and shared by every asteroid
     *  drawn from that preset. */
    static Material *presetMaterial(std::size_t index);

    /**
     * The distortion parameters the most recently assigned geometry was built
     * from, so a shader continuing the same crater field can be handed the
     * exact numbers - seed included - rather than guessing at them.
     */
    static DistortionParams lastDistortion;

    /**
     * Diagnostic hook: when set, the pool is built from these parameters
     * instead of the defaults. Lets a harness isolate one term of the
     * displacement - craters with the fbm switched off, say - which is the only
     * way to check the mesh and a shader agree about where the craters are.
     */
    static const DistortionParams *distortionOverride;

    /**
     * How many variants the pool holds *when distortionOverride is set*.
     *
     * The game no longer uses this: its pool is one mesh per entry in presets(),
     * because a variant is now a named look rather than another roll of the same
     * dice. This only governs the override path, which is the lab driving the
     * factory from its panel - and the lab wants exactly one, since it throws the
     * pool away on every slider release and paying for four level-6 meshes per
     * edit would make the panel unusable.
     */
    static int poolVariants;

    /**
     * Drops every pooled mesh so the next setGeometry builds fresh - from
     * whatever distortionOverride now points at, and with a new crater seed.
     *
     * The companion to distortionOverride, and diagnostic for the same reason:
     * without it, changing the override after the first asteroid has no effect,
     * because the pool is only ever filled once. The game builds its pool at
     * startup and never calls this.
     */
    static void clearGeometryPool();

    static void setKinematics(Entity *asteroid, double radius);

    static void setCollisions(Entity *asteroid, double radius);
};


#endif //STARFOX_ASTEROIDFACTORY_H
