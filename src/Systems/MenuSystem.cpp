#include "MenuSystem.h"
#include "../GameModel.h"
#include "../Globals.h"
#include "../Helpers.h"
#include "../Quaternion.h"
#include "../Components/Camera.h"
#include "../Components/Kinematics.h"
#include "../Components/Light.h"
#include "../Components/Position.h"
#include "../Components/Rotation.h"
#include "../Components/Skybox.h"
#include "../Factory/AsteroidFactory.h"
#include "../Helpers/ObjParser.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>

namespace {
    const double MARGIN = 44.0;

    const double TITLE = 84.0;
    const double TITLE_TRACKING = 0.22;
    const double PROMPT = 26.0;
    const double SUBTITLE = 18.0;
    const double HEADING = 17.0;
    const double HEADING_TRACKING = 0.34;

    const double STAT_VALUE = 32.0;
    const double STAT_LABEL = 19.0;
    const double STAT_SPACING = 48.0;

    const double CONTROL_KEY = 21.0;
    const double CONTROL_SPACING = 42.0;

    // Gap either side of the centre line, where labels end and values begin.
    const double COLUMN_GAP = 22.0;

    // The layout box. Width is capped so the frame stays around the text on a
    // wide screen instead of retreating into the corners; the vertical bounds
    // give the block room to breathe without drifting apart on a tall one.
    const double PANEL_MAX_HALF_WIDTH = 480.0;
    const double PANEL_TOP = 906.0;
    const double PANEL_BOTTOM = 94.0;

    // The attract-mode backdrop: rocks tumbling in a band around the origin,
    // with the camera walking slowly around the outside of them.
    const int ATTRACT_ROCKS = 9;
    const double ATTRACT_MIN_RANGE = 25.0;  // the belt's inner and outer edge
    const double ATTRACT_MAX_RANGE = 70.0;
    const double ATTRACT_BAND = 14.0;       // how far out of the belt's plane a rock strays
    const double ATTRACT_MIN_SPIN = 5.0;    // degrees per second
    const double ATTRACT_MAX_SPIN = 16.0;
    const double ORBIT_RADIUS = 150.0;  // outside the belt, so nothing fills the frame
    const double ORBIT_SPEED = 4.5;     // degrees per second
    const double ORBIT_HEIGHT = 46.0;   // above the belt, looking down across it
    const double ORBIT_BOB = 10.0;      // how far the camera rises and falls

    // The camera aims at a point above the belt, which puts the belt below the
    // middle of the frame and leaves the title and the prompt against clear sky.
    const double LOOK_HEIGHT = 34.0;

    std::string padded(int value, int digits) {
        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%0*d", digits, value);
        return buffer;
    }

    // How a bound key should read on screen.
    std::string keyName(char key) {
        if (key == ' ') {
            return "SPACE";
        }
        return std::string(1, (char) std::toupper((unsigned char) key));
    }
}

std::vector<MenuSystem::Control> MenuSystem::controls() const {
    if (gameModel.touchControls) {
        // Mirrors the phone control scheme in web/shell.html: the ship thrusts
        // by itself, and steering is where the phone points.
        return {
                {"AIM",   "STEER"},
                {"TILT",  "ROLL"},
                {"TAP",   "FIRE"},
                {"SLIDE", "CAMERA"},
                {"AUTO",  "THRUST"},
        };
    }

    std::vector<Control> keyboard = {
            {keyName(gameConfig.PLAYER_FORWARD),                                     "THRUST"},
            {keyName(gameConfig.PLAYER_LEFT) + " / " + keyName(gameConfig.PLAYER_RIGHT), "ROLL"},
            {"MOUSE",                                                                "AIM"},
            {keyName(gameConfig.PLAYER_SHOOT),                                       "FIRE"},
            // Not in GameConfig: PlayerInputSystem reads these two directly.
            {"I / K",                                                                "CAMERA"},
    };
#ifndef __EMSCRIPTEN__
    keyboard.push_back({"ESC", "QUIT"}); // the browser has no window to close
#endif
    return keyboard;
}

MenuSystem::Panel MenuSystem::panel() const {
    Panel box{};
    box.centreX = overlay.width() / 2;

    double halfWidth = std::min((overlay.width() - 2 * MARGIN) / 2, PANEL_MAX_HALF_WIDTH);
    box.left = box.centreX - halfWidth;
    box.right = box.centreX + halfWidth;

    box.top = std::min(overlay.height() - MARGIN, PANEL_TOP);
    box.bottom = std::max(MARGIN, PANEL_BOTTOM);
    return box;
}

void MenuSystem::sceneTornDown() {
    rocks = 0;
}

