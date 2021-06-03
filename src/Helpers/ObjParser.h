#ifndef STARFOX_OBJPARSER_H
#define STARFOX_OBJPARSER_H

#include <iostream>
#include <fstream>
#include <Components/Geometry.h>

class ObjParser {

    std::ifstream objFile;

    std::string objFilePath;

    Geometry geometry;

    std::vector<Vector3> normalVectors;

    Material *currentMaterial;

    int numVerts;
    int numTexCoors;
    int numNormals;
    int numFaces;
    int numShapes;
public:
    ObjParser() {};

    Geometry parse(std::string filepath);

    void parseLine(std::string &_line);

    void parseVertices(std::stringstream stringstream);

    void parseFaces(std::stringstream stringstream);

    void parseFaces(std::string &line);

    void parseVertices(const std::string &string);

    void parseTriangleFace(const std::string &basicString);

    void parseNormals(const std::string &string);

    void parseTexCoor(const std::string &string);

    void parseMaterialFile(std::stringstream stringstream);

    void parseMaterialFile(const std::string &stringstream);

    void normaliseNormals();

    void setCurrentMaterial(std::string materialName);

    void parseCurrentMaterial(std::string &string);

    void countLine(std::string line);

};


#endif //STARFOX_OBJPARSER_H
