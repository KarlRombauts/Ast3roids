#ifndef STARFOX_OBJPARSER_H
#define STARFOX_OBJPARSER_H

#include <iostream>
#include <fstream>
#include <Components/Geometry.h>

class ObjParser {
    std::ifstream objFile;
    Geometry geometry;
    std::vector<Vector3> normalVectors;

public:
    ObjParser() {};

    Geometry parse(std::string filename);

    void parseLine(std::string &_line);

    void parseVertices(std::stringstream stringstream);

    void parseFaces(std::stringstream stringstream);

    void parseFaces(std::string &line);

    void parseVertices(const std::string &string);

    void parseQuadFace(const std::string &basicString);

    void parseTriangleFace(const std::string &basicString);

    void parseNormals(const std::string &string);
};


#endif //STARFOX_OBJPARSER_H
