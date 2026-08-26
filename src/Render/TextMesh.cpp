#include "TextMesh.h"

static const int FLOATS_PER_VERTEX = 5; // x, y, z, u, v

TextMesh::~TextMesh() {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
    }
}

void TextMesh::upload(const std::vector<float> &vertices) {
    vertexCount = (GLsizei) (vertices.size() / FLOATS_PER_VERTEX);

    bool firstUpload = (vao == 0);
    if (firstUpload) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertices.size() * sizeof(float)),
                 vertices.data(), GL_DYNAMIC_DRAW);

    if (firstUpload) {
        GLsizei stride = FLOATS_PER_VERTEX * sizeof(float);
        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
        glEnableVertexAttribArray(2); // UV, matching the shader's layout
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *) (3 * sizeof(float)));
    }

    glBindVertexArray(0);
}

void TextMesh::draw() const {
    if (vertexCount == 0) {
        return;
    }
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}
