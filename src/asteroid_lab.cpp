/**
 * StarFox_asteroid - a standalone asteroid viewer.
 *
 * One rock, centred at the origin, filling the frame, and nothing else in the
 * scene. It exists so the asteroid surface shading can be iterated on without
 * flying a ship at it first: edit Shaders/basic.frag, rebuild (the shader files
 * are copied next to the binary on every build), relaunch.
 *
 * Everything here is the game's own machinery - Platform/Window for the SDL2
 * window and GL context, AsteroidFactory for the mesh, RenderSystem for the
 * draw, the same MaterialLibrary::ASTEROID material - so what you see is what
 * the game draws. The only things this file adds are the camera framing, the
 * three-point lighting rig, the input handling and the parameter panel.
 *
 * The panel (Dear ImGui, vendored in lib/imgui and linked into this binary
 * alone) drives every knob live. The command-line flags are all still there and
 * still set the starting values; the panel's "copy command line" button emits
 * the flags for whatever is currently on screen, so a configuration worth
 * keeping can be written down and turned into new defaults later.
 *
 * It does NOT touch main.cpp or any game system.
 */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "OpenGL.h"
#include <Helpers/Normals.h>
#include "Platform/Window.h"
#include "Platform/Time.h"
#include "Globals.h"
#include "GameModel.h"
#include "ecs/EntityManager.h"
#include "Systems/RenderSystem.h"
#include "Factory/AsteroidFactory.h"
#include "Components/Camera.h"
#include "Components/Geometry.h"
#include "Components/Light.h"
#include "Components/Position.h"
#include "Components/RenderMesh.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// ---------------------------------------------------------------------------
// Knobs.
//
// Every one of these is on the panel and on the command line. They live in one
// struct so "reset to defaults" is a single assignment against a snapshot taken
// before the command line is read, rather than a second copy of the defaults
// sitting somewhere waiting to drift out of step with the first.
// ---------------------------------------------------------------------------

struct LabSettings {
    /** Icosphere subdivision level. -1 = whatever AsteroidFactory picks for the
     *  radius below (6 for a large asteroid). Resolved to a real level before
     *  the window opens, so the panel always sees 0-6. Level 6 is ~82k
     *  triangles and a few hundred ms to build; drop to 4 or 5 for a panel that
     *  responds instantly. */
    int subdivisions = -1;

    /** Mesh displacement. The defaults are DistortionParams' own, so passing
     *  nothing builds exactly the rock the game builds. */
    DistortionParams mesh;

    /** Smooth base shading. With the fine detail now coming from the fragment
     *  shader, faceting is a high-frequency artifact of the geometry competing
     *  for the same band - the mesh should carry the silhouette and nothing
     *  else. */
    bool smoothShading = true;

    // Procedural detail, evaluated per fragment. All of it is a uniform write,
    // so all of it applies the instant a slider moves.
    bool detailOn = true;
    float detailScale = 2.0f;
    float detailStrength = 0.30f;
    float detailRidge = 0.25f;
    float detailAlbedo = 1.0f;
    float albedoScale = 5.0f;
    float albedoGain = 0.75f;
    float albedoContrast = 0.85f;
    float craterTone = 0.45f;
    float rockTint[3] = {1.0f, 1.0f, 1.0f};
    bool albedoOnly = false; // paint the albedo with the lighting taken away

    // Three-point rig. Intensities scale the fixed tints below; ambient is the
    // scene-wide term RenderSystem uploads.
    float keyIntensity = 1.00f;
    float fillIntensity = 0.30f;
    float rimIntensity = 1.15f;
    float ambientLevel = 0.12f;
};

static LabSettings settings;

/** The compiled-in defaults, captured before parseArgs runs. */
static LabSettings defaults;

/** Body radius, in world units. Also decides the factory's subdivision level
 *  when subdivisions is -1 (AsteroidFactory::getSubdivisions: >10 -> 6, else 5). */
static double ASTEROID_RADIUS = 20.0;

static bool CRATERS_ONLY = false;

/** Contact-sheet mode: see runGrid. Empty means the normal interactive lab. */
static std::string gridFile;
static int gridCols = 5;
static int gridRows = 4;
static int gridCell = 300; // pixels per cell, so the window is cols*cell wide

/** Window size, in pixels. Wider than tall now that a panel sits down one side. */
static const int WINDOW_WIDTH = 1100;
static const int WINDOW_HEIGHT = 900;

/** How much room to leave around the rock. 1.0 = exactly touching the frame. */
static const double FRAME_MARGIN = 1.03;

/** Idle tumble, degrees per second. Two incommensurate axes, so the poles pass
 *  through the light too rather than spinning in place. */
static const double SPIN_YAW = 9.0;
static const double SPIN_PITCH = 2.3;

/** Mouse drag sensitivity, degrees of orbit per pixel. */
static const double ORBIT_SENSITIVITY = 0.35;

/** Starting orbit, in degrees. Chosen so the key light (far away in -z, slightly
 *  up) rakes across the surface at roughly 45 degrees instead of either
 *  back-lighting it or washing it out from behind the camera. */
static const double START_YAW = 135.0;
static const double START_PITCH = -18.0;

/** Vertical field of view. Must match GameModel::resizeWorld's perspective. */
static const double FOV_Y = 60.0;

// ---------------------------------------------------------------------------
// Lighting rig.
//
// One key light and a flat scene ambient is what made the rock read as a shaded
// ball: the lit side was uniform, the dark side was dead, and the silhouette
// dissolved into the black background. Three lights answer all three - and
// RenderSystem does upload every one of them (it loops to MAX_LIGHTS = 8, which
// basic.frag also declares), so this needs no renderer changes.
//
// Every light is positional with the default constant attenuation of 1, which
// the shader applies as a plain divide - no distance falloff at all. Parking
// them 1500 units out therefore makes them behave as pure directional lights,
// which is the existing trick and the reason only direction matters here.
// ---------------------------------------------------------------------------

static const double LIGHT_DISTANCE = 1500.0;

/** Warm, and the brightest. Fixed in world space, high and beyond the rock in
 *  -z, which is where the game's skybox puts its sun disc - and the direction
 *  START_YAW was chosen against, so it still rakes at about 45 degrees. */
static const Vector3 KEY_DIRECTION(0.0, 0.10, -1.0);
static const Vector3 KEY_TINT(1.05, 1.00, 0.90);

/** Cool and dim, roughly opposite the key and lower, so the shadow side keeps
 *  some shape instead of going black. Also fixed in world space: a fill that
 *  tracked the camera would flatten the rock every time you orbited, which is
 *  the exact opposite of what it is for. */
static const Vector3 FILL_DIRECTION(0.55, -0.35, 0.78);
static const Vector3 FILL_TINT(0.55, 0.68, 0.92);

