#ifndef STARFOX_MENUSYSTEM_H
#define STARFOX_MENUSYSTEM_H

#include "System.h"
#include "../Render/Overlay.h"
#include <string>
#include <vector>

/**
 * The screens either side of a run: the title screen, and the one that shows
 * how the run went. Both list the controls, and both are drawn through the same
 * Overlay the in-flight HUD uses, so they need no assets of their own and look
 * like part of the same instrument.
 *
 * The screens are not drawn on black: ensureScene() builds an attract-mode
 * asteroid field for them to sit over, and orbitScene() flies the camera slowly
 * around it. main tears that down when a run starts.
 *
 * Nothing here reads input. The menus say which key starts a game; main's state
 * machine is what watches for it.
 */
class MenuSystem : public System {
public:
    void update(EntityManager &entities, double dt) override;

    /**
     * Builds the attract-mode backdrop, at most one entity per call. Spreading
     * it out matters: generating an asteroid mesh takes long enough to be seen
     * as a stall, and the menu should appear immediately.
     */
    void ensureScene(EntityManager &entities);

    // Advances the camera's orbit. `dt` is in milliseconds.
    void orbitScene(double dt);

    // Forgets the backdrop, for when main destroys the world to start a run.
    void sceneTornDown();

private:
    struct Control {
        std::string key;
        std::string action;
    };

    /**
     * The box the menu is laid out inside. It is centred and bounded rather
     * than being the whole screen: on a wide display, corner brackets pinned to
     * the screen edges end up a long way from the text they are supposed to
     * frame, and the content reads as a thin column adrift in the middle.
     */
    struct Panel {
        double left;
        double right;
        double bottom;
        double top;
        double centreX;

        double width() const { return right - left; }
    };

    Panel panel() const;

    void drawTitleScreen();
    void drawEndScreen();

    // Corner brackets inset from the screen edge, the same shape the HUD puts
    // around a target - it frames the menu as part of the same instrument.
    void drawFrame(const Panel &panel);

    // Centred heading with a rule running out to either side.
    void drawHeading(const Panel &panel, const std::string &text, double y);

    // A right-aligned dim label and a left-aligned bright value, meeting in the
    // middle of the screen: the shape both the stats and the controls use.
    void drawRow(const std::string &label, const std::string &value, double y,
                 double labelSize, double valueSize,
                 OverlayColour valueColour = OverlayColour::PRIMARY);

    void drawControls(const Panel &panel, double top);

    // Shrinks `size` until the string fits between the frame's corners, so a
    // narrow window clips nothing.
    double fitSize(const Panel &panel, const std::string &text, double size,
                   double tracking) const;

    // Which controls to list. The web build tells the game when it is being
    // played on a phone (web_set_touch), because the controls are not the same.
    std::vector<Control> controls() const;

    int rocks = 0;              // how much of the attract field exists yet
    double orbitAngle = 0;      // degrees around the field
};

#endif //STARFOX_MENUSYSTEM_H
