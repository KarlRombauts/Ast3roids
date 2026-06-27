#include "Window.h"
#include "../OpenGL.h"
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

bool Window::create(const std::string &title, int width, int height) {
#ifdef __EMSCRIPTEN__
    // Emscripten's EGL only creates WebGL1 contexts, so SDL's GL path can't give
    // us WebGL2. Create the WebGL2 context directly on the canvas and make it
    // current; SDL still owns the window, events and input. This must happen
    // before SDL touches the canvas (so it isn't claimed first).
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2; // WebGL 2.0 == OpenGL ES 3.0
    attrs.minorVersion = 0;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attrs);
    if (ctx <= 0) {
        std::cerr << "Failed to create a WebGL2 context (handle " << (long) ctx << ")" << std::endl;
        return false;
    }
    emscripten_webgl_make_context_current(ctx);
#endif

    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

#ifndef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Desktop GL 3.3 core (feature-equivalent to WebGL2). macOS only grants core
    // profiles with the forward-compatible flag set.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
#endif

#ifdef __EMSCRIPTEN__
    // No SDL_WINDOW_OPENGL: SDL must not claim a GL context on the canvas, or our
    // own emscripten_webgl_create_context below can't create the WebGL2 one.
    Uint32 windowFlags = 0;
#else
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
#endif
    window = SDL_CreateWindow(title.c_str(),
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, windowFlags);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

#ifndef __EMSCRIPTEN__
    glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_GL_SetSwapInterval(1); // vsync
#endif

    const char *version = (const char *) glGetString(GL_VERSION);
    std::cout << "GL_VERSION: " << (version ? version : "(null)") << std::endl;

    return true;
}

void Window::swap() {
#ifndef __EMSCRIPTEN__
    SDL_GL_SwapWindow(window);
#endif
    // On Emscripten the canvas is presented automatically after each animation
    // frame, so there is nothing to swap.
}

void Window::destroy() {
    if (glContext != nullptr) {
        SDL_GL_DeleteContext(glContext);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}