/**
 * The one that matters most: a bright, slightly cool light behind the rock
 * *relative to the camera*, so a lit edge separates the silhouette from the
 * background and the rock reads as a solid body rather than a flat disc.
 *
 * Camera-relative, so it is recomputed from the orbit angles every frame - a
 * rim light nailed to world space stops being a rim light the moment you drag.
 * Two things decide where exactly, and neither is "dead behind":
 *
 * How far behind. The shading here is Lambert plus a very tight Phong lobe
 * (basic.frag multiplies shininess by 6, so the rock's Ns of 50 becomes an
 * exponent of 300 - a pinpoint, no use at all at a grazing angle). All the rim
 * has to work with is N.L, and with the light directly behind, N.L peaks on the
 * back face nobody can see and falls to zero exactly at the limb. RIM_BEHIND of
 * 0.5 against RIM_ACROSS of 0.87 puts it 120 degrees off the view axis, which
 * is where N.L is largest on the silhouette itself.
 *
 * Which side. Opposite the key, worked out per frame rather than fixed: an edge
 * drawn down the side the key is already lighting adds nothing, and it is the
 * *shadow* side that dissolves into a black background. Since the key is fixed
 * in world space and the camera is not, which screen edge that is changes as
 * you orbit - so it has to be derived, not chosen once.
 */
static const double RIM_ACROSS = 0.87;
static const double RIM_BEHIND = 0.50;
static const double RIM_LIFT = 0.35; // classic rim sits a little above the eyeline
static const Vector3 RIM_TINT(0.80, 0.88, 1.05);

/** Slightly cool, suggesting starlight bounce. Multiplied by ambientLevel. */
static const Vector3 AMBIENT_TINT(0.85, 0.90, 1.02);

// ---------------------------------------------------------------------------

static Window window;
static EntityManager entities;
static RenderSystem renderSystem;

static Entity *rock = nullptr;
static Entity *camera = nullptr;
static Entity *keyLight = nullptr;
static Entity *fillLight = nullptr;
static Entity *rimLight = nullptr;

static bool running = true;
static bool spinning = true;
static bool wireframe = false;
static bool panelVisible = true;

static double orbitYaw = START_YAW;
static double orbitPitch = START_PITCH;
static double cameraDistance = 60.0;

/** Radius of the current mesh in model space (the icosphere is unit-radius
 *  before the crater field displaces it outward/inward). */
static double meshRadius = 1.0;

static bool dragging = false;

/** Cost of the last mesh build, shown on the panel so the subdivision slider's
 *  price is visible rather than merely felt. */
static int lastBuildMillis = 0;

/** Set by --screenshot, by the panel button and by S; saved once the scene has
 *  settled, then cleared. */
static std::string screenshotPath;

/**
 * Dumps the framebuffer to a binary PPM. Deliberately the dumbest possible
 * format - no encoder, no dependency - because its only job is to let a shader
 * change be eyeballed or diffed against the previous run.
 */
static void saveScreenshot(const std::string &path) {
    int w = gameModel.width;
    int h = gameModel.height;
    // The drawable can be larger than the window on a HiDPI display.
    int drawableW = w, drawableH = h;
    SDL_GL_GetDrawableSize(window.handle(), &drawableW, &drawableH);
    w = drawableW;
    h = drawableH;

    std::vector<unsigned char> pixels((size_t) w * h * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    FILE *file = std::fopen(path.c_str(), "wb");
    if (file == nullptr) {
        std::cout << "[lab] could not open " << path << " for writing" << std::endl;
        return;
    }
    std::fprintf(file, "P6\n%d %d\n255\n", w, h);
    // GL reads bottom-up; PPM is top-down.
    for (int y = h - 1; y >= 0; y--) {
        std::fwrite(pixels.data() + (size_t) y * w * 3, 1, (size_t) w * 3, file);
    }
    std::fclose(file);
    std::cout << "[lab] screenshot written to " << path << " (" << w << "x" << h << ")" << std::endl;
}

static void printGlErrors(const char *stage) {
    GLenum error;
    bool any = false;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "GL error at " << stage << ": 0x" << std::hex << error << std::dec << std::endl;
        any = true;
    }
    if (!any) {
        std::cout << "[lab] no GL errors at " << stage << std::endl;
    }
}

static void printBindings() {
    std::cout << "\n=== StarFox asteroid lab ===\n"
              << "  drag left mouse   orbit the camera around the rock\n"
              << "  scroll wheel      dolly in / out\n"
              << "  N or Space        regenerate (new crater seed)\n"
              << "  H                 show / hide the parameter panel\n"
              << "  W                 toggle wireframe\n"
              << "  P                 pause / resume the idle tumble\n"
              << "  F                 reset the view\n"
              << "  S                 save a screenshot (asteroid_lab.ppm)\n"
              << "  Esc or Q          quit\n"
              << "============================\n" << std::endl;
}

/** Farthest vertex from the origin, so the framing accounts for the crater
 *  displacement rather than assuming a unit sphere. */
static double computeMeshRadius(const Geometry &geometry) {
    double maxSq = 0;
    for (const Vector3 &v : geometry.vertices) {
        double sq = v.dot(v);
        if (sq > maxSq) {
            maxSq = sq;
        }
    }
    return std::sqrt(maxSq);
}

/**
 * Distance that makes the rock's bounding sphere fill the frame.
 *
 * The frustum has to *contain* the sphere, not just reach its centre-plane, so
 * the distance is r/sin(halfAngle) - the frustum plane is tangent to the sphere.
 * Using tan() instead (the obvious "opposite over adjacent" answer) puts the
 * camera ~13% too close and clips the silhouette. The narrower of the two
 * half-angles wins, so a non-square window crops nothing.
 */
static void reframe() {
    double worldRadius = meshRadius * ASTEROID_RADIUS;
    double halfV = FOV_Y * 0.5 * M_PI / 180.0;
    double halfH = std::atan(std::tan(halfV) * gameModel.aspectRatio);
    double limiting = halfV < halfH ? halfV : halfH;
    cameraDistance = worldRadius * FRAME_MARGIN / std::sin(limiting);
}

static void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    gameModel.resizeScreen(w, h);
    reframe();
}

/**
 * Pushes the shader-side knobs into the asteroid material.
 *
 * Split out of the mesh rebuild and called every frame, because that is exactly
 * the difference between the panel's two groups: these are uniform writes and
 * cost nothing, so they can apply while the slider is still moving.
 *
 * The crater terms are read from AsteroidFactory::lastDistortion, not from
 * settings.mesh - the shader has to continue the field the *vertices* were
 * built from, seed included, and settings.mesh may well have moved on since the
 * last rebuild.
 */
