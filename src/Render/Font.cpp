#include "Font.h"

#include <cmath>
#include <cstdio>
#include <map>

#define STBTT_STATIC                 // keep these symbols to this translation unit
#define STB_TRUETYPE_IMPLEMENTATION
#include "../Helpers/StbTrueType.h"

namespace {
    const int FIRST_CHAR = 32;   // space
    const int CHAR_COUNT = 95;   // through '~'

    // A resize drag asks for a new size every frame. Baking is cheap but
    // textures are not free, so the cache is dropped once it grows past a few
    // entries rather than accumulating one atlas per pixel height crossed.
    const size_t MAX_CACHED_SIZES = 8;
}

struct Font::Impl {
    std::vector<unsigned char> fontData;
    stbtt_fontinfo info{};
    bool valid = false;

    // Em height (what stb calls "pixel height") per unit of cap height. stb
    // sizes text by the ascent-to-descent box; the HUD sizes it by the cap
    // height, which is the measurement you actually see.
    double emPerCap = 1.4;

    struct Atlas {
        GLuint texture = 0;
        int width = 0;
        int height = 0;
        stbtt_bakedchar chars[CHAR_COUNT]{};
    };

    std::map<int, Atlas> atlases;

    ~Impl() {
        for (auto &entry : atlases) {
            if (entry.second.texture != 0) {
                glDeleteTextures(1, &entry.second.texture);
            }
        }
    }

