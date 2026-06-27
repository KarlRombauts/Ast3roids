#include "Shader.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
static const char *VERSION_HEADER = "#version 300 es\nprecision highp float;\n";
#else
static const char *VERSION_HEADER = "#version 330 core\n";
#endif

GLuint Shader::compile(GLenum type, const std::string &source) {
    std::string full = std::string(VERSION_HEADER) + source;
    const char *src = full.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_FALSE) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::loadFromSource(const std::string &vertexSource, const std::string &fragmentSource) {
    GLuint vert = compile(GL_VERTEX_SHADER, vertexSource);
    GLuint frag = compile(GL_FRAGMENT_SHADER, fragmentSource);
    if (vert == 0 || frag == 0) {
        return false;
    }

    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (ok == GL_FALSE) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Shader link error:\n" << log << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return ok == GL_TRUE;
}

void Shader::use() const {
    glUseProgram(program);
}

void Shader::setMat4(const std::string &name, const Matrix4 &matrix) const {
    float f[16];
    for (int i = 0; i < 16; i++) {
        f[i] = (float) matrix.data()[i];
    }
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, f);
}

void Shader::setInt(const std::string &name, int value) const {
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniform1i(location, value);
}
