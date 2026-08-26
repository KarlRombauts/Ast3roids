#include "HudSystem.h"
#include "../GameModel.h"
#include "../Globals.h"
#include "../Matrix4.h"
#include "../Quaternion.h"
#include "../Components/Asteroid.h"
#include "../Components/Kinematics.h"
#include "../Components/Position.h"
#include "../Components/Rotation.h"
#include "../Components/SpaceShip.h"
#include "../Helpers.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {
    const double MARGIN = 44.0;      // gap from the screen edge to the readouts
    const double TEXT = 28.0;        // cap height of a readout value
    const double LABEL = 19.0;       // cap height of its label
    const double LABEL_GAP = 17.0;

    // Labels are small caps with the letters held apart, values are set tight:
    // the size difference alone was not enough to tell them apart at a glance.
    const double LABEL_TRACKING = 0.16;

    const double BAR_WIDTH = 210.0;  // the thrust bar
    const double BAR_HEIGHT = 13.0;

    const double RETICLE_GAP = 10.0; // clear space at the centre of the crosshair
    const double RETICLE_ARM = 17.0;

    // Where the aiming reticle sits when nothing is locked up: far enough out
    // that it reads as "downrange" rather than as part of the ship.
    const double AIM_RANGE = 60.0;

    // Off-screen contacts are marked on a ring this far from the reticle.
    const double RING_RADIUS = 0.33;

    // Cluttering the ring with the whole wave defeats the point, so only the
    // nearest few off-screen asteroids get a chevron.
    const size_t MAX_CHEVRONS = 14;

    // One contact the HUD has to draw, resolved out of the ECS before any of it
    // is turned into line work.
    struct Contact {
        double distance;
        double x;        // overlay position, if on screen
        double y;
        double radius;   // projected radius in overlay units
        double dirX;     // unit bearing in screen space, for the ring
        double dirY;
        bool onScreen;
        bool behind;
        bool locked;
    };

    std::string padded(int value, int digits) {
        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%0*d", digits, value);
        return buffer;
    }

    double clamp(double value, double min, double max) {
        return value < min ? min : (value > max ? max : value);
    }
}

void HudSystem::drawReadout(const std::string &label, const std::string &value,
                            double x, double y, TextAlignment align) {
    // The label sits on the value's baseline but is smaller, so the number is
    // what the eye lands on.
    if (align == TextAlignment::RIGHT) {
        overlay.text(value, x, y, TEXT, OverlayColour::PRIMARY, TextAlignment::RIGHT);
        double labelRight = x - overlay.textWidth(value, TEXT) - LABEL_GAP;
        overlay.text(label, labelRight, y, LABEL, OverlayColour::DIM,
                     TextAlignment::RIGHT, LABEL_TRACKING);
    } else {
        overlay.text(label, x, y, LABEL, OverlayColour::DIM, TextAlignment::LEFT, LABEL_TRACKING);
        double valueLeft = x + overlay.textWidth(label, LABEL, LABEL_TRACKING) + LABEL_GAP;
        overlay.text(value, valueLeft, y, TEXT, OverlayColour::PRIMARY, TextAlignment::LEFT);
    }
}

void HudSystem::drawReadouts(Entity *ship) {
    double top = overlay.height() - MARGIN - TEXT;

    drawReadout("SCORE", padded(gameModel.score, 6), MARGIN, top, TextAlignment::LEFT);
    drawReadout("WAVE", padded(gameModel.waveCount, 2),
                overlay.width() - MARGIN, top, TextAlignment::RIGHT);

    // Millis into this run. Clamped because a frame can land between the reset
    // and the first gameplay update.
    int elapsed = std::max(0, gameModel.elapsedTime - gameModel.resetTime);
    drawReadout("TIME", formatTime(elapsed), overlay.width() - MARGIN, MARGIN, TextAlignment::RIGHT);

    double speed = (ship != nullptr) ? ship->get<Kinematics>()->velocity.magnitude() : 0;
    drawReadout("SPD", padded((int) std::lround(speed), 3), MARGIN, MARGIN, TextAlignment::LEFT);

    // Thrust, straight off the value that drives the engine glow, so the bar and
    // the exhaust always agree. 0..1, which is why it can have a scale at all -
    // the ship's top speed depends on the frame time.
    double thrust = (ship != nullptr) ? clamp(ship->get<SpaceShip>()->thrust, 0, 1) : 0;
    double barY = MARGIN + TEXT + 20;
    overlay.box(MARGIN + BAR_WIDTH / 2, barY + BAR_HEIGHT / 2,
                BAR_WIDTH / 2, BAR_HEIGHT / 2, OverlayColour::DIM);

    // "Filled" with ticks rather than a solid block, because the only primitive
    // the line pass draws is a line.
    double filled = BAR_WIDTH * thrust;
    for (double offset = 5; offset < filled; offset += 7) {
        overlay.line(MARGIN + offset, barY + 2, MARGIN + offset, barY + BAR_HEIGHT - 2,
                     OverlayColour::PRIMARY);
    }
}