    // Bakes an atlas for this cap height, or returns the cached one. Null if
    // the glyphs could not be fitted.
    const Atlas *atlas(int capPixels) {
        if (!valid || capPixels < 4) {
            return nullptr;
        }

        auto found = atlases.find(capPixels);
        if (found != atlases.end()) {
            return found->second.texture != 0 ? &found->second : nullptr;
        }

        if (atlases.size() >= MAX_CACHED_SIZES) {
            for (auto &entry : atlases) {
                if (entry.second.texture != 0) {
                    glDeleteTextures(1, &entry.second.texture);
                }
            }
            atlases.clear();
        }

        Atlas atlas;
        double emPixels = capPixels * emPerCap;

        // Grow the bitmap until every glyph fits. BakeFontBitmap reports a
        // negative number of characters when it runs out of room.
        std::vector<unsigned char> coverage;
        for (int side = 128; side <= 2048; side *= 2) {
            coverage.assign((size_t) side * side, 0);
            int baked = stbtt_BakeFontBitmap(fontData.data(), 0, (float) emPixels,
                                             coverage.data(), side, side,
                                             FIRST_CHAR, CHAR_COUNT, atlas.chars);
            if (baked > 0) {
                atlas.width = side;
                atlas.height = side;
                break;
            }
        }
        if (atlas.width == 0) {
            atlases[capPixels] = atlas; // remember the failure, stop retrying
            return nullptr;
        }

        // The shader multiplies the texture's RGB by uColor and takes its
        // alpha, so the atlas goes up as white with the coverage in alpha.
        std::vector<unsigned char> rgba((size_t) atlas.width * atlas.height * 4);
        for (size_t i = 0; i < coverage.size(); i++) {
            rgba[i * 4 + 0] = 255;
            rgba[i * 4 + 1] = 255;
            rgba[i * 4 + 2] = 255;
            rgba[i * 4 + 3] = coverage[i];
        }

        glGenTextures(1, &atlas.texture);
        glBindTexture(GL_TEXTURE_2D, atlas.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas.width, atlas.height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
        // Linear, with the quads snapped to whole pixels: texels then line up
        // one-to-one with screen pixels and the text comes out sharp.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        atlases[capPixels] = atlas;
        return &atlases[capPixels];
    }

    // Advance width of `text` in atlas pixels.
    double advance(const Atlas &atlas, const std::string &text, double trackingPixels) const {
        double total = 0;
        for (char c : text) {
            if (c < FIRST_CHAR || c >= FIRST_CHAR + CHAR_COUNT) {
                continue;
            }
            total += atlas.chars[c - FIRST_CHAR].xadvance + trackingPixels;
        }
        if (!text.empty()) {
            total -= trackingPixels;
        }
        return total;
    }
};

Font::Font() : impl(new Impl()) {}

Font::~Font() = default;

bool Font::load(const std::string &path) {
    FILE *file = std::fopen(path.c_str(), "rb");
    if (file == nullptr) {
        std::printf("[font] could not open %s\n", path.c_str());
        return false;
    }

    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    if (size <= 0) {
        std::fclose(file);
        return false;
    }

    impl->fontData.resize((size_t) size);
    size_t read = std::fread(impl->fontData.data(), 1, (size_t) size, file);
    std::fclose(file);
    if (read != (size_t) size) {
        return false;
    }

    if (!stbtt_InitFont(&impl->info, impl->fontData.data(),
                        stbtt_GetFontOffsetForIndex(impl->fontData.data(), 0))) {
        std::printf("[font] could not parse %s\n", path.c_str());
        return false;
    }

    // Measure the cap height off a capital H rather than trusting a table, then
    // keep the ratio that turns a cap height into stb's em height.
    int ascent = 0, descent = 0, lineGap = 0;
    stbtt_GetFontVMetrics(&impl->info, &ascent, &descent, &lineGap);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    if (stbtt_GetCodepointBox(&impl->info, 'H', &x0, &y0, &x1, &y1) && y1 > y0) {
        impl->emPerCap = (double) (ascent - descent) / (double) (y1 - y0);
    }

    impl->valid = true;
    return true;
}

bool Font::ready() const {
    return impl->valid;
}

double Font::width(const std::string &text, int capPixels, double unitsPerPixel, double tracking) {
    const Impl::Atlas *atlas = impl->atlas(capPixels);
    if (atlas == nullptr) {
        return 0;
    }
    return impl->advance(*atlas, text, tracking / unitsPerPixel) * unitsPerPixel;
}

GLuint Font::append(std::vector<float> &vertices, const std::string &text,
                    double x, double y, int capPixels, double unitsPerPixel,
                    TextAlignment align, double tracking) {
    const Impl::Atlas *atlas = impl->atlas(capPixels);
    if (atlas == nullptr) {
        return 0;
    }

    double trackingPixels = tracking / unitsPerPixel;

    // Work in pixels, then scale back out: the pen starts on a whole pixel so
    // every glyph quad lands on the grid its bitmap was rasterised for.
    double penX = std::round(x / unitsPerPixel);
    double baseline = std::round(y / unitsPerPixel);
    double advance = impl->advance(*atlas, text, trackingPixels);
    if (align == TextAlignment::CENTER) {
        penX = std::round(penX - advance / 2);
    } else if (align == TextAlignment::RIGHT) {
        penX = std::round(penX - advance);
    }

    for (char c : text) {
        if (c < FIRST_CHAR || c >= FIRST_CHAR + CHAR_COUNT) {
            continue;
        }
        const stbtt_bakedchar &glyph = atlas->chars[c - FIRST_CHAR];

        // stb's offsets assume y grows downward from the baseline; the HUD has
        // y up, so the glyph's top edge is the baseline minus its y offset.
        double left = (penX + glyph.xoff) * unitsPerPixel;
        double top = (baseline - glyph.yoff) * unitsPerPixel;
        double right = left + (glyph.x1 - glyph.x0) * unitsPerPixel;
        double bottom = top - (glyph.y1 - glyph.y0) * unitsPerPixel;

        double u0 = (double) glyph.x0 / atlas->width;
        double v0 = (double) glyph.y0 / atlas->height;
        double u1 = (double) glyph.x1 / atlas->width;
        double v1 = (double) glyph.y1 / atlas->height;

        const float quad[6][5] = {
                {(float) left,  (float) top,    0, (float) u0, (float) v0},
                {(float) left,  (float) bottom, 0, (float) u0, (float) v1},
                {(float) right, (float) bottom, 0, (float) u1, (float) v1},
                {(float) left,  (float) top,    0, (float) u0, (float) v0},
                {(float) right, (float) bottom, 0, (float) u1, (float) v1},
                {(float) right, (float) top,    0, (float) u1, (float) v0},
        };
        for (const auto &vertex : quad) {
            vertices.insert(vertices.end(), vertex, vertex + 5);
        }

        penX += glyph.xadvance + trackingPixels;
    }

    return atlas->texture;
}
