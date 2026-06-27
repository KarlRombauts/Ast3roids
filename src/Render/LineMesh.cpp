#include "LineMesh.h"

LineMesh::~LineMesh() {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
    }
}

void LineMesh::upload(const std::vector<Vector3> &points) {
    std::vector<float> data;
    data.reserve(points.size() * 3);
    for (const Vector3 &p : points) {
        data.push_back((float) p.x);
        data.push_back((float) p.y);
        data.push_back((float) p.z);
    }
    vertexCount = (GLsizei) points.size();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (data.size() * sizeof(float)), data.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position only
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    glBindVertexArray(0);
}

void LineMesh::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glBindVertexArray(0);
}