static void applyMaterial() {
    const DistortionParams &built = AsteroidFactory::lastDistortion;
    Material *mat = materialLibrary.ASTEROID;

    mat->detailNormals = settings.detailOn;
    mat->detailScale = settings.detailScale;
    mat->detailStrength = settings.detailStrength;
    mat->detailRidge = settings.detailRidge;
    mat->detailAlbedo = settings.detailAlbedo;
    mat->albedoScale = settings.albedoScale;
    mat->albedoGain = settings.albedoGain;
    mat->albedoContrast = settings.albedoContrast;
    mat->craterTone = settings.craterTone;
    mat->rockTint[0] = settings.rockTint[0];
    mat->rockTint[1] = settings.rockTint[1];
    mat->rockTint[2] = settings.rockTint[2];
    mat->detailDebug = settings.albedoOnly ? 1 : 0;
}

/** Colour one light by tint * intensity, diffuse and specular alike - the rim's
 *  specular is half of what makes the lit edge read. */
static void setLightColour(Entity *light, const Vector3 &tint, float intensity) {
    Light *l = light->get<Light>();
    l->setDiffuse((GLfloat) (tint.x * intensity),
                  (GLfloat) (tint.y * intensity),
                  (GLfloat) (tint.z * intensity));
    l->setSpecular((GLfloat) (tint.x * intensity),
                   (GLfloat) (tint.y * intensity),
                   (GLfloat) (tint.z * intensity));
}

/**
 * Positions and colours the rig. Called every frame: the rim light is
 * camera-relative, and the intensities are on sliders.
 */
static Vector3 unit(const Vector3 &v) {
    // Vector3::normalize is non-const (it returns a fresh vector rather than
    // mutating, but says otherwise), so the direction constants above are
    // normalised through a copy instead of being made mutable to suit it.
    Vector3 copy = v;
    return copy.normalize();
}

static void applyLighting() {
    Quaternion orbit = Quaternion::angleAxis(orbitYaw, Vector3(0, 1, 0))
                       * Quaternion::angleAxis(orbitPitch, Vector3(1, 0, 0));

    keyLight->get<Position>()->position = unit(KEY_DIRECTION) * LIGHT_DISTANCE;
    fillLight->get<Position>()->position = unit(FILL_DIRECTION) * LIGHT_DISTANCE;

    // The camera sits at orbit * +z, so that is the view axis. Project the key
    // onto the screen plane to find which side of the rock it is lighting, flip
    // that, lift it, and lean the result back by RIM_BEHIND.
    Vector3 view = orbit * Vector3(0, 0, 1);
    Vector3 key = unit(KEY_DIRECTION);
    Vector3 alongView = view * key.dot(view);
    Vector3 across = key - alongView;
    if (across.magnitude() < 1e-3) {
        // Key is straight down the barrel and has no side. Any side will do.
        across = orbit * Vector3(1, 0, 0);
    }
    Vector3 up = orbit * Vector3(0, 1, 0);
    Vector3 lifted = unit(across) * -1.0;
    Vector3 side = unit(lifted + up * RIM_LIFT);
    Vector3 behind = view * -RIM_BEHIND;
    Vector3 rim = side * RIM_ACROSS + behind;
    rimLight->get<Position>()->position = unit(rim) * LIGHT_DISTANCE;

    setLightColour(keyLight, KEY_TINT, settings.keyIntensity);
    setLightColour(fillLight, FILL_TINT, settings.fillIntensity);
    setLightColour(rimLight, RIM_TINT, settings.rimIntensity);

    RenderSystem::globalAmbient[0] = (float) AMBIENT_TINT.x * settings.ambientLevel;
    RenderSystem::globalAmbient[1] = (float) AMBIENT_TINT.y * settings.ambientLevel;
    RenderSystem::globalAmbient[2] = (float) AMBIENT_TINT.z * settings.ambientLevel;
}

/**
 * Rebuilds the rock's mesh through the game's own factory, and drops the
 * uploaded GPU mesh so RenderSystem re-uploads it on the next frame.
 *
 * `freshSeed` throws the factory's geometry pool away first, which is what makes
 * a parameter change (or a press of N) actually reach the vertices - the pool is
 * otherwise filled once and reused forever, override or no override. Without it
 * this is a cheap re-fetch of the mesh already built, which is all the smooth /
 * faceted toggle needs.
 */
static void rebuildRock(bool freshSeed) {
    int startedAt = Time::millis();

    if (freshSeed) {
        AsteroidFactory::clearGeometryPool();
    }
    AsteroidFactory::setGeometry(rock, ASTEROID_RADIUS, settings.subdivisions);

    Geometry *geometry = rock->get<Geometry>();
    if (settings.smoothShading) {
        // The factory hands back a faceted mesh for the game's low-poly look.
        // Recompute smooth vertex normals over it - correct now that
        // Normals::recalculate clears before accumulating instead of piling
        // onto whatever the primitive left behind.
        geometry->flatShaded = false;
        Normals::recalculate(*geometry);
    }

    // Hand the shader the very field the vertices came from. Without the seed
    // the fine generations would sit in different places from the coarse ones -
    // detail that plainly belongs to a different rock.
    applyMaterial();

    lastBuildMillis = Time::millis() - startedAt;

    meshRadius = computeMeshRadius(*geometry);
    reframe();

    rock->remove<RenderMesh>();

    std::cout << "[lab] asteroid: " << geometry->faces.size() << " triangles, "
              << geometry->vertices.size() << " vertices, radius "
              << (meshRadius * ASTEROID_RADIUS) << " world units"
              << " (" << lastBuildMillis << " ms)" << std::endl;
}

/** Camera orientation from the orbit angles: yaw about world up, then pitch
 *  about the camera's own right axis. Position is straight back along the view
 *  direction, so the rock stays centred whatever the angles are. */
static void updateCamera() {
    Quaternion yaw = Quaternion::angleAxis(orbitYaw, Vector3(0, 1, 0));
    Quaternion pitch = Quaternion::angleAxis(orbitPitch, Vector3(1, 0, 0));
    Quaternion rotation = yaw * pitch;

    camera->get<Rotation>()->rotation = rotation;
    // An unrotated camera looks down -z, so +z is "backwards" from the target.
    camera->get<Position>()->position = (rotation * Vector3(0, 0, 1)) * cameraDistance;
}

static void buildScene() {
    if (CRATERS_ONLY) {
        // fbm off, so the mesh's radius is craters and nothing else - the only
        // way the red/green overlay compares like with like.
        settings.mesh.amount = 0.0f;
    }
    // Always on now that the panel drives it. The defaults are DistortionParams'
    // own, so an untouched panel builds exactly the rock the game builds.
    AsteroidFactory::distortionOverride = &settings.mesh;
    // One variant per level, not four: the pool is thrown away and rebuilt every
    // time a mesh slider is released, and paying for four level-6 meshes per
    // edit - most of two seconds - would make the panel unusable.
    AsteroidFactory::poolVariants = 1;

    rock = entities.create();
    rock->assign<Position>(Vector3(0, 0, 0));
    rock->assign<Rotation>();
    rock->assign<Scale>(ASTEROID_RADIUS);
    rebuildRock(true);

    camera = entities.create();
    camera->assign<Camera>(FOV_Y, gameModel.aspectRatio, 1, 10000);
    camera->assign<Position>(Vector3(0, 0, cameraDistance));
    camera->assign<Rotation>();
    gameModel.activeCamera = camera;
    updateCamera();

    // The rig itself. Positions and colours are all applyLighting's business,
    // so they are left at whatever the components default to here.
    keyLight = entities.create();
    keyLight->assign<Position>();
    keyLight->assign<Light>();

    fillLight = entities.create();
    fillLight->assign<Position>();
    fillLight->assign<Light>();

    rimLight = entities.create();
    rimLight->assign<Position>();
    rimLight->assign<Light>();

    applyLighting();
}

