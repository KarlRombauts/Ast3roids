//
// Created by Karl Rombauts on 2/6/21.
//

#include "MaterialParser.h"
#include "TextureLoader.h"
#include "Components/Material.h"
#include <fstream>
#include <sstream>
#include <Helpers.h>

std::vector<Material *> MaterialParser::parse(std::string filepath) {
    mtlFilePath = filepath;
    std::ifstream mtlFile = std::ifstream(filepath);

    std::string line;
    if (mtlFile.is_open()) {
        while (getline(mtlFile, line)) {
            parseLine(line);
        }
        mtlFile.close();
    }

    return materials;
}

void MaterialParser::parseLine(std::string &_line) {

    std::stringstream line(_line);

    std::string token;
    line >> token;

    if (token == "newmtl") {
        parseName(_line);
    } else if (materials.empty()) {
        // A property before any `newmtl` has no material to attach to; ignore it.
        return;
    } else if (token == "Ns") {
        parseShininess(_line);
    } else if (token == "Ka") {
        parseAmbient(_line);
    } else if (token == "Kd") {
        parseDiffuse(_line);
    } else if (token == "Ks") {
        parseSpecular(_line);
    } else if (token == "Ke") {
        parseEmission(_line);
    } else if (token == "map_Kd") {
        parseTexture(_line);
    } else if (token == "map_Ks") {
        parseSpecMap(_line);
    } else if (token == "map_Bump" || token == "bump") {
        parseNormalMap(_line);
    }

}

void MaterialParser::parseName(std::string &string) {
    Material *material = new Material();

    std::stringstream line(string);
    std::string token;
    line >> token;
    line >> material->name;

    materials.push_back(material);
}

void MaterialParser::parseEmission(std::string &string) {
    Material *material = materials.back();

    std::stringstream line(string);
    std::string token;
    line >> token;

    line >> material->emission[0];
    line >> material->emission[1];
    line >> material->emission[2];
    material->emission[3] = 1 ;
}

void MaterialParser::parseSpecular(std::string &string) {
    Material *material = materials.back();

    std::stringstream line(string);
    std::string token;
    line >> token;

    line >> material->specular[0];
    line >> material->specular[1];
    line >> material->specular[2];
    material->specular[3] = 1 ;
}

void MaterialParser::parseDiffuse(std::string &string) {
    Material *material = materials.back();

    std::stringstream line(string);
    std::string token;
    line >> token;

    line >> material->diffuse[0];
    line >> material->diffuse[1];
    line >> material->diffuse[2];
    material->diffuse[3] = 1;
}

void MaterialParser::parseShininess(std::string &string) {
    Material *material = materials.back();

    std::stringstream line(string);
    std::string token;
    line >> token;
    line >> material->shininess;
}

void MaterialParser::parseAmbient(std::string &string) {
    Material *material = materials.back();

    std::stringstream line(string);
    std::string token;
    line >> token;

    line >> material->ambient[0];
    line >> material->ambient[1];
    line >> material->ambient[2];
    material->ambient[3] = 1;
}

void MaterialParser::parseTexture(std::string &string) {
    Material *material = materials.back();

    std::stringstream line(string);
    std::string token;
    std::string filepath;

    line >> token;
    line >> filepath;

    std::string path = getDirFromPath(mtlFilePath) + '/' + filepath;
    material->textureId = TextureLoader::load(path);
    line >> token;
}

// map_Ks / map_Bump can carry options (e.g. "-bm 1.0 file.png"), so take the
// last whitespace-separated token as the filename.
static std::string lastToken(const std::string &string) {
    std::stringstream line(string);
    std::string token, last;
    while (line >> token) {
        last = token;
    }
    return last;
}

void MaterialParser::parseSpecMap(std::string &string) {
    std::string filepath = lastToken(string);
    if (filepath.empty()) {
        return;
    }
    materials.back()->specTextureId = TextureLoader::load(getDirFromPath(mtlFilePath) + '/' + filepath);
}

void MaterialParser::parseNormalMap(std::string &string) {
    std::string filepath = lastToken(string);
    if (filepath.empty()) {
        return;
    }
    materials.back()->normalTextureId = TextureLoader::load(getDirFromPath(mtlFilePath) + '/' + filepath);
}

