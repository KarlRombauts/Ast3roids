//
// Created by Karl Rombauts on 2/6/21.
//

#ifndef STARFOX_MATERIALPARSER_H
#define STARFOX_MATERIALPARSER_H


#include <vector>
#include <Components/Geometry.h>

class MaterialParser {
    std::string mtlFilePath;

    std::vector<Material *> materials;

    void parseLine(std::string &_line);

    void parseName(std::string &string);

    void parseDiffuse(std::string &string);

    void parseShininess(std::string &string);

    void parseAmbient(std::string &string);

    void parseSpecular(std::string &string);

public:
    std::vector<Material *> parse(std::string filepath);

    void parseTexture(std::string &string);

};


#endif //STARFOX_MATERIALPARSER_H
