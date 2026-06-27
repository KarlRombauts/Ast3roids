#include "MaterialLibrary.h"
#include "Globals.h"
#include "Components/Material.h"

void MaterialLibrary::init() {
    ASTEROID = new Material();
    ASTEROID->setSpecular(0.03,0.03,0.03); // almost matte rock
    ASTEROID->shininess = 4;
    ASTEROID->setDiffuse(0.8,0.8,0.8);
    ASTEROID->setAmbient(1,1,1);
    ASTEROID->setEmission(0, 0, 0);
    ASTEROID->textureId = TextureLoader::load(gameConfig.TEXTURE_DIR + "/asteroid-2.jpg");

    BULLET = new Material();
    BULLET->setSpecular(0,0,0);
    BULLET->setDiffuse(0,0,0);
    BULLET->setAmbient(0,0,0);
    BULLET->setEmission(1, 1, 1);
    BULLET->textureId = TextureLoader::load(gameConfig.TEXTURE_DIR + "/fireball_green_ani.png");

    EXPLOSION = new Material();
    EXPLOSION->setSpecular(0,0,0);
    EXPLOSION->setDiffuse(0,0,0);
    EXPLOSION->setAmbient(0,0,0);
    EXPLOSION->setEmission(1, 1, 1);
    EXPLOSION->textureId = TextureLoader::load(gameConfig.TEXTURE_DIR + "/explosion.png");

    GLOW_PARTICLE = new Material();
    GLOW_PARTICLE->setSpecular(0,0,0);
    GLOW_PARTICLE->setDiffuse(0,0,0);
    GLOW_PARTICLE->setAmbient(0,0,0);
    GLOW_PARTICLE->setEmission(1, 1, 1);
    GLOW_PARTICLE->textureId = TextureLoader::load(gameConfig.TEXTURE_DIR + "/glow-particle.png");

    IMPACT = new Material();
    IMPACT->setSpecular(0,0,0);
    IMPACT->setDiffuse(0,0,0);
    IMPACT->setAmbient(0,0,0);
    IMPACT->setEmission(1, 1, 1);
    IMPACT->textureId = TextureLoader::load(gameConfig.TEXTURE_DIR + "/impact_green.png");
}

MaterialLibrary materialLibrary;