#include <sstream>
#include <Helpers.h>
#include <regex>
#include "ObjParser.h"
#include "Normals.h"
#include "MaterialParser.h"

Geometry ObjParser::parse(std::string filepath) {
    geometry = Geometry();
    normalVectors = std::vector<Vector3>();

    numFaces=0;
    numNormals=0;
    numTexCoors=0;
    numVerts=0;
    numShapes=0;

    objFilePath = filepath;
    objFile = std::ifstream(filepath);

    std::string line;
    if (objFile.is_open()) {
        while (getline(objFile, line)) {
            countLine(line);
        }
        objFile.clear();
        objFile.seekg(0, std::ios::beg);

        while (getline(objFile, line)) {
            parseLine(line);
        }
        objFile.close();
    }

//    normaliseNormals();
    return geometry;
}

void ObjParser::normaliseNormals() {
    for (Vector3 &normal: geometry.normals) {
        normal = normal.normalize();
    }
}

void ObjParser::parseLine(std::string &_line) {
    std::stringstream line(_line);

    std::string token;
    line >> token;

    if (token == "mtllib") {
        parseMaterialFile(_line);
    } else if (token == "v") {
        parseVertices(_line);
    } else if (token == "vt") {
        parseTexCoor(_line);
    } else if (token == "vn") {
        parseNormals(_line);
    } else if (token == "f") {
        parseFaces(_line);
    } else if (token == "usemtl") {
        parseCurrentMaterial(_line);
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
        geometry.normals = std::vector<Vector3>(numVerts);
    }

    if (geometry.uvs.empty()) {
        geometry.uvs = std::vector<Vector2>(numTexCoors);
    }

    parseTriangleFace(line);
}


void ObjParser::parseTriangleFace(const std::string &line) {
    TriangleIndices vertIndices;
    TriangleIndices uvIndices;
    int vn1, vn2, vn3; // Normals

    sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
           &vertIndices.v1, &uvIndices.v1, &vn1,
           &vertIndices.v2, &uvIndices.v2, &vn2,
           &vertIndices.v3, &uvIndices.v3, &vn3
    );

    vertIndices.v1--;
    vertIndices.v2--;
    vertIndices.v3--;

    uvIndices.v1--;
    uvIndices.v2--;
    uvIndices.v3--;


    geometry.normals[vertIndices.v1] += normalVectors[vn1 - 1];
    geometry.normals[vertIndices.v2] += normalVectors[vn2 - 1];
    geometry.normals[vertIndices.v3] += normalVectors[vn3 - 1];

    geometry.faces.emplace_back(vertIndices, uvIndices, currentMaterial);
}

void ObjParser::parseTexCoor(const std::string &string) {
    std::stringstream stringstream(string);

    std::string token;
    double x;
    double y;

    stringstream >> token; // skip token
    stringstream >> x;
    stringstream >> y;
    geometry.uvs.emplace_back(x, y);
}

void ObjParser::setCurrentMaterial(std::string materialName) {
    for (Material *material: geometry.materials) {
        if (material->name == materialName) {
            currentMaterial = material;
            return;
        }
    }
}

void ObjParser::parseMaterialFile(const std::string & string) {
    std::stringstream stringstream(string);
    std::string token;
    std::string filename;

    stringstream >> token;
    stringstream >> filename;

    geometry.materials = MaterialParser().parse(getDirFromPath(objFilePath) + "/" + filename);
}

void ObjParser::parseCurrentMaterial(std::string &string) {
    std::stringstream stringstream(string);
    std::string token;
    std::string materialName;

    stringstream >> token;
    stringstream >> materialName;

    setCurrentMaterial(materialName);
}

void ObjParser::countLine(std::string _line) {
    std::stringstream line(_line);

    std::string token;
    line >> token;

    if (token == "v") {
        numVerts++;
    } else if (token == "vt") {
        numTexCoors++;
    } else if (token == "vn") {
        numNormals++;
    } else if (token == "f") {
        numFaces++;
    } else if (token == "o") {
        numShapes++;
    }
}
