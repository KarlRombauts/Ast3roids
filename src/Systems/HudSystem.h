#ifndef STARFOX_HUDSYSTEM_H
#define STARFOX_HUDSYSTEM_H

#include "System.h"
#include "../Render/Overlay.h"
#include <string>

/**
 * The in-flight heads-up display: score, wave, clock and speed readouts, a
 * thrust bar, an aiming reticle, a bracket around every asteroid on screen and
 * a ring of chevrons for the ones that are not.
 *
 * All of it goes through Overlay, which owns the 2D pass, the palette and the
 * font. MenuSystem draws the screens either side of a run through the same one.
 */
class HudSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;

private:
    // A dim label with a bright value beside it ("SCORE 001200"), anchored left
    // or right at (x, y).
    void drawReadout(const std::string &label, const std::string &value,
                     double x, double y, TextAlignment align);

    void drawReadouts(Entity *ship);
    void drawTargeting(EntityManager &entities, Entity *ship);
    void drawReticle(double x, double y, bool locked);
    void drawBracket(double x, double y, double radius, OverlayColour colour);

    /**
     * A marker on the ring around the reticle for a contact that is off screen.
     * (dirX, dirY) is the unit bearing to it in screen space; `behind` doubles
     * the chevron, which is how the HUD says "this one is behind you".
     */
    void drawChevron(double dirX, double dirY, double size, bool behind, OverlayColour colour);
};

#endif //STARFOX_HUDSYSTEM_H
