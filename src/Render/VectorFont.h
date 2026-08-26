#ifndef STARFOX_VECTORFONT_H
#define STARFOX_VECTORFONT_H

#include <Vector3.h>
#include <enum/TextAlignment.h>
#include <string>
#include <vector>

/**
 * A stroke font: every glyph is a handful of line segments rather than pixels
 * in a texture. The HUD therefore needs no font atlas, no new asset and no
 * second shader - the text goes down the same unlit GL_LINES path as the
 * arena's wireframe walls, and stays sharp at any resolution.
 *
 * Glyphs are defined in a cell one unit tall (the cap height) and a little over
 * half that wide, with the origin at the baseline's left edge. Round shapes are
 * real arcs rather than cut corners. Nothing descends below the baseline, so
 * `y` is also the bottom of the line, and every glyph is the same width, so
 * digits are tabular and a changing readout never shifts.
 */
namespace VectorFont {
    // Width of `text` when drawn at `size` (the cap height), in those units.
    // `tracking` is extra space per character, also in cap heights.
    double width(const std::string &text, double size, double tracking = 0);

    /**
     * Appends `text` to `points` as GL_LINES vertex pairs on the z = 0 plane.
     * (x, y) is the baseline, positioned by `align`. Lower case is drawn as
     * upper case; characters with no glyph advance without drawing.
     */
    void append(std::vector<Vector3> &points, const std::string &text,
                double x, double y, double size,
                TextAlignment align = TextAlignment::LEFT,
                double tracking = 0);
}

#endif //STARFOX_VECTORFONT_H
