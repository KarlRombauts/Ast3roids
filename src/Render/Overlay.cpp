#include "Overlay.h"
#include "VectorFont.h"
#include "../OpenGL.h"
#include "../GameModel.h"
#include "../Globals.h"
#include "../Matrix4.h"
#include <cmath>

Overlay overlay;

const double Overlay::HEIGHT = 1000.0;

namespace {
    // How dark the backdrop dimmer is, and how much of the scene it keeps.
    const unsigned char SHADE_ALPHA = 168;
    const float SHADE_COLOUR[3] = {0.02f, 0.03f, 0.05f};

    // Instrument cyan, cool enough to sit against the warm key light without
    // competing with the explosions.
    const float PALETTE[5][3] = {
            {0.55f, 0.87f, 1.00f}, // PRIMARY
            {0.34f, 0.53f, 0.64f}, // DIM
            {0.90f, 0.97f, 1.00f}, // BRIGHT
            {0.40f, 1.00f, 0.50f}, // LOCK
            {1.00f, 0.48f, 0.28f}, // WARN
    };
}

void Overlay::ensureInitialised() {
    if (initialised) {
        return;
    }

    // The same shader the world is drawn with. Its unlit branch multiplies a
    // texture by uColor and keeps the texture's alpha, which covers both the
    // untextured line work and the glyph atlas, so there is no second program
    // to compile.
    shader.loadFromFiles("Shaders/basic.vert", "Shaders/basic.frag");

    // JetBrains Mono, subset to printable ASCII. Missing or unreadable is not
    // fatal: text falls back to VectorFont's stroke letterforms.
    fontLoaded = font.load(gameConfig.FONT_DIR + "/JetBrainsMono-Medium.ttf");

    unsigned char shade[4] = {255, 255, 255, SHADE_ALPHA};
    glGenTextures(1, &shadeTexture);
    glBindTexture(GL_TEXTURE_2D, shadeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, shade);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    initialised = true;
}

void Overlay::begin() {
    ensureInitialised();

    spaceWidth = HEIGHT * (gameModel.aspectRatio > 0 ? gameModel.aspectRatio : 1.0);
    unitsPerPixel = gameModel.height > 0 ? HEIGHT / gameModel.height : 1.0;

    // Asking for a width bakes this frame's atlas, so a zero means the atlas
    // could not be built (or the window is so small the glyphs would be
    // unreadable) and the stroke font takes over for the whole frame rather
    // than half a screen.
    useFont = fontLoaded && font.width("0", capPixels(24), unitsPerPixel, 0) > 0;

    shadeRequested = false;
    for (LineBatch &batch : lines) {
        batch.points.clear();
    }
    for (auto &entry : texts) {
        entry.second->vertices.clear();
    }
}

int Overlay::capPixels(double size) const {
    return (int) std::lround(size / unitsPerPixel);
}

void Overlay::setColour(OverlayColour colour) {
    const float *rgb = PALETTE[(int) colour];
    shader.setVec3("uColor", rgb[0], rgb[1], rgb[2]);
}

void Overlay::shade() {
    shadeRequested = true;
}

