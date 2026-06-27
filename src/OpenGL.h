#ifndef UNTITLED_OPENGL_H
#define UNTITLED_OPENGL_H

// Core/ES 3 only — no fixed-function pipeline. Desktop uses a GL 3.3 core
// context; the web build uses WebGL2 (GLES3). The shared shader-based renderer
// targets both.
#ifdef __EMSCRIPTEN__
# include <GLES3/gl3.h>
#elif defined(__APPLE__)
# define GL_SILENCE_DEPRECATION
# include <OpenGL/gl3.h>
#else
# include <GL/gl3.h>
#endif

#endif //UNTITLED_OPENGL_H