void HudSystem::drawReticle(double x, double y, bool locked) {
    OverlayColour colour = locked ? OverlayColour::LOCK : OverlayColour::PRIMARY;

    overlay.line(x - RETICLE_GAP - RETICLE_ARM, y, x - RETICLE_GAP, y, colour);
    overlay.line(x + RETICLE_GAP, y, x + RETICLE_GAP + RETICLE_ARM, y, colour);
    overlay.line(x, y - RETICLE_GAP - RETICLE_ARM, x, y - RETICLE_GAP, colour);
    overlay.line(x, y + RETICLE_GAP, x, y + RETICLE_GAP + RETICLE_ARM, colour);

    if (locked) {
        // A diamond around the crosshair: "the guns are on something".
        double r = RETICLE_GAP + RETICLE_ARM + 12;
        overlay.line(x - r, y, x, y + r, colour);
        overlay.line(x, y + r, x + r, y, colour);
        overlay.line(x + r, y, x, y - r, colour);
        overlay.line(x, y - r, x - r, y, colour);
    }
}

void HudSystem::drawBracket(double x, double y, double radius, OverlayColour colour) {
    double r = clamp(radius, 11, 260);
    double tick = clamp(r * 0.35, 6, 26);

    // Four corner ticks rather than a closed box, so the rock stays readable
    // through the bracket.
    overlay.line(x - r, y - r, x - r + tick, y - r, colour);
    overlay.line(x - r, y - r, x - r, y - r + tick, colour);
    overlay.line(x + r, y - r, x + r - tick, y - r, colour);
    overlay.line(x + r, y - r, x + r, y - r + tick, colour);
    overlay.line(x - r, y + r, x - r + tick, y + r, colour);
    overlay.line(x - r, y + r, x - r, y + r - tick, colour);
    overlay.line(x + r, y + r, x + r - tick, y + r, colour);
    overlay.line(x + r, y + r, x + r, y + r - tick, colour);
}

void HudSystem::drawChevron(double dirX, double dirY, double size, bool behind,
                            OverlayColour colour) {
    double centreX = overlay.width() / 2;
    double centreY = overlay.height() / 2;
    double ring = overlay.height() * RING_RADIUS;

    // Perpendicular to the bearing, for the chevron's base.
    double perpX = -dirY;
    double perpY = dirX;

    // Pointing outward, away from the reticle: the direction to turn.
    double apexX = centreX + dirX * (ring + size);
    double apexY = centreY + dirY * (ring + size);
    double baseX = centreX + dirX * ring;
    double baseY = centreY + dirY * ring;
    double half = size * 0.62;

    overlay.line(apexX, apexY, baseX + perpX * half, baseY + perpY * half, colour);
    overlay.line(apexX, apexY, baseX - perpX * half, baseY - perpY * half, colour);
    overlay.line(baseX + perpX * half, baseY + perpY * half,
                 baseX - perpX * half, baseY - perpY * half, colour);

    if (!behind) {
        return;
    }

    // A second, inner chevron marks a contact behind the camera, where the
    // bearing says which way to turn rather than where to look.
    double innerApexX = centreX + dirX * (ring - size * 0.35);
    double innerApexY = centreY + dirY * (ring - size * 0.35);
    double innerBaseX = centreX + dirX * (ring - size * 1.15);
    double innerBaseY = centreY + dirY * (ring - size * 1.15);
    double innerHalf = half * 0.62;

    overlay.line(innerApexX, innerApexY,
                 innerBaseX + perpX * innerHalf, innerBaseY + perpY * innerHalf, colour);
    overlay.line(innerApexX, innerApexY,
                 innerBaseX - perpX * innerHalf, innerBaseY - perpY * innerHalf, colour);
}

