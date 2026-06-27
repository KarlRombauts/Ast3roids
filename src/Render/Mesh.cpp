#include "Mesh.h"
#include <Components/Material.h>
#include <algorithm>

Mesh::~Mesh() {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
    }
}

void Mesh::upload(const Geometry &geometry) {
    // Interleaved layout per vertex: position(3) + normal(3) + uv(2) = 8 floats.
    std::vector<float> data;
    data.reserve(geometry.faces.size() * 3 * 8);

    bool hasNormals = !geometry.normals.empty();
    bool hasUvs = !geometry.uvs.empty();

    auto emitVertex = [&](const Face &face, int i) {
        const Vector3 &v = geometry.vertices[face.vertIndices[i]];
        data.push_back((float) v.x);
        data.push_back((float) v.y);
        data.push_back((float) v.z);

        if (hasNormals) {
            const Vector3 &n = geometry.normals[face.vertIndices[i]];
            data.push_back((float) n.x);
            data.push_back((float) n.y);
            data.push_back((float) n.z);
        } else {
            data.push_back(0); data.push_back(0); data.push_back(0);
        }

        if (hasUvs) {
            const Vector2 &uv = geometry.uvs[face.uvIndices[i]];
            data.push_back((float) uv.x);
            data.push_back((float) uv.y);
        } else {
            data.push_back(0); data.push_back(0);
        }
    };

    // Collect the distinct (shape, material) groups in first-seen order. Grouping
    // by shape lets each shape be drawn with its own (possibly animated) transform.
    std::vector<std::pair<int, const Material *>> groups;
    for (const Face &face : geometry.faces) {
        std::pair<int, const Material *> key = {face.shapeIndex, face.material};
        if (std::find(groups.begin(), groups.end(), key) == groups.end()) {
            groups.push_back(key);
        }
    }

    // Append each group's faces as one contiguous range.
    for (const auto &group : groups) {
        GLint start = (GLint) (data.size() / 8);
        GLsizei count = 0;
        for (const Face &face : geometry.faces) {
            if (face.shapeIndex != group.first || face.material != group.second) {
                continue;
            }
            for (int i = 0; i < 3; i++) {
                emitVertex(face, i);
            }
            count += 3;
        }
        submeshes.push_back({group.first, group.second, start, count});
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (data.size() * sizeof(float)), data.data(), GL_STATIC_DRAW);

    GLsizei stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *) 0);
    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *) (6 * sizeof(float)));

    glBindVertexArray(0);
}

void Mesh::draw(const std::function<void(const SubMesh &)> &setup) const {
    glBindVertexArray(vao);
    for (const SubMesh &sub : submeshes) {
        setup(sub);
        glDrawArrays(GL_TRIANGLES, sub.start, sub.count);
    }
    glBindVertexArray(0);
}
