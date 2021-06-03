#ifndef STARFOX_TEXTURELOADER_H
#define STARFOX_TEXTURELOADER_H

#include <iostream>
#include <map>

class TextureLoader {
    typedef std::map<std::string, int> TexturesMap;
    static TexturesMap textures;

public:
    static unsigned int load(std::string filepath);
};

#endif //STARFOX_TEXTURELOADER_H