void HudSystem::drawTargeting(EntityManager &entities, Entity *ship) {
    Vector3 &shipPosition = ship->get<Position>()->position;
    Quaternion &shipRotation = ship->get<Rotation>()->rotation;
    Vector3 &shipVelocity = ship->get<Kinematics>()->velocity;

    // Where the shots actually go, not where the nose points: a bullet is fired
    // with the ship's own velocity added to it (FiringSystem), so while the ship
    // is drifting the barrel direction is a lie.
    Vector3 aim = shipVelocity + Vector3::polar(shipRotation, gameConfig.BULLET_SPEED);
    double aimSpeed = aim.magnitude();
    if (aimSpeed < 0.0001) {
        return;
    }
    Vector3 aimDirection = aim / aimSpeed;

    Matrix4 view = gameModel.viewMatrix();
    Matrix4 &projection = gameModel.projection;

    std::vector<Entity *> asteroids = entities.getEntitiesWith<Asteroid, Position>();

    // Pass one: which asteroid, if any, the aim ray passes through. Nearest
    // wins, since that is the one the shots reach first.
    Entity *lockedTarget = nullptr;
    double lockedDistance = 0;
    for (Entity *asteroid : asteroids) {
        Vector3 &position = asteroid->get<Position>()->position;
        Vector3 toAsteroid = position - shipPosition;
        double distance = toAsteroid.magnitude();
        if (distance < 0.001) {
            continue;
        }

        if (!rayHitsSphere(shipPosition, aimDirection, position, asteroid->get<Asteroid>()->size)) {
            continue;
        }

        if (lockedTarget == nullptr || distance < lockedDistance) {
            lockedTarget = asteroid;
            lockedDistance = distance;
        }
    }

    // Pass two: turn each asteroid into a screen-space contact.
    std::vector<Contact> contacts;
    contacts.reserve(asteroids.size());
    for (Entity *asteroid : asteroids) {
        Vector3 &position = asteroid->get<Position>()->position;
        Vector3 toAsteroid = position - shipPosition;

        Contact contact{};
        contact.distance = toAsteroid.magnitude();
        contact.locked = (asteroid == lockedTarget);

        Vector3 viewSpace = view.transformPoint(position);
        // The camera looks down -z, so anything at or behind z = 0 is behind the
        // viewer, where the perspective divide would mirror it onto the screen
        // instead of dropping it.
        contact.behind = viewSpace.z > -0.5;

        double bearingLength = std::sqrt(viewSpace.x * viewSpace.x + viewSpace.y * viewSpace.y);
        contact.dirX = bearingLength > 0.0001 ? viewSpace.x / bearingLength : 0;
        contact.dirY = bearingLength > 0.0001 ? viewSpace.y / bearingLength : 1;

        if (!contact.behind) {
            Vector3 centre = projection.transformPoint(viewSpace);
            contact.x = (centre.x * 0.5 + 0.5) * overlay.width();
            contact.y = (centre.y * 0.5 + 0.5) * overlay.height();

            // Projecting a point one radius to the camera's right gives the
            // on-screen size without needing the field of view here.
            double radius = asteroid->get<Asteroid>()->size;
            Vector3 edge = projection.transformPoint(
                    Vector3(viewSpace.x + radius, viewSpace.y, viewSpace.z));
            contact.radius = std::fabs((edge.x * 0.5 + 0.5) * overlay.width() - contact.x);

            // Inset, so a bracket half off the edge becomes a chevron instead.
            double inset = 24;
            contact.onScreen = contact.x > inset && contact.x < overlay.width() - inset
                               && contact.y > inset && contact.y < overlay.height() - inset;
        }

        contacts.push_back(contact);
    }

    // Brackets for what is on screen.
    for (const Contact &contact : contacts) {
        if (!contact.onScreen) {
            continue;
        }
        OverlayColour colour = contact.locked
                               ? OverlayColour::LOCK
                               : (contact.distance < gameConfig.WARNING_DISTANCE
                                  ? OverlayColour::WARN : OverlayColour::PRIMARY);
        drawBracket(contact.x, contact.y, contact.radius, colour);
    }

    // Chevrons for what is not, nearest first and capped so the ring stays
    // readable when a wave is large.
    std::vector<const Contact *> offScreen;
    for (const Contact &contact : contacts) {
        if (!contact.onScreen) {
            offScreen.push_back(&contact);
        }
    }
    std::sort(offScreen.begin(), offScreen.end(), [](const Contact *a, const Contact *b) {
        return a->distance < b->distance;
    });
    if (offScreen.size() > MAX_CHEVRONS) {
        offScreen.resize(MAX_CHEVRONS);
    }
    for (const Contact *contact : offScreen) {
        OverlayColour colour = contact->distance < gameConfig.WARNING_DISTANCE
                               ? OverlayColour::WARN : OverlayColour::DIM;
        // Closer contacts get a bigger marker, so the ring reads as a threat
        // display rather than a list.
        double size = 15 + 13 * clamp((150 - contact->distance) / 150, 0, 1);
        drawChevron(contact->dirX, contact->dirY, size, contact->behind, colour);
    }

    // The reticle goes on the locked target's range when there is one, so it
    // sits on the rock instead of floating in front of it.
    double range = lockedTarget != nullptr ? lockedDistance : AIM_RANGE;
    Vector3 aimPoint = shipPosition + aimDirection * range;
    Vector3 aimView = view.transformPoint(aimPoint);
    if (aimView.z < -0.5) {
        Vector3 aimClip = projection.transformPoint(aimView);
        drawReticle((aimClip.x * 0.5 + 0.5) * overlay.width(),
                    (aimClip.y * 0.5 + 0.5) * overlay.height(),
                    lockedTarget != nullptr);
    }
}

void HudSystem::update(EntityManager &entities, double dt) {
    // The menu screens have a camera of their own now - they sit over an
    // attract-mode asteroid field - so it is the state that says whether there
    // is a run in progress to put instruments on.
    bool inFlight = gameModel.state == GameState::PLAYING
                    || gameModel.state == GameState::WAVE_OVER
                    || gameModel.state == GameState::GAME_OVER_TRANSITION;
    if (!inFlight || gameModel.activeCamera == nullptr) {
        return;
    }

    overlay.begin();

    std::vector<Entity *> ships =
            entities.getEntitiesWith<SpaceShip, Position, Rotation, Kinematics>();
    Entity *ship = ships.empty() ? nullptr : ships[0];

    drawReadouts(ship);
    if (ship != nullptr) {
        drawTargeting(entities, ship);
    }

    overlay.end();
}
