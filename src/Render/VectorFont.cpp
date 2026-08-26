#include "VectorFont.h"
#include <cctype>
#include <cmath>
#include <unordered_map>

namespace {
    // A point in the glyph cell: x in [0, WIDTH], y in [0, 1] (baseline at 0).
    struct Point {
        double x;
        double y;
    };

    // A run of connected points. A glyph is one or more of these; consecutive
    // points become one line segment each.
    using Stroke = std::vector<Point>;

    struct Glyph {
        double advance; // pen movement to the next character, in cap heights
        std::vector<Stroke> strokes;
    };

    // Narrower than it is tall, the way display faces usually are. Every glyph
    // is drawn in this cell, so the digits are tabular by construction and a
    // changing readout never shuffles the ones beside it.
    const double WIDTH = 0.52;
    const double GAP = 0.16;
    const double ADVANCE = WIDTH + GAP;
    const double MIDX = WIDTH / 2;

    /**
     * Points along an ellipse, `steps` segments from `fromDeg` to `toDeg`.
     * Degrees are the usual convention - 0 at the right, increasing
     * anticlockwise - so a decreasing range sweeps the other way. Round
     * letterforms are built from these rather than from cut corners, which is
     * the difference between a line renderer drawing an O and drawing an
     * octagon.
     */
    Stroke arc(double cx, double cy, double rx, double ry,
               double fromDeg, double toDeg, int steps) {
        Stroke points;
        points.reserve(steps + 1);
        for (int i = 0; i <= steps; i++) {
            double t = fromDeg + (toDeg - fromDeg) * ((double) i / steps);
            double radians = t * M_PI / 180.0;
            points.push_back({cx + rx * std::cos(radians), cy + ry * std::sin(radians)});
        }
        return points;
    }

    Stroke ellipse(double cx, double cy, double rx, double ry, int steps = 18) {
        return arc(cx, cy, rx, ry, 0, 360, steps);
    }

    // Builds one stroke out of pieces, so a letter that runs from a straight
    // stem into a bowl stays a single unbroken path.
    Stroke chain(const std::vector<Stroke> &pieces) {
        Stroke joined;
        for (const Stroke &piece : pieces) {
            for (const Point &point : piece) {
                joined.push_back(point);
            }
        }
        return joined;
    }

