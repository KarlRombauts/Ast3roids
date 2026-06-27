#ifndef STARFOX_SHADER_H
#define STARFOX_SHADER_H

#include <OpenGL.h>
#include <Matrix4.h>
#include <string>

// Compiles and links a GLSL program. Sources are written without a #version line;
// Shader prepends the right one (#version 330 core on desktop, #version 300 es on
// web), so the same shader body works for both GL 3.3 core and WebGL2/GLES3.
class Shader {
public:
    // Loads, compiles and links a vertex + fragment shader from files. The files
    // contain no #version line; the right one is prepended per platform.
    bool loadFromFiles(const std::string &vertexPath, const std::string &fragmentPath);
    void use() const;
    void setMat4(const std::string &name, const Matrix4 &matrix) const;
    void setInt(const std::string &name, int value) const;

    GLuint id() const { return program; }

private:
    GLuint program = 0;

    static GLuint compile(GLenum type, const std::string &source);
};

#endif //STARFOX_SHADER_H
