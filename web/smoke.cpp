// Toolchain smoke test: proves emcc + SDL2 + WebGL2 (GLES3) + the browser main
// loop work end-to-end, independent of the game's (still fixed-function) renderer.
// Once RenderSystem is ported to GLES3 (Tasks 2-6), the real game build replaces this.
#include <SDL.h>
#include <emscripten.h>
#include <GLES3/gl3.h>
#include <cmath>

static SDL_Window *window = nullptr;

static void frame() {
    // Animated clear color so it's unmistakably live (not just a dark static frame).
    float t = SDL_GetTicks() / 1000.0f;
    float r = 0.5f + 0.5f * std::sin(t);
    float g = 0.5f + 0.5f * std::sin(t + 2.0f);
    float b = 0.5f + 0.5f * std::sin(t + 4.0f);
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    window = SDL_CreateWindow("smoke", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              600, 600, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(window);
    emscripten_set_main_loop(frame, 0, 1);
    return 0;
}