    const std::unordered_map<char, Glyph> &table() {
        static const std::unordered_map<char, Glyph> glyphs = {
                {' ', {0.44, {}}},
                {'A', {ADVANCE, {{{0, 0}, {MIDX, 1}, {WIDTH, 0}},
                                 {{0.09, 0.32}, {0.43, 0.32}}}}},
                {'B', {ADVANCE, {{{0, 0}, {0, 1}},
                                 chain({{{0, 1}}, arc(0.14, 0.75, 0.34, 0.25, 90, -90, 6), {{0, 0.5}}}),
                                 chain({{{0, 0.5}}, arc(0.14, 0.25, 0.36, 0.25, 90, -90, 6), {{0, 0}}})}}},
                {'C', {ADVANCE, {arc(MIDX, 0.5, MIDX, 0.5, 52, 308, 13)}}},
                {'D', {ADVANCE, {{{0, 0}, {0, 1}},
                                 chain({{{0, 1}}, arc(0.10, 0.5, 0.42, 0.5, 90, -90, 8), {{0, 0}}})}}},
                {'E', {ADVANCE, {{{WIDTH, 1}, {0, 1}, {0, 0}, {WIDTH, 0}},
                                 {{0, 0.5}, {0.40, 0.5}}}}},
                {'F', {ADVANCE, {{{WIDTH, 1}, {0, 1}, {0, 0}},
                                 {{0, 0.5}, {0.40, 0.5}}}}},
                {'G', {ADVANCE, {chain({arc(MIDX, 0.5, MIDX, 0.5, 52, 308, 13),
                                        {{WIDTH, 0.20}, {WIDTH, 0.44}, {0.30, 0.44}}})}}},
                {'H', {ADVANCE, {{{0, 0}, {0, 1}}, {{WIDTH, 0}, {WIDTH, 1}}, {{0, 0.5}, {WIDTH, 0.5}}}}},
                {'I', {ADVANCE, {{{MIDX, 0}, {MIDX, 1}}, {{0.08, 1}, {0.44, 1}}, {{0.08, 0}, {0.44, 0}}}}},
                {'J', {ADVANCE, {chain({{{WIDTH, 1}, {WIDTH, 0.26}}, arc(MIDX, 0.26, MIDX, 0.26, 0, -180, 7)})}}},
                {'K', {ADVANCE, {{{0, 0}, {0, 1}}, {{WIDTH, 1}, {0.06, 0.42}}, {{0.20, 0.56}, {WIDTH, 0}}}}},
                {'L', {ADVANCE, {{{0, 1}, {0, 0}, {WIDTH, 0}}}}},
                {'M', {ADVANCE, {{{0, 0}, {0, 1}, {MIDX, 0.52}, {WIDTH, 1}, {WIDTH, 0}}}}},
                {'N', {ADVANCE, {{{0, 0}, {0, 1}, {WIDTH, 0}, {WIDTH, 1}}}}},
                {'O', {ADVANCE, {ellipse(MIDX, 0.5, MIDX, 0.5)}}},
                {'P', {ADVANCE, {{{0, 0}, {0, 1}},
                                 chain({{{0, 1}}, arc(0.13, 0.74, 0.35, 0.26, 90, -90, 7), {{0, 0.48}}})}}},
                {'Q', {ADVANCE, {ellipse(MIDX, 0.5, MIDX, 0.5), {{0.32, 0.22}, {WIDTH, 0}}}}},
                {'R', {ADVANCE, {{{0, 0}, {0, 1}},
                                 chain({{{0, 1}}, arc(0.13, 0.74, 0.35, 0.26, 90, -90, 7), {{0, 0.48}}}),
                                 {{0.22, 0.48}, {WIDTH, 0}}}}},
                {'S', {ADVANCE, {{{0.47, 0.85}, {0.40, 0.96}, {MIDX, 1.0}, {0.12, 0.96}, {0.04, 0.85},
                                  {0.05, 0.72}, {0.14, 0.62}, {0.38, 0.50}, {0.48, 0.38}, {0.48, 0.20},
                                  {0.40, 0.06}, {MIDX, 0.0}, {0.12, 0.04}, {0.03, 0.14}}}}},
                {'T', {ADVANCE, {{{0, 1}, {WIDTH, 1}}, {{MIDX, 1}, {MIDX, 0}}}}},
                {'U', {ADVANCE, {chain({{{0, 1}, {0, 0.26}}, arc(MIDX, 0.26, MIDX, 0.26, 180, 360, 8),
                                        {{WIDTH, 1}}})}}},
                {'V', {ADVANCE, {{{0, 1}, {MIDX, 0}, {WIDTH, 1}}}}},
                {'W', {ADVANCE, {{{0, 1}, {0.13, 0}, {MIDX, 0.62}, {0.39, 0}, {WIDTH, 1}}}}},
                {'X', {ADVANCE, {{{0, 0}, {WIDTH, 1}}, {{0, 1}, {WIDTH, 0}}}}},
                {'Y', {ADVANCE, {{{0, 1}, {MIDX, 0.52}, {WIDTH, 1}}, {{MIDX, 0.52}, {MIDX, 0}}}}},
                {'Z', {ADVANCE, {{{0, 1}, {WIDTH, 1}, {0, 0}, {WIDTH, 0}}}}},
                {'0', {ADVANCE, {ellipse(MIDX, 0.5, MIDX, 0.5)}}},
                {'1', {ADVANCE, {{{0.08, 0.80}, {MIDX, 1.0}, {MIDX, 0}}, {{0.08, 0}, {0.44, 0}}}}},
                {'2', {ADVANCE, {chain({arc(MIDX, 0.74, MIDX, 0.26, 160, -20, 8), {{0, 0}, {WIDTH, 0}}})}}},
                {'3', {ADVANCE, {chain({{{0.05, 0.88}}, arc(0.24, 0.76, 0.24, 0.24, 130, -90, 8),
                                        arc(0.24, 0.26, 0.26, 0.26, 90, -140, 9)})}}},
                {'4', {ADVANCE, {{{0.40, 0}, {0.40, 1}, {0.02, 0.30}, {WIDTH, 0.30}}}}},
                {'5', {ADVANCE, {chain({{{0.50, 1}, {0.08, 1}, {0.06, 0.56}},
                                        arc(MIDX, 0.30, MIDX, 0.30, 100, -145, 10)})}}},
                {'6', {ADVANCE, {ellipse(MIDX, 0.26, MIDX, 0.26, 14),
                                 {{0.50, 0.92}, {0.34, 1.0}, {0.16, 0.94}, {0.04, 0.72}, {0, 0.42}}}}},
                {'7', {ADVANCE, {{{0, 1}, {WIDTH, 1}, {0.16, 0}}}}},
                {'8', {ADVANCE, {ellipse(MIDX, 0.74, 0.24, 0.26, 14),
                                 ellipse(MIDX, 0.26, MIDX, 0.26, 14)}}},
                {'9', {ADVANCE, {ellipse(MIDX, 0.74, MIDX, 0.26, 14),
                                 {{WIDTH, 0.74}, {0.50, 0.30}, {0.42, 0.10}, {0.28, 0.0}, {0.10, 0.02}}}}},
                {':', {0.38, {{{0.16, 0.28}, {0.16, 0.40}}, {{0.16, 0.62}, {0.16, 0.74}}}}},
                {'.', {0.38, {{{0.16, 0}, {0.16, 0.10}}}}},
                {'-', {ADVANCE, {{{0.08, 0.5}, {0.44, 0.5}}}}},
                {'/', {ADVANCE, {{{0, 0}, {WIDTH, 1}}}}},
                {'+', {ADVANCE, {{{MIDX, 0.26}, {MIDX, 0.74}}, {{0.03, 0.5}, {0.49, 0.5}}}}},
        };
        return glyphs;
    }
}