// ---------------------------------------------------------------------------
// The panel
// ---------------------------------------------------------------------------

static void appendFlag(std::string &out, const char *name, double value) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), " %s %g", name, value);
    out += buffer;
}

/**
 * The current settings as a runnable command line.
 *
 * Every flag, not just the ones that differ from the defaults: the point is to
 * paste a line into a note and get this exact rock back months later, and a
 * line that leans on today's defaults will not survive them changing - which,
 * given that recording good defaults is what this button is for, is precisely
 * the case it has to survive.
 */
static std::string currentFlags() {
    const DistortionParams &m = settings.mesh;
    std::string out = "./StarFox_asteroid";

    appendFlag(out, "--subdivisions", settings.subdivisions);
    appendFlag(out, "--radius", ASTEROID_RADIUS);

    appendFlag(out, "--amount", m.amount);
    appendFlag(out, "--frequency", m.frequency);
    appendFlag(out, "--octaves", m.octaves);
    appendFlag(out, "--gain", m.gain);
    appendFlag(out, "--crater-amount", m.craterAmount);
    appendFlag(out, "--crater-size", m.craterSize);
    appendFlag(out, "--generations", m.generations);
    appendFlag(out, "--crater-falloff", m.craterFalloff);
    appendFlag(out, "--crater-depth", m.craterDepth);
    appendFlag(out, "--crater-rim", m.craterRim);
    appendFlag(out, "--irregularity", m.irregularity);

    appendFlag(out, "--detail-scale", settings.detailScale);
    appendFlag(out, "--detail-strength", settings.detailStrength);
    appendFlag(out, "--detail-ridge", settings.detailRidge);
    appendFlag(out, "--detail-albedo", settings.detailAlbedo);
    appendFlag(out, "--albedo-scale", settings.albedoScale);
    appendFlag(out, "--albedo-gain", settings.albedoGain);
    appendFlag(out, "--albedo-contrast", settings.albedoContrast);
    appendFlag(out, "--crater-tone", settings.craterTone);
    appendFlag(out, "--stretch-x", settings.mesh.stretchX);
    appendFlag(out, "--stretch-y", settings.mesh.stretchY);
    appendFlag(out, "--stretch-z", settings.mesh.stretchZ);
    appendFlag(out, "--tint-r", settings.rockTint[0]);
    appendFlag(out, "--tint-g", settings.rockTint[1]);
    appendFlag(out, "--tint-b", settings.rockTint[2]);
    if (settings.albedoOnly) out += " --albedo-only";

    appendFlag(out, "--key", settings.keyIntensity);
    appendFlag(out, "--fill", settings.fillIntensity);
    appendFlag(out, "--rim", settings.rimIntensity);
    appendFlag(out, "--ambient", settings.ambientLevel);

    if (!settings.detailOn) out += " --no-detail";
    if (!settings.smoothShading) out += " --faceted";

    return out;
}

/**
 * A mesh slider. Returns true when the drag has *finished*, which is the whole
 * reason these are wrapped rather than called directly.
 *
 * A rebuild is a few hundred ms at level 6, so rebuilding per frame while a
 * slider is held would drop the frame rate to about two and make a value
 * impossible to aim at. Rebuilding on release costs one rebuild per gesture,
 * the number still tracks the mouse the whole way down, and nothing has to be
 * applied by hand afterwards - which is why this rather than an Apply button.
 * Anyone who wants the loop faster still has the subdivision slider right
 * there.
 */
static bool meshSlider(const char *label, float *value, float min, float max) {
    ImGui::SliderFloat(label, value, min, max);
    return ImGui::IsItemDeactivatedAfterEdit();
}

