#define STB_IMAGE_IMPLEMENTATION
#include "TextureLoader.h"
#include "Helpers/StbImage.h"
#include "OpenGL.h"

TextureLoader::TexturesMap TextureLoader::textures = {};

unsigned int TextureLoader::load(std::string filepath) {
    auto it = textures.find(filepath);
    if (it != textures.end()) {
        return it->second;
    }

    int width, height, components;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &components, STBI_rgb_alpha);

    unsigned int id;
    glPushAttrib(GL_TEXTURE_BIT);
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glPopAttrib();

    delete data;

    textures.insert({filepath, id});
    return id;
}
