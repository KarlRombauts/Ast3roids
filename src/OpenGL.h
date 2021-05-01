#ifndef UNTITLED_OPENGL_H
#define UNTITLED_OPENGL_H


#if _WIN32
# include <windows.h>
#endif

#if __APPLE__
#define GL_SILENCE_DEPRECATION
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
    # include <GL/glu.h>
    # include <GL/glut.h>
#endif

#endif //UNTITLED_OPENGL_H
