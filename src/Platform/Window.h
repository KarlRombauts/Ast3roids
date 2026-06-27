#ifndef STARFOX_WINDOW_H
#define STARFOX_WINDOW_H

// We provide our own main(), so stop SDL from redefining it to SDL_main.
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <string>

// Owns the SDL2 window + OpenGL context. Replaces GLUT's window/context/loop so
// the same code builds for native desktop and (Task 0b) Emscripten/WebGL2.
class Window {
public:
    bool create(const std::string &title, int width, int height);
    void swap();
    void destroy();

    SDL_Window *handle() const { return window; }

private:
    SDL_Window *window = nullptr;
    SDL_GLContext glContext = nullptr;
};

#endif //STARFOX_WINDOW_H