static void drawPanel() {
    if (!panelVisible) {
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(12, 12), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(390, 780), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Asteroid lab", &panelVisible)) {
        ImGui::End();
        return;
    }

    // Set by any mesh control that was released this frame. Collected rather
    // than acted on where it happens, so two controls released on the same
    // frame cannot cost two rebuilds - and so the rebuild lands outside the
    // ImGui frame rather than stalling in the middle of one.
    bool rebuild = false;

    ImGui::PushItemWidth(-140);

    if (ImGui::CollapsingHeader("Mesh displacement", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextDisabled("rebuilds the geometry when you let go");

        DistortionParams &m = settings.mesh;
        rebuild |= meshSlider("amount", &m.amount, 0.0f, 0.5f);
        rebuild |= meshSlider("frequency", &m.frequency, 0.2f, 8.0f);
        rebuild |= meshSlider("octaves", &m.octaves, 1.0f, 10.0f);
        rebuild |= meshSlider("gain", &m.gain, 0.1f, 0.9f);
        rebuild |= meshSlider("crater amount", &m.craterAmount, 0.0f, 1.0f);
        rebuild |= meshSlider("crater size", &m.craterSize, 0.02f, 1.0f);
        rebuild |= meshSlider("generations", &m.generations, 0.0f, 8.0f);
        rebuild |= meshSlider("crater falloff", &m.craterFalloff, 0.2f, 0.95f);
        rebuild |= meshSlider("crater depth", &m.craterDepth, 0.0f, 3.0f);
        rebuild |= meshSlider("crater rim", &m.craterRim, 0.0f, 1.5f);
        rebuild |= meshSlider("irregularity", &m.irregularity, 0.0f, 1.5f);
        rebuild |= meshSlider("stretch x", &m.stretchX, 0.4f, 2.0f);
        rebuild |= meshSlider("stretch y", &m.stretchY, 0.4f, 2.0f);
        rebuild |= meshSlider("stretch z", &m.stretchZ, 0.4f, 2.0f);
        ImGui::SetItemTooltip("Non-uniform body scale, applied after the displacement.\n"
                              "Everything else can only roughen a sphere; this is what\n"
                              "stops it being one.");

        ImGui::SliderInt("subdivisions", &settings.subdivisions, 0, 6);
        ImGui::SetItemTooltip("Level 6 is the game's own tessellation and a few\n"
                              "hundred ms to build. 4 or 5 makes this panel snappy.");
        rebuild |= ImGui::IsItemDeactivatedAfterEdit();

        if (ImGui::Checkbox("smooth shading", &settings.smoothShading)) {
            // Only the normals change, so this re-fetches the mesh already in
            // the pool rather than building a new one.
            rebuildRock(false);
        }

        if (ImGui::Button("Regenerate")) {
            rebuild = true;
        }
        ImGui::SetItemTooltip("Same parameters, new crater seed.");
        ImGui::SameLine();
        if (ImGui::Button("Reset to defaults")) {
            settings = defaults;
            settings.subdivisions = defaults.subdivisions < 0
                                    ? AsteroidFactory::getSubdivisions(ASTEROID_RADIUS)
                                    : defaults.subdivisions;
            rebuild = true;
        }
        ImGui::SetItemTooltip("Every group, not just this one.");

        ImGui::Text("%d triangles, %d ms to build",
                    (int) rock->get<Geometry>()->faces.size(), lastBuildMillis);
    }

    if (ImGui::CollapsingHeader("Shader detail", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextDisabled("uniform writes - applies as you drag");

        ImGui::Checkbox("detail normals", &settings.detailOn);
        ImGui::SliderFloat("detail scale", &settings.detailScale, 0.1f, 40.0f);
        ImGui::SliderFloat("detail strength", &settings.detailStrength, 0.0f, 1.5f);
        ImGui::SliderFloat("detail ridge", &settings.detailRidge, 0.0f, 1.0f);
        ImGui::SliderFloat("detail albedo", &settings.detailAlbedo, 0.0f, 1.0f);
        ImGui::SetItemTooltip("Blends the whole procedural tone against a flat rock colour.");
    }

    if (ImGui::CollapsingHeader("Albedo", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("albedo scale", &settings.albedoScale, 0.2f, 20.0f);
        ImGui::SetItemTooltip("Base frequency of the tonal octaves. Low is continent-sized\n"
                              "patches; high is gravel.");
        ImGui::SliderFloat("albedo gain", &settings.albedoGain, 0.2f, 0.85f);
        ImGui::SetItemTooltip("Amplitude falloff per octave. Low leaves a few broad patches;\n"
                              "high gives every scale equal say and reads as grit.");
        ImGui::SliderFloat("albedo contrast", &settings.albedoContrast, 0.0f, 1.0f);
        ImGui::SetItemTooltip("How far the noise swings between weathered and fresh.");
        ImGui::SliderFloat("crater tone", &settings.craterTone, 0.0f, 1.0f);
        ImGui::SetItemTooltip("How hard the baked crater height drives the colour: floors dark,\n"
                              "rims bright. Free - it is the displacement the vertices carry,\n"
                              "not a field re-evaluated per pixel.");
        ImGui::ColorEdit3("rock tint", settings.rockTint);
        ImGui::SetItemTooltip("Multiplies the finished colour. Near-black for a carbonaceous\n"
                              "body, warm for a stony one, cool grey for a metallic one.");
        ImGui::Checkbox("albedo only", &settings.albedoOnly);
        ImGui::SetItemTooltip("Lighting off, so a tone can be judged on its own.");
    }

    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("key", &settings.keyIntensity, 0.0f, 2.0f);
        ImGui::SetItemTooltip("Warm and brightest. Fixed high and beyond the rock in -z.");
        ImGui::SliderFloat("fill", &settings.fillIntensity, 0.0f, 1.0f);
        ImGui::SetItemTooltip("Cool and dim, opposite the key and lower. Keeps the\n"
                              "shadow side from going flat black.");
        ImGui::SliderFloat("rim", &settings.rimIntensity, 0.0f, 3.0f);
        ImGui::SetItemTooltip("Cool back light, held 120 degrees off the view axis\n"
                              "and opposite the key, so it edges whichever side is\n"
                              "in shadow. Take it to zero and watch the silhouette\n"
                              "dissolve into the background.");
        ImGui::SliderFloat("ambient", &settings.ambientLevel, 0.0f, 0.6f);
        ImGui::SetItemTooltip("Scene-wide and directionless. The fill light does this\n"
                              "job with shape instead, so this can stay low.");
    }

    if (ImGui::CollapsingHeader("View", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("wireframe", &wireframe)) {
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }
        ImGui::SameLine();
        ImGui::Checkbox("tumble", &spinning);
        if (ImGui::Button("Reset view")) {
            orbitYaw = START_YAW;
            orbitPitch = START_PITCH;
            reframe();
            updateCamera();
        }
        ImGui::SameLine();
        if (ImGui::Button("Screenshot")) {
            screenshotPath = "asteroid_lab.ppm";
        }
        ImGui::SetItemTooltip("asteroid_lab.ppm, taken before this panel is drawn.");
    }

    if (ImGui::CollapsingHeader("Command line", ImGuiTreeNodeFlags_DefaultOpen)) {
        std::string flags = currentFlags();
        // Read-only and wrapped, so the whole line is readable in place and can
        // be selected by hand as well as copied wholesale.
        ImGui::InputTextMultiline("##flags", flags.data(), flags.size() + 1,
                                  ImVec2(-1, ImGui::GetTextLineHeight() * 7),
                                  ImGuiInputTextFlags_ReadOnly);
        if (ImGui::Button("Copy to clipboard")) {
            ImGui::SetClipboardText(flags.c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("Print to stdout")) {
            std::cout << "[lab] " << flags << std::endl;
        }
    }

    ImGui::PopItemWidth();
    ImGui::End();

    if (rebuild) {
        rebuildRock(true);
    }
}

// ---------------------------------------------------------------------------

static void handleEvent(const SDL_Event &event) {
    ImGuiIO &io = ImGui::GetIO();
    // A drag that started on the rock keeps the mouse even if it wanders over
    // the panel; without the !dragging the orbit would stall the moment the
    // cursor crossed the panel's edge. A drag that started on the panel never
    // sets `dragging` in the first place, so it can never claim the orbit.
    bool panelWantsMouse = io.WantCaptureMouse && !dragging;
    bool panelWantsKeys = io.WantCaptureKeyboard;

    switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            if (panelWantsKeys) {
                break;
            }
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    running = false;
                    break;
                case SDLK_n:
                case SDLK_SPACE:
                    rebuildRock(true);
                    break;
                case SDLK_h:
                    panelVisible = !panelVisible;
                    std::cout << "[lab] panel " << (panelVisible ? "shown" : "hidden") << std::endl;
                    break;
                case SDLK_w:
                    wireframe = !wireframe;
                    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
                    std::cout << "[lab] wireframe " << (wireframe ? "on" : "off") << std::endl;
                    break;
                case SDLK_p:
                    spinning = !spinning;
                    std::cout << "[lab] tumble " << (spinning ? "running" : "paused") << std::endl;
                    break;
                case SDLK_s:
                    screenshotPath = "asteroid_lab.ppm";
                    break;
                case SDLK_f:
                    orbitYaw = START_YAW;
                    orbitPitch = START_PITCH;
                    reframe();
                    updateCamera();
                    std::cout << "[lab] view reset" << std::endl;
                    break;
                default:
                    break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (panelWantsMouse) {
                break;
            }
            if (event.button.button == SDL_BUTTON_LEFT) {
                dragging = true;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            // Never filtered: a button-up swallowed by the panel would leave
            // `dragging` stuck on and the camera spinning with the free cursor.
            if (event.button.button == SDL_BUTTON_LEFT) {
                dragging = false;
            }
            break;

        case SDL_MOUSEMOTION:
            if (dragging) {
                orbitYaw -= event.motion.xrel * ORBIT_SENSITIVITY;
                orbitPitch -= event.motion.yrel * ORBIT_SENSITIVITY;
                // Stop short of the poles so the orbit never gimbal-flips.
                if (orbitPitch > 89) orbitPitch = 89;
                if (orbitPitch < -89) orbitPitch = -89;
                updateCamera();
            }
            break;

        case SDL_MOUSEWHEEL: {
            if (panelWantsMouse) {
                break;
            }
            double step = cameraDistance * 0.08 * (event.wheel.y > 0 ? -1 : 1);
            if (event.wheel.y != 0) {
                cameraDistance += step;
                double minDistance = meshRadius * ASTEROID_RADIUS * 1.05;
                if (cameraDistance < minDistance) cameraDistance = minDistance;
                if (cameraDistance > 5000) cameraDistance = 5000;
                updateCamera();
            }
            break;
        }

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                reshape(event.window.data1, event.window.data2);
                updateCamera();
            }
            break;

        default:
            break;
    }
}

/** Tumble the rock a little further. Accumulated on the left so the axes stay
 *  in world space - a body-space spin would drift into a single axis. */
static void tumble(double dtSeconds) {
    if (!spinning) {
        return;
    }
    Quaternion &rotation = rock->get<Rotation>()->rotation;
    Quaternion step = Quaternion::angleAxis(SPIN_YAW * dtSeconds, Vector3(0, 1, 0))
                      * Quaternion::angleAxis(SPIN_PITCH * dtSeconds, Vector3(1, 0, 0));
    rotation = (step * rotation).normalize();
}

static void printUsage() {
    std::cout <<
        "usage: StarFox_asteroid [flags]\n"
        "\n"
        "Every flag below is also a control on the in-window panel (H hides it).\n"
        "The panel's \"Copy to clipboard\" button emits this same set, filled in\n"
        "with whatever is currently on screen.\n"
        "\n"
        "  geometry\n"
        "    --subdivisions N      icosphere level, 0-6 (default: from --radius)\n"
        "    --radius R            body radius in world units (default 20)\n"
        "    --faceted             per-face normals instead of smooth ones\n"
        "\n"
        "  mesh displacement (DistortionParams)\n"
        "    --amount F            radial fbm displacement, fraction of radius\n"
        "    --frequency F         base fbm frequency\n"
        "    --octaves F           fbm octave count\n"
        "    --gain F              amplitude falloff per fbm octave\n"
        "    --crater-amount F     fraction of lattice cells holding an impact\n"
        "    --crater-size F       mean crater radius of the coarsest generation\n"
        "    --generations F       crater generation count\n"
        "    --crater-falloff F    size falloff per crater generation\n"
        "    --crater-depth F      depth as a fraction of crater radius\n"
        "    --crater-rim F        raised rim height, relative to the bowl\n"
        "    --irregularity F      how far crater outlines and depths wander\n"
        "    --stretch-x/y/z F     non-uniform body scale, applied after the\n"
        "                          displacement - the only knob that stops the\n"
        "                          body being a sphere\n"
        "    --craters-only        fbm off, so the radius is craters and nothing else\n"
        "\n"
        "  shader detail (Material)\n"
        "    --no-detail           switch the per-fragment detail off\n"
        "    --detail-scale F      features per unit of object space\n"
        "    --detail-strength F   how hard the field bends the shading normal\n"
        "    --detail-ridge F      0 = plain fbm, 1 = fully ridged\n"
        "    --detail-albedo F     0 = flat colour, 1 = full procedural rock tone\n"
        "    --albedo-scale F      base frequency of the tonal octaves\n"
        "    --albedo-gain F       amplitude falloff per tonal octave\n"
        "    --albedo-contrast F   how far the tone swings weathered to fresh\n"
        "    --crater-tone F       how hard the baked crater height drives the colour\n"
        "    --tint-r/g/b F        multiplies the finished rock colour\n"
        "    --albedo-only         paint the albedo with the lighting taken away\n"
        "\n"
        "  lighting (three-point rig)\n"
        "    --key F               warm key intensity (default 1)\n"
        "    --fill F              cool fill intensity (default 0.3)\n"
        "    --rim F               cool camera-relative rim intensity (default 1.15)\n"
        "    --ambient F           scene-wide ambient level (default 0.12)\n"
        "\n"
        "  contact sheet\n"
        "    --grid FILE           render one rock per cell from FILE, whose lines\n"
        "                          are name|group|flags. The literal word 'presets'\n"
        "                          renders the game's own AsteroidFactory table\n"
        "                          instead, which is how a transcription slip in it\n"
        "                          gets caught.\n"
        "                          and write a PPM per page.\n"
        "                          One window and one frame for the lot: each cell\n"
        "                          is its own viewport with the same camera and rig,\n"
        "                          so the tiles are directly comparable and the only\n"
        "                          cost per rock is the mesh build. --screenshot sets\n"
        "                          the output stem; pages get -1, -2, ... suffixes.\n"
        "    --grid-cols N         cells across (default 5)\n"
        "    --grid-rows N         cells down (default 4)\n"
        "    --grid-cell N         pixels per cell (default 300)\n"
        "\n"
        "  harness\n"
        "    --yaw F               starting camera orbit, degrees (default 135)\n"
        "    --pitch F             starting camera pitch, degrees (default -18)\n"
        "    --no-tumble           hold the rock still, so --yaw/--pitch fully\n"
        "                          determine the view - what a turntable needs\n"
        "    --wireframe           start in wireframe\n"
        "    --no-panel            start with the parameter panel hidden\n"
        "    --screenshot FILE     write one PPM once the scene has settled\n"
        "    --exit-after SECONDS  quit on a timer (used by the smoke test)\n"
        << std::endl;
}

/**
 * Applies a flag list to `settings`.
 *
 * Split out of main's argv walk so the grid mode can re-apply a different flag
 * line per cell without spawning a process to do it. That is the whole trick
 * behind the grid: one window, one GL context, one frame, and the only cost per
 * rock is the mesh build it was always going to pay.
 */
static void applyArgs(const std::vector<std::string> &argv, double &exitAfterSeconds) {
    DistortionParams &mesh = settings.mesh;
    int argc = (int) argv.size();
    for (int i = 1; i < argc; i++) {
        const std::string &arg = argv[i];
        bool hasValue = (i + 1) < argc;
        if (arg == "--subdivisions" && hasValue) {
            settings.subdivisions = std::atoi(argv[++i].c_str());
        } else if (arg == "--radius" && hasValue) {
            ASTEROID_RADIUS = std::atof(argv[++i].c_str());
        } else if (arg == "--detail-scale" && hasValue) {
            settings.detailScale = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--detail-strength" && hasValue) {
            settings.detailStrength = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--detail-ridge" && hasValue) {
            settings.detailRidge = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--detail-albedo" && hasValue) {
            settings.detailAlbedo = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--albedo-scale" && hasValue) {
            settings.albedoScale = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--albedo-gain" && hasValue) {
            settings.albedoGain = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--albedo-contrast" && hasValue) {
            settings.albedoContrast = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--crater-tone" && hasValue) {
            settings.craterTone = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--albedo-only") {
            settings.albedoOnly = true;
        } else if (arg == "--amount" && hasValue) {
            mesh.amount = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--frequency" && hasValue) {
            mesh.frequency = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--crater-amount" && hasValue) {
            mesh.craterAmount = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--crater-size" && hasValue) {
            mesh.craterSize = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--octaves" && hasValue) {
            mesh.octaves = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--generations" && hasValue) {
            mesh.generations = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--gain" && hasValue) {
            mesh.gain = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--crater-falloff" && hasValue) {
            mesh.craterFalloff = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--crater-depth" && hasValue) {
            mesh.craterDepth = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--crater-rim" && hasValue) {
            mesh.craterRim = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--irregularity" && hasValue) {
            mesh.irregularity = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--stretch-x" && hasValue) {
            mesh.stretchX = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--stretch-y" && hasValue) {
            mesh.stretchY = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--stretch-z" && hasValue) {
            mesh.stretchZ = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--tint-r" && hasValue) {
            settings.rockTint[0] = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--tint-g" && hasValue) {
            settings.rockTint[1] = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--tint-b" && hasValue) {
            settings.rockTint[2] = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--craters-only") {
            CRATERS_ONLY = true;
        } else if (arg == "--no-detail") {
            settings.detailOn = false;
        } else if (arg == "--faceted") {
            settings.smoothShading = false;
        } else if (arg == "--key" && hasValue) {
            settings.keyIntensity = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--fill" && hasValue) {
            settings.fillIntensity = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--rim" && hasValue) {
            settings.rimIntensity = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--ambient" && hasValue) {
            settings.ambientLevel = (float) std::atof(argv[++i].c_str());
        } else if (arg == "--yaw" && hasValue) {
            orbitYaw = std::atof(argv[++i].c_str());
        } else if (arg == "--pitch" && hasValue) {
            orbitPitch = std::atof(argv[++i].c_str());
        } else if (arg == "--no-tumble") {
            spinning = false;
        } else if (arg == "--grid" && hasValue) {
            gridFile = argv[++i];
        } else if (arg == "--grid-cols" && hasValue) {
            gridCols = std::atoi(argv[++i].c_str());
        } else if (arg == "--grid-rows" && hasValue) {
            gridRows = std::atoi(argv[++i].c_str());
        } else if (arg == "--grid-cell" && hasValue) {
            gridCell = std::atoi(argv[++i].c_str());
        } else if (arg == "--wireframe") {
            wireframe = true;
        } else if (arg == "--no-panel") {
            panelVisible = false;
        } else if (arg == "--screenshot" && hasValue) {
            screenshotPath = argv[++i];
        } else if (arg == "--exit-after" && hasValue) {
            exitAfterSeconds = std::atof(argv[++i].c_str());
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            std::exit(EXIT_SUCCESS);
        } else {
            std::cout << "[lab] ignoring unknown argument: " << arg << std::endl;
        }
    }
}

static void parseArgs(int argc, char **argv, double &exitAfterSeconds) {
    std::vector<std::string> args;
    args.reserve(argc);
    for (int i = 0; i < argc; i++) {
        args.emplace_back(argv[i]);
    }
    applyArgs(args, exitAfterSeconds);
}


/**
 * Contact-sheet mode: one rock per cell, one frame for the page.
 *
 * The obvious way to make a sheet of sixty asteroids is to run the lab sixty
 * times and stitch the screenshots. That pays for a process, a window and a GL
 * context per rock, and - because the screenshot waits for the scene to settle
 * and the run then sits out its --exit-after - spends about five seconds each to
 * do a fraction of a second of work.
 *
 * Nothing about that is necessary. A cell is just a viewport. Rebuild the one
 * rock entity with the next set of parameters, point glViewport at the next
 * cell, scissor the clear so it cannot touch the cells already drawn, and render
 * the same scene again. Sixty rocks come out of one frame, the only cost left is
 * the mesh build each one was always going to pay, and because every cell uses
 * the identical camera and rig the tiles are directly comparable - which
 * stitching separate screenshots never quite guarantees.
 */
struct GridEntry {
    std::string name;
    std::string group;
    std::vector<std::string> args; // argv-shaped: [0] is a placeholder
};

/**
 * The game's own preset table, as grid entries.
 *
 * Reading the table rather than re-running the flag lines it was written from is
 * the point: the flag lines prove the numbers were good, and this proves they
 * were transcribed correctly and that the game path builds them. A typo in one
 * digit of AsteroidFactory::presets() is invisible in a diff and obvious here.
 */
static std::vector<GridEntry> presetGrid() {
    std::vector<GridEntry> out;
    for (const AsteroidPreset &p : AsteroidFactory::presets()) {
        GridEntry e;
        e.name = p.name;
        e.group = "preset";
        e.args.emplace_back("lab");
        auto add = [&e](const char *flag, float v) {
            e.args.emplace_back(flag);
            e.args.emplace_back(std::to_string(v));
        };
        add("--stretch-x", p.mesh.stretchX);
        add("--stretch-y", p.mesh.stretchY);
        add("--stretch-z", p.mesh.stretchZ);
        add("--frequency", p.mesh.frequency);
        add("--amount", p.mesh.amount);
        add("--gain", p.mesh.gain);
        add("--octaves", p.mesh.octaves);
        add("--crater-amount", p.mesh.craterAmount);
        add("--crater-size", p.mesh.craterSize);
        add("--generations", p.mesh.generations);
        add("--crater-falloff", p.mesh.craterFalloff);
        add("--crater-rim", p.mesh.craterRim);
        add("--crater-depth", p.mesh.craterDepth);
        add("--irregularity", p.mesh.irregularity);
        add("--detail-scale", p.detailScale);
        add("--detail-strength", p.detailStrength);
        add("--detail-ridge", p.detailRidge);
        out.push_back(std::move(e));
    }
    return out;
}

static std::vector<GridEntry> readGrid(const std::string &path) {
    std::vector<GridEntry> out;
    std::ifstream in(path);
    if (!in) {
        std::cout << "[lab] cannot open grid file: " << path << std::endl;
        return out;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::size_t a = line.find('|');
        std::size_t b = line.find('|', a == std::string::npos ? 0 : a + 1);
        if (a == std::string::npos || b == std::string::npos) continue;

        GridEntry e;
        e.name = line.substr(0, a);
        e.group = line.substr(a + 1, b - a - 1);
        e.args.emplace_back("lab"); // applyArgs skips index 0, as argv does
        std::istringstream flags(line.substr(b + 1));
        std::string tok;
        while (flags >> tok) e.args.push_back(tok);
        out.push_back(std::move(e));
    }
    return out;
}

static void runGrid() {
    std::vector<GridEntry> entries = gridFile == "presets" ? presetGrid() : readGrid(gridFile);
    if (entries.empty()) return;

    const int perPage = gridCols * gridRows;
    const int pages = ((int) entries.size() + perPage - 1) / perPage;
    const std::string stem = screenshotPath.empty() ? "grid.ppm" : screenshotPath;
    const std::string base = stem.substr(0, stem.rfind('.'));

    // Cells are square, so each one frames its rock exactly as the single-rock
    // lab would at a 1:1 window.
    gameModel.resizeScreen(gridCell, gridCell);
    double ignored = 0;
    int built = 0;

    for (int page = 0; page < pages; page++) {
        glDisable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int cell = 0; cell < perPage; cell++) {
            int index = page * perPage + cell;
            if (index >= (int) entries.size()) break;

            applyArgs(entries[index].args, ignored);
            rebuildRock(true);
            applyMaterial();
            reframe();
            updateCamera();
            applyLighting();

            int col = cell % gridCols;
            int row = cell / gridCols;
            // GL's origin is bottom-left; the sheet reads top-left, so the row
            // index is flipped rather than the image afterwards.
            int x = col * gridCell;
            int y = (gridRows - 1 - row) * gridCell;

            glViewport(x, y, gridCell, gridCell);
            glScissor(x, y, gridCell, gridCell);
            glEnable(GL_SCISSOR_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderSystem.update(entities, 0);

            built++;
            std::cout << "[lab] " << built << "/" << entries.size()
                      << "  " << entries[index].name << std::endl;
        }

        glDisable(GL_SCISSOR_TEST);
        std::string out = base + "-" + std::to_string(page + 1) + ".ppm";
        saveScreenshot(out);
        window.swap(); // so the sheet is actually visible while it works
    }
    std::cout << "[lab] grid complete: " << entries.size() << " rocks over "
              << pages << " page(s)" << std::endl;
}

int main(int argc, char **argv) {
    double exitAfterSeconds = 0; // 0 = run until quit. Used by the smoke test.

    // Snapshot before the command line touches anything, so the panel's "reset
    // to defaults" restores the defaults declared in this file rather than the
    // flags this particular run happened to be launched with.
    defaults = settings;

    parseArgs(argc, argv, exitAfterSeconds);

    if (settings.subdivisions < 0) {
        settings.subdivisions = AsteroidFactory::getSubdivisions(ASTEROID_RADIUS);
    }

    printBindings();
    std::cout << "[lab] radius " << ASTEROID_RADIUS
              << ", subdivisions " << settings.subdivisions << std::endl;

    int winW = WINDOW_WIDTH, winH = WINDOW_HEIGHT;
    if (!gridFile.empty()) {
        winW = gridCols * gridCell;
        winH = gridRows * gridCell;
    }
    if (!window.create("Asteroid Lab", winW, winH)) {
        return EXIT_FAILURE;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window.handle(), SDL_GL_GetCurrentContext());
    // GLSL 1.50 is the core-profile dialect that pairs with the GL 3.3 context
    // Window::create asks for, and the only one macOS accepts in a core profile.
    ImGui_ImplOpenGL3_Init("#version 150");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.02f, 0.02f, 0.03f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    materialLibrary.init();

    // Lab-only material override. The game currently ships ASTEROID with its
    // texture disabled for debugging, and its ambient at (1,1,1) - which was
    // fine when a texture was modulating it, but against the white fallback it
    // blows the surface out to flat white and hides exactly the shading this
    // prototype exists to judge. Drop the ambient and give it a rock diffuse so
    // the terminator and the crater walls are actually readable.
    materialLibrary.ASTEROID->setAmbient(0.22, 0.21, 0.20);
    materialLibrary.ASTEROID->setDiffuse(0.74, 0.71, 0.68);

    reshape(winW, winH);
    buildScene();
    printGlErrors("setup");

    if (!gridFile.empty()) {
        runGrid();
        printGlErrors("grid");
        entities.destroyAll();
        gameModel.activeCamera = nullptr;
        window.destroy();
        std::cout << "[lab] clean exit" << std::endl;
        return EXIT_SUCCESS;
    }

    int lastFrame = Time::millis();
    int lastReport = lastFrame;
    int framesSinceReport = 0;
    int totalFrames = 0;
    int startedAt = lastFrame;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // ImGui sees every event first - it is what decides whether the
            // panel or the scene owns the cursor and the keyboard this frame.
            ImGui_ImplSDL2_ProcessEvent(&event);
            handleEvent(event);
        }

        int now = Time::millis();
        double dt = (now - lastFrame) / 1000.0;
        lastFrame = now;
        // A long stall (window drag, a mesh rebuild) shouldn't jump the spin.
        if (dt > 0.1) {
            dt = 0.1;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        drawPanel();
        ImGui::Render();

        tumble(dt);
        // Both are pure uniform writes, so the shader-detail and lighting
        // sliders take effect on the very frame they move.
        applyMaterial();
        applyLighting();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderSystem.update(entities, dt);

        totalFrames++;
        // A few frames in, so the mesh is uploaded and the first frame's stall
        // is behind us. Taken here rather than after the swap so the image is
        // of the rock and not of the panel sitting in front of it.
        if (!screenshotPath.empty() && totalFrames > 10) {
            saveScreenshot(screenshotPath);
            screenshotPath.clear();
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swap();

        framesSinceReport++;
        if (now - lastReport >= 1000) {
            std::cout << "[lab] " << framesSinceReport << " fps ("
                      << (1000.0 / framesSinceReport) << " ms/frame)" << std::endl;
            lastReport = now;
            framesSinceReport = 0;
        }

        if (exitAfterSeconds > 0 && (now - startedAt) / 1000.0 >= exitAfterSeconds) {
            std::cout << "[lab] --exit-after reached, quitting" << std::endl;
            running = false;
        }
    }

    printGlErrors("shutdown");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    entities.destroyAll();
    gameModel.activeCamera = nullptr;
    window.destroy();
    std::cout << "[lab] clean exit" << std::endl;
    return EXIT_SUCCESS;
}