void Overlay::drawShade() {
    // One quad over the whole space. Every corner samples the same texel, so the
    // UVs are all the same.
    const float quad[6][5] = {
            {0,                  0,                   0, 0.5f, 0.5f},
            {(float) spaceWidth, 0,                   0, 0.5f, 0.5f},
            {(float) spaceWidth, (float) HEIGHT,      0, 0.5f, 0.5f},
            {0,                  0,                   0, 0.5f, 0.5f},
            {(float) spaceWidth, (float) HEIGHT,      0, 0.5f, 0.5f},
            {0,                  (float) HEIGHT,      0, 0.5f, 0.5f},
    };
    std::vector<float> vertices;
    for (const auto &vertex : quad) {
        vertices.insert(vertices.end(), vertex, vertex + 5);
    }

    shader.setVec3("uColor", SHADE_COLOUR[0], SHADE_COLOUR[1], SHADE_COLOUR[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadeTexture);
    shadeMesh.upload(vertices);
    shadeMesh.draw();
}

void Overlay::line(double x1, double y1, double x2, double y2, OverlayColour colour) {
    LineBatch &batch = lines[(int) colour];
    batch.points.emplace_back(x1, y1, 0);
    batch.points.emplace_back(x2, y2, 0);
}

void Overlay::box(double x, double y, double halfWidth, double halfHeight, OverlayColour colour) {
    line(x - halfWidth, y - halfHeight, x + halfWidth, y - halfHeight, colour);
    line(x + halfWidth, y - halfHeight, x + halfWidth, y + halfHeight, colour);
    line(x + halfWidth, y + halfHeight, x - halfWidth, y + halfHeight, colour);
    line(x - halfWidth, y + halfHeight, x - halfWidth, y - halfHeight, colour);
}

Overlay::TextBatch &Overlay::textBatch(int capPixels, OverlayColour colour) {
    std::pair<int, int> key(capPixels, (int) colour);
    auto found = texts.find(key);
    if (found == texts.end()) {
        found = texts.emplace(key, std::unique_ptr<TextBatch>(new TextBatch())).first;
        found->second->colour = colour;
    }
    return *found->second;
}

void Overlay::text(const std::string &string, double x, double y, double size, OverlayColour colour,
                   TextAlignment align, double tracking) {
    if (!useFont) {
        VectorFont::append(lines[(int) colour].points, string, x, y, size, align, tracking);
        return;
    }

    int cap = capPixels(size);
    TextBatch &batch = textBatch(cap, colour);
    GLuint texture = font.append(batch.vertices, string, x, y, cap, unitsPerPixel,
                                 align, tracking * size);
    if (texture != 0) {
        batch.texture = texture;
    }
}

double Overlay::textWidth(const std::string &string, double size, double tracking) {
    if (!useFont) {
        return VectorFont::width(string, size, tracking);
    }
    return font.width(string, capPixels(size), unitsPerPixel, tracking * size);
}

void Overlay::end() {
    shader.use();
    shader.setMat4("uViewProj", Matrix4::orthographic(0, spaceWidth, 0, HEIGHT, -1, 1));
    shader.setMat4("uModel", Matrix4::identity());
    shader.setInt("uUnlit", 1);
    shader.setInt("uHasTexture", 0);
    shader.setInt("uHasSpecMap", 0);
    shader.setInt("uFog", 0);
    shader.setVec2("uUvOffset", 0.0f, 0.0f);
    shader.setVec2("uUvScale", 1.0f, 1.0f);

    glDisable(GL_DEPTH_TEST);

    // The dimmer goes down before anything that has to read over it.
    if (shadeRequested) {
        shader.setInt("uHasTexture", 1);
        drawShade();
        shader.setInt("uHasTexture", 0);
    }

    // Dim first so brighter work lands on top of it where they overlap.
    const OverlayColour order[COLOUR_COUNT] = {
            OverlayColour::DIM, OverlayColour::PRIMARY, OverlayColour::BRIGHT,
            OverlayColour::WARN, OverlayColour::LOCK,
    };
    for (OverlayColour colour : order) {
        LineBatch &batch = lines[(int) colour];
        if (batch.points.empty()) {
            continue;
        }
        setColour(colour);
        batch.mesh.upload(batch.points);
        batch.mesh.draw();
    }

    // Then the glyph quads, the only textured thing in this pass.
    shader.setInt("uHasTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    for (auto &entry : texts) {
        TextBatch &batch = *entry.second;
        if (batch.vertices.empty() || batch.texture == 0) {
            continue;
        }
        setColour(batch.colour);
        glBindTexture(GL_TEXTURE_2D, batch.texture);
        batch.mesh.upload(batch.vertices);
        batch.mesh.draw();
    }
    shader.setInt("uHasTexture", 0);

    glEnable(GL_DEPTH_TEST);
}
