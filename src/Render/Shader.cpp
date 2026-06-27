#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

static std::string readFile(const std::string &path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

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

bool Shader::loadFromFiles(const std::string &vertexPath, const std::string &fragmentPath) {
    std::string vertexSource = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);
    if (vertexSource.empty() || fragmentSource.empty()) {
        return false;
    }

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

void Shader::setFloat(const std::string &name, float value) const {
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniform1f(location, value);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniform3f(location, x, y, z);
}
