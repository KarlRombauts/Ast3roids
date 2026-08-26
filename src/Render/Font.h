#ifndef STARFOX_FONT_H
#define STARFOX_FONT_H

#include <OpenGL.h>
#include <enum/TextAlignment.h>
#include <memory>
#include <string>
#include <vector>

/**
 * A real typeface, rasterised into a glyph atlas at runtime.
 *
 * The font file is read once and kept in memory; an atlas is baked per cap
 * height that gets asked for and cached, so a window resize re-bakes at the new
 * pixel size rather than scaling a blurry texture. Text is emitted as quads for
 * TextMesh in HUD units, with the pen snapped to whole pixels so glyph bitmaps
 * land on the pixel grid.
 *
 * Nothing here touches the ECS or the game: give it a path and a size and it
 * hands back vertices and a texture to bind.
 */
class Font {
public:
    Font();
    ~Font();
    Font(const Font &) = delete;
    Font &operator=(const Font &) = delete;

    // Reads a TTF. False if the file is missing or unparseable, which leaves
    // the font unusable but harmless - callers fall back to drawing text with
    // VectorFont.
    bool load(const std::string &path);
    bool ready() const;

    // Width of `text` in HUD units, for a cap height of `capPixels` drawn at
    // `unitsPerPixel`. `tracking` is extra space per character, in HUD units.
    double width(const std::string &text, int capPixels, double unitsPerPixel, double tracking);

    /**
     * Appends `text` to `vertices` as position + UV triangles in HUD units, and
     * returns the atlas texture to bind (0 if this size could not be baked, in
     * which case nothing was appended). (x, y) is the baseline, placed by
     * `align`.
     */
    GLuint append(std::vector<float> &vertices, const std::string &text,
                  double x, double y, int capPixels, double unitsPerPixel,
                  TextAlignment align = TextAlignment::LEFT, double tracking = 0);

private:
    // Hides stb_truetype (and its baked-glyph structs) from everything that
    // only wants to draw a string.
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif //STARFOX_FONT_H
