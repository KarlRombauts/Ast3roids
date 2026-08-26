#ifndef STARFOX_NOISEDISTORTION_H
#define STARFOX_NOISEDISTORTION_H

#include <vector>
#include <cstddef>
#include <Vector3.h>
#include <Helpers.h>

/**
 * The knobs the asteroid surface is built from, one for one with the sliders in
 * the portfolio write-up's icosphere demo, so a shape approved there can be
 * dialled in here.
 *
 * The defaults are the settings that demo was approved on. `frequency` stays on
 * the base frequency the asteroid factory has always used - the fBm generates
 * the higher octaves itself now. `minFeature` has to be filled in per mesh (see
 * meanEdgeLength) or every crater generation gates itself off.
 */
struct DistortionParams {
    /** Radial fBm displacement, as a fraction of the body radius. */
    float amount = 0.08f;

    /** Base frequency of the fBm. Each octave doubles it. */
    float frequency = 1.95f;

    /** Fraction of lattice cells holding an impact, in the coarsest generation. */
    float craterAmount = 1.00f;

    /** Mean crater radius of the coarsest generation, in body radii. */
    float craterSize = 0.45f;

    /**
     * Octave count and generation count, separately.
     *
     * These were one `detail` knob driving both, which cannot express a rock
     * that wants many noise octaves and few crater generations (or the
     * reverse) - and the demo these parameters come from has since split them
     * for exactly that reason.
     */
    float octaves = 6.0f;
    float generations = 2.7f;

    /** Amplitude falloff per fBm octave. Low is a few big lumps under a smooth skin. */
    float gain = 0.54f;

    /** Size falloff per crater generation. */
    float craterFalloff = 0.56f;

    /** Crater depth as a fraction of crater radius. */
    float craterDepth = 1.20f;

    /** Height of the raised rim, relative to the bowl. */
    float craterRim = 0.30f;

    /** How far crater outlines and depths wander. 0 gives textbook circles. */
    float irregularity = 0.40f;

    /**
     * Mean edge length of the mesh being displaced. Crater generations finer
     * than the mesh can represent are faded out rather than left to alias into
     * crinkle.
     */
    float minFeature = 0.0f;

    /**
     * Non-uniform body scale, applied after the radial displacement.
     *
     * Every knob above moves the surface in and out along the radius, which can
     * roughen a sphere but cannot stop it being one - a rock built from them
     * alone is always a ball with dents. Real bodies are elongated, flattened,
     * sometimes two lobes stuck together, and that proportion is the first thing
     * the eye reads at any distance. Applied last, so the crater lattice is
     * still sampled on the round body the field was designed for.
     */
    float stretchX = 1.0f;
    float stretchY = 1.0f;
    float stretchZ = 1.0f;

    /**
     * Offsets the crater lattice so two asteroids do not come out pockmarked
     * identically. The Perlin term reseeds itself per call; the crater hash is a
     * fixed arithmetic one, so it needs this.
     */
    float craterSeed = 0.0f;
};

/**
 * Mean edge length of a unit sphere tiled by `faceCount` triangles: each covers
 * 4pi/N of the surface, and an equilateral triangle of edge e has area
 * e^2*sqrt(3)/4. Derived rather than tabulated per subdivision level so it holds
 * for the UV sphere too, whose triangles are not equilateral but average out
 * close enough for a fade threshold.
 */
float meanEdgeLength(std::size_t faceCount);

std::vector<Vector3> & distortMesh(std::vector<Vector3> &vertices, float amount, float frequency);

/**
 * @param craterHeights if non-null, filled with the crater displacement applied
 *        to each vertex - the fBm term excluded. The shader tones the surface by
 *        it: floors dark with settled regolith, rims bright with fresh material.
 */
std::vector<Vector3> & distortMesh(std::vector<Vector3> &vertices, const DistortionParams &params,
                                   std::vector<float> *craterHeights = nullptr);

#endif //STARFOX_NOISEDISTORTION_H
