//
// Created by Karl Rombauts on 7/5/21.
//

#include <sstream>
#include <Helpers.h>
#include <regex>
#include "ObjParser.h"

Geometry ObjParser::parse(std::string filename) {
    geometry = Geometry();
    normalVectors.clear();
    objFile = std::ifstream(filename);
    std::string line;
    if (objFile.is_open()) {
        while (getline(objFile, line)) {
            parseLine(line);
        }
        objFile.close();
    }

    for (Vector3 &normal: geometry.normals) {
        normal = normal.normalize();
    }

    return geometry;
}

void ObjParser::parseLine(std::string &_line) {
    std::stringstream line(_line);

    std::string token;
    line >> token;

    if (token == "v") {
        parseVertices(_line);
    } else if (token == "vn") {
        parseNormals(_line);
    } else if (token == "f") {
        parseFaces(_line);
    }
}

void ObjParser::parseVertices(const std::string &string) {
    std::stringstream stringstream(string);

    std::string token;
    double x;
    double y;
    double z;

    stringstream >> token; // skip token
    stringstream >> x;
    stringstream >> y;
    stringstream >> z;
    geometry.vertices.emplace_back(x, y, z);
}

void ObjParser::parseNormals(const std::string &string) {
    std::stringstream stringstream(string);

    std::string token;
    double x;
    double y;
    double z;

    stringstream >> token; // skip token
    stringstream >> x;
    stringstream >> y;
    stringstream >> z;
    normalVectors.emplace_back(x, y, z);
}

void ObjParser::parseFaces(std::string &line) {
    if (geometry.normals.empty()) {
        geometry.normals = std::vector<Vector3>(geometry.vertices);
    }

    trim(line);
    std::vector<std::string> tokens = splitString(line, " ");
    int numVerts = tokens.size() - 1; // Ignore first token

    if (numVerts == 3) {
        parseTriangleFace(line);
    } else if (numVerts == 4) {
        parseQuadFace(line);
    }
}

void ObjParser::parseQuadFace(const std::string &line) {
    QuadIndices quad;
    int vt1, vt2, vt3, vt4; // texture
    int vn1, vn2, vn3, vn4; // normals

    sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
        &quad.v1, &vt1, &vn1,
        &quad.v2, &vt2, &vn2,
        &quad.v3, &vt3, &vn3,
        &quad.v4, &vt4, &vn4
    );

    quad.v1--;
    quad.v2--;
    quad.v3--;
    quad.v4--;

    geometry.normals[quad.v1] += normalVectors[vn1 - 1];
    geometry.normals[quad.v2] += normalVectors[vn2 - 1];
    geometry.normals[quad.v3] += normalVectors[vn3 - 1];
    geometry.normals[quad.v4] += normalVectors[vn4 - 1];

    geometry.quads.push_back(quad);
}

void ObjParser::parseTriangleFace(const std::string &line) {
    TriangleIndices triangle;
    int vt1, vt2, vt3; // texture
    int vn1, vn2, vn3; // normals

    sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
           &triangle.v1, &vt1, &vn1,
           &triangle.v2, &vt2, &vn2,
           &triangle.v3, &vt3, &vn3
    );

    triangle.v1--;
    triangle.v2--;
    triangle.v3--;

    if (triangle.v1)
    geometry.normals[triangle.v1] += normalVectors[vn1 - 1];
    geometry.normals[triangle.v2] += normalVectors[vn2 - 1];
    geometry.normals[triangle.v3] += normalVectors[vn3 - 1];

    geometry.triangles.push_back(triangle);
}