void MenuSystem::ensureScene(EntityManager &entities) {
    // No camera means no backdrop yet (main clears it when it tears the world
    // down). Build the fixed parts first, then one rock per frame.
    if (gameModel.activeCamera == nullptr) {
        Entity *skybox = entities.create();
        skybox->assign<Skybox>();
        skybox->assign<Geometry>(ObjParser().parse(gameConfig.MODEL_DIR + "/skybox/skybox.obj"));

        // The same key light the world uses: the sun that is visible in the
        // skybox, far off in -z, so the rocks are lit from where it looks like
        // they should be.
        Entity *keyLight = entities.create();
        keyLight->assign<Position>(Vector3(0, 150, -1500));
        keyLight->assign<Light>(1.05, 1.0, 0.9);

        // No arena: its wireframe cage is a gameplay boundary, and from out
        // here it would sit between the camera and the field.
        Entity *camera = entities.create();
        camera->assign<Camera>(60, gameModel.aspectRatio, 1, 1000);
        camera->assign<Position>(Vector3(ORBIT_RADIUS, ORBIT_HEIGHT, 0));
        camera->assign<Rotation>();
        gameModel.activeCamera = camera;

        rocks = 0;
        orbitAngle = randf(0, 360); // a different view of the field each time
        return;
    }

    if (rocks >= ATTRACT_ROCKS) {
        return;
    }

    // Smaller than the ones a wave throws at you: these are scenery, and want
    // to read as a field rather than fill the frame.
    Entity *asteroid = AsteroidFactory::create(entities, randInt(6, 14));

    // A belt, not a scatter. Each rock takes its own slice of the circle (with
    // enough jitter that the spacing does not look measured), so the field stays
    // spread out as the camera comes round instead of clumping into one corner
    // and leaving the other empty. Flattening it into a band also keeps the
    // rocks near the camera's own height, where they are actually in shot.
    double slice = 360.0 / ATTRACT_ROCKS;
    double degrees = rocks * slice + randf(-slice * 0.3, slice * 0.3);
    double radians = degrees * M_PI / 180;
    double range = randf(ATTRACT_MIN_RANGE, ATTRACT_MAX_RANGE);
    asteroid->assign<Position>(Vector3(std::cos(radians) * range,
                                       randf(-ATTRACT_BAND, ATTRACT_BAND),
                                       std::sin(radians) * range));

    Kinematics *kinematics = asteroid->get<Kinematics>();

    // Tumbling on the spot. Drift would empty the frame after a minute on the
    // title screen, and there is no OutOfBoundsSystem running to catch them.
    kinematics->velocity = Vector3();

    // Far slower than in play, where a rock can spin at 180 degrees a second.
    // At that rate scenery reads as a hazard; this is meant to idle.
    kinematics->angularVelocity = Vector3::random(randf(ATTRACT_MIN_SPIN, ATTRACT_MAX_SPIN));

    rocks++;
}

void MenuSystem::orbitScene(double dt) {
    Entity *camera = gameModel.activeCamera;
    if (camera == nullptr) {
        return;
    }

    orbitAngle += ORBIT_SPEED * dt / 1000;
    if (orbitAngle > 360) {
        orbitAngle -= 360;
    }

    double radians = orbitAngle * M_PI / 180;
    Vector3 eye(std::cos(radians) * ORBIT_RADIUS,
                ORBIT_HEIGHT + std::sin(radians * 2) * ORBIT_BOB,
                std::sin(radians) * ORBIT_RADIUS);
    camera->get<Position>()->position = eye;

    // The camera looks down its own -z, so +z has to point back along the line
    // from what it is aimed at for the field to be in front of it.
    Vector3 target(0, LOOK_HEIGHT, 0);
    camera->get<Rotation>()->rotation = Quaternion::lookRotation(eye - target, Vector3::up());
}

double MenuSystem::fitSize(const Panel &panel, const std::string &text, double size,
                           double tracking) const {
    double available = panel.width() - 2 * MARGIN;
    double width = overlay.textWidth(text, size, tracking);
    if (width <= 0 || width <= available) {
        return size;
    }
    // Text width is linear in the cap height, so one division lands it.
    return size * available / width;
}

void MenuSystem::drawFrame(const Panel &panel) {
    // Ticks sized off the panel, so the corners read the same at any aspect.
    double tick = std::min(panel.width(), panel.top - panel.bottom) * 0.10;

    overlay.line(panel.left, panel.bottom, panel.left + tick, panel.bottom, OverlayColour::DIM);
    overlay.line(panel.left, panel.bottom, panel.left, panel.bottom + tick, OverlayColour::DIM);
    overlay.line(panel.right, panel.bottom, panel.right - tick, panel.bottom, OverlayColour::DIM);
    overlay.line(panel.right, panel.bottom, panel.right, panel.bottom + tick, OverlayColour::DIM);
    overlay.line(panel.left, panel.top, panel.left + tick, panel.top, OverlayColour::DIM);
    overlay.line(panel.left, panel.top, panel.left, panel.top - tick, OverlayColour::DIM);
    overlay.line(panel.right, panel.top, panel.right - tick, panel.top, OverlayColour::DIM);
    overlay.line(panel.right, panel.top, panel.right, panel.top - tick, OverlayColour::DIM);
}

