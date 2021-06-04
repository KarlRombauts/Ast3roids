#include "MaterialLibrary.h"

void MaterialLibrary::init() {
    ASTEROID = new Material();
    ASTEROID->setSpecular(1,1,1);
    ASTEROID->setDiffuse(0.8,0.8,0.8);
    ASTEROID->setAmbient(1,1,1);
    ASTEROID->setEmission(0, 0, 0);
    ASTEROID->textureId = TextureLoader::load(
            "/Users/karlrombauts/CLionProjects/asteroids-3d/src/Textures/asteroid.jpg");

    BULLET = new Material();
    BULLET->setSpecular(0,0,0);
    BULLET->setDiffuse(0,0,0);
    BULLET->setAmbient(0,0,0);
    BULLET->setEmission(1, 1, 1);
    BULLET->textureId = TextureLoader::load(
            "/Users/karlrombauts/CLionProjects/asteroids-3d/src/Textures/fireball_green.png");

    EXPLOSION = new Material();
    EXPLOSION->setSpecular(0,0,0);
    EXPLOSION->setDiffuse(0,0,0);
    EXPLOSION->setAmbient(0,0,0);
    EXPLOSION->setEmission(1, 1, 1);
    EXPLOSION->textureId = TextureLoader::load(
            "/Users/karlrombauts/CLionProjects/asteroids-3d/src/Textures/explosion.png");

    GLOW_PARTICLE = new Material();
    GLOW_PARTICLE->setSpecular(0,0,0);
    GLOW_PARTICLE->setDiffuse(0,0,0);
    GLOW_PARTICLE->setAmbient(0,0,0);
    GLOW_PARTICLE->setEmission(1, 1, 1);
    GLOW_PARTICLE->textureId = TextureLoader::load(
            "/Users/karlrombauts/CLionProjects/asteroids-3d/src/Textures/glow-particle.png");
}

MaterialLibrary materialLibrary;