double VectorFont::width(const std::string &text, double size, double tracking) {
    double total = 0;
    for (char raw : text) {
        char c = (char) std::toupper((unsigned char) raw);
        auto found = table().find(c);
        total += (found != table().end()) ? found->second.advance : ADVANCE;
        total += tracking;
    }
    // Neither the trailing gap nor the trailing tracking is visible ink.
    if (!text.empty()) {
        total -= GAP + tracking;
    }
    return total * size;
}

void VectorFont::append(std::vector<Vector3> &points, const std::string &text,
                        double x, double y, double size, TextAlignment align, double tracking) {
    double penX = x;
    if (align == TextAlignment::CENTER) {
        penX -= width(text, size, tracking) / 2;
    } else if (align == TextAlignment::RIGHT) {
        penX -= width(text, size, tracking);
    }

    for (char raw : text) {
        char c = (char) std::toupper((unsigned char) raw);
        auto found = table().find(c);
        if (found == table().end()) {
            penX += (ADVANCE + tracking) * size;
            continue;
        }

        const Glyph &glyph = found->second;
        for (const Stroke &stroke : glyph.strokes) {
            for (size_t i = 1; i < stroke.size(); i++) {
                const Point &from = stroke[i - 1];
                const Point &to = stroke[i];
                points.emplace_back(penX + from.x * size, y + from.y * size, 0);
                points.emplace_back(penX + to.x * size, y + to.y * size, 0);
            }
        }
        penX += (glyph.advance + tracking) * size;
    }
}