void MenuSystem::drawHeading(const Panel &panel, const std::string &text, double y) {
    overlay.text(text, panel.centreX, y, HEADING, OverlayColour::DIM,
                 TextAlignment::CENTER, HEADING_TRACKING);

    // Rules running out from the heading to just inside the panel, which ties
    // the frame to the content instead of leaving it stranded at the edges.
    double half = overlay.textWidth(text, HEADING, HEADING_TRACKING) / 2;
    double rule = y + HEADING * 0.4;
    double inner = half + 26;
    double outer = panel.width() / 2 - 34;
    if (outer > inner) {
        overlay.line(panel.centreX - outer, rule, panel.centreX - inner, rule, OverlayColour::DIM);
        overlay.line(panel.centreX + inner, rule, panel.centreX + outer, rule, OverlayColour::DIM);
    }
}

void MenuSystem::drawRow(const std::string &label, const std::string &value, double y,
                         double labelSize, double valueSize, OverlayColour valueColour) {
    double centre = overlay.width() / 2;
    overlay.text(label, centre - COLUMN_GAP, y, labelSize, OverlayColour::DIM,
                 TextAlignment::RIGHT, 0.10);
    overlay.text(value, centre + COLUMN_GAP, y, valueSize, valueColour, TextAlignment::LEFT);
}

void MenuSystem::drawControls(const Panel &panel, double top) {
    drawHeading(panel, "CONTROLS", top);

    double y = top - 60;
    for (const Control &control : controls()) {
        // Key on the left of the centre line, what it does on the right - so
        // the reader scans one column, not a table.
        drawRow(control.key, control.action, y, CONTROL_KEY, CONTROL_KEY);
        y -= CONTROL_SPACING;
    }
}

void MenuSystem::drawTitleScreen() {
    Panel box = panel();
    drawFrame(box);

    overlay.text("AST3ROIDS", box.centreX, 716,
                 fitSize(box, "AST3ROIDS", TITLE, TITLE_TRACKING),
                 OverlayColour::BRIGHT, TextAlignment::CENTER, TITLE_TRACKING);

    // What it is built out of. Fills the space under the title, which on a big
    // screen was reading as a hole rather than as breathing room. The font is
    // subset to ASCII, so the separator is a slash and not a middot.
#ifdef __EMSCRIPTEN__
    const char *subtitle = "C++ / WEBGL2 / WEBASSEMBLY";
#else
    const char *subtitle = "C++ / OPENGL 3.3";
#endif
    overlay.text(subtitle, box.centreX, 658, fitSize(box, subtitle, SUBTITLE, 0.30),
                 OverlayColour::DIM, TextAlignment::CENTER, 0.30);

    std::string prompt = gameModel.touchControls ? "TAP PLAY TO START" : "PRESS SPACE TO START";
    overlay.text(prompt, box.centreX, 580, fitSize(box, prompt, PROMPT, 0.12),
                 OverlayColour::PRIMARY, TextAlignment::CENTER, 0.12);

    if (gameModel.bestScore > 0) {
        drawRow("BEST", padded(gameModel.bestScore, 6), 534, STAT_LABEL, CONTROL_KEY);
    }

    drawControls(box, 494);
}

void MenuSystem::drawEndScreen() {
    Panel box = panel();
    drawFrame(box);

    overlay.text("GAME OVER", box.centreX, 780,
                 fitSize(box, "GAME OVER", TITLE, TITLE_TRACKING),
                 OverlayColour::WARN, TextAlignment::CENTER, TITLE_TRACKING);

    // The run that just ended: gameModel keeps these until the next reset.
    double y = 670;
    drawRow("SCORE", padded(gameModel.score, 6), y, STAT_LABEL, STAT_VALUE);
    y -= STAT_SPACING;
    drawRow("WAVE", padded(gameModel.waveCount, 2), y, STAT_LABEL, STAT_VALUE);
    y -= STAT_SPACING;
    drawRow("TIME", formatTime(std::max(0, gameModel.elapsedTime - gameModel.resetTime)),
            y, STAT_LABEL, STAT_VALUE);
    y -= STAT_SPACING;

    // Green when this run is the one that set it - the only congratulation the
    // screen offers.
    bool newBest = gameModel.score > 0 && gameModel.score >= gameModel.bestScore;
    drawRow("BEST", padded(gameModel.bestScore, 6), y, STAT_LABEL, STAT_VALUE,
            newBest ? OverlayColour::LOCK : OverlayColour::PRIMARY);

    std::string prompt = gameModel.touchControls ? "TAP PLAY TO PLAY AGAIN"
                                                 : "PRESS SPACE TO PLAY AGAIN";
    overlay.text(prompt, box.centreX, 452, fitSize(box, prompt, PROMPT, 0.12),
                 OverlayColour::BRIGHT, TextAlignment::CENTER, 0.12);

    drawControls(box, 372);
}

void MenuSystem::update(EntityManager &entities, double dt) {
    // GAME_OVER lasts a single frame (main tears the world down in it) and hands
    // over to PLAY_AGAIN, so both draw the same screen.
    bool ended = gameModel.state == GameState::PLAY_AGAIN
                 || gameModel.state == GameState::GAME_OVER;
    if (gameModel.state != GameState::START && !ended) {
        return;
    }

    overlay.begin();
    overlay.shade();   // the attract field is behind this; the text has to win
    if (ended) {
        drawEndScreen();
    } else {
        drawTitleScreen();
    }
    overlay.end();
}
