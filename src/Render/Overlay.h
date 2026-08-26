#ifndef STARFOX_OVERLAY_H
#define STARFOX_OVERLAY_H

#include "Shader.h"
#include "LineMesh.h"
#include "TextMesh.h"
#include "Font.h"
#include "../Vector3.h"
#include "../enum/TextAlignment.h"
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * The overlay's palette. Colour is a uniform here rather than a vertex
 * attribute, so everything drawn in one colour collapses into one draw call.
 */
enum class OverlayColour {
    PRIMARY,   // the instrument colour: readouts, reticle, brackets
    DIM,       // labels, rules, contacts too far off to matter
    BRIGHT,    // titles, and the one line the player should read first
    LOCK,      // the target the guns are pointing at
    WARN,      // close enough to matter
};

/**
 * The 2D pass everything in front of the world is drawn in - the in-flight HUD
 * and the menu screens both build their frame out of these calls.
 *
 * Coordinates are overlay units: origin bottom-left, HEIGHT tall and
 * HEIGHT * aspect wide, so a size means the same fraction of the screen at any
 * window size or device pixel ratio. Lines go through the same unlit shader
 * branch as the arena's wireframe walls; text is quads out of a glyph atlas
 * baked from a real typeface, and falls back to stroke letterforms if the font
 * file cannot be read.
 *
 * Call begin(), then any number of line()/box()/text(), then end().
 */
class Overlay {
public:
    // Height of the coordinate space. Width follows the aspect ratio.
    static const double HEIGHT;

    // Sizes the space to the window and clears the last frame's geometry.
    void begin();

    // Uploads and draws it: in front of everything, writing no depth.
    void end();

    double width() const { return spaceWidth; }
    double height() const { return HEIGHT; }

    /**
     * Dims everything behind the overlay by drawing one dark, translucent quad
     * over the whole screen. The menus use it so their text reads over the
     * attract-mode scene; the in-flight HUD never does.
     */
    void shade();

    void line(double x1, double y1, double x2, double y2, OverlayColour colour);

    // An axis-aligned rectangle outline, centred on (x, y).
    void box(double x, double y, double halfWidth, double halfHeight, OverlayColour colour);

    /**
     * Draws `text` with its baseline at (x, y), positioned by `align`. `size` is
     * the cap height and `tracking` is extra space per character as a fraction
     * of it.
     */
    void text(const std::string &string, double x, double y, double size, OverlayColour colour,
              TextAlignment align = TextAlignment::LEFT, double tracking = 0);

    // Width of the same string, in overlay units.
    double textWidth(const std::string &string, double size, double tracking = 0);

private:
    static const int COLOUR_COUNT = 5;

    struct LineBatch {
        std::vector<Vector3> points;
        LineMesh mesh;
    };

    // One per (pixel size, colour): a batch can only bind one atlas and set one
    // colour, and the sizes in use are few.
    struct TextBatch {
        std::vector<float> vertices;
        TextMesh mesh;
        GLuint texture = 0;
        OverlayColour colour = OverlayColour::PRIMARY;
    };

    void ensureInitialised();
    void drawShade();
    void setColour(OverlayColour colour);
    int capPixels(double size) const;
    TextBatch &textBatch(int capPixels, OverlayColour colour);

    bool initialised = false;
    Shader shader;
    Font font;
    bool fontLoaded = false;
    bool useFont = false;   // re-checked per frame; false means the stroke fallback

    double spaceWidth = HEIGHT;
    double unitsPerPixel = 1;

    LineBatch lines[COLOUR_COUNT];
    std::map<std::pair<int, int>, std::unique_ptr<TextBatch>> texts;

    // The backdrop dimmer: a 1x1 white texture with a fixed alpha (the shader
    // takes its alpha from the texture, so the level is baked in there) and one
    // quad to stretch it over.
    GLuint shadeTexture = 0;
    TextMesh shadeMesh;
    bool shadeRequested = false;
};

extern Overlay overlay;

#endif //STARFOX_OVERLAY_H
