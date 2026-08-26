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

    bool firstUpload = (vao == 0);
    if (firstUpload) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // DYNAMIC_DRAW because the HUD rewrites its mesh once a frame; the walls
    // upload once and never come back, which this hint costs nothing.
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (data.size() * sizeof(float)), data.data(), GL_DYNAMIC_DRAW);

    if (firstUpload) {
        glEnableVertexAttribArray(0); // position only
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    }

    glBindVertexArray(0);
}

void LineMesh::draw() const {
    if (vertexCount == 0) {
        return;
    }
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glBindVertexArray(0);
}
