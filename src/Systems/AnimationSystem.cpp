#include <Components/Light.h>
#include <Components/LightAnimation.h>
#include <Helpers.h>
#include "AnimationSystem.h"

void AnimationSystem::update(EntityManager &entities, double dt) {
    int msPerFrame = 1000 / 60;

    if (timeSinceLastFrame < msPerFrame) {
        timeSinceLastFrame += dt;
        return;
    }

    int elapsedFrames = (int) timeSinceLastFrame / msPerFrame;
    animateLights(entities, elapsedFrames);
}

void AnimationSystem::animateLights(EntityManager &entities, int elapsedFrames) {
    for (Entity *entity: entities.getEntitiesWith<Light, LightAnimation>()) {
        Light &start = entity->get<LightAnimation>()->start;
        Light &end = entity->get<LightAnimation>()->end;

        int duration = entity->get<LightAnimation>()->duration;
        int &currentFrame = entity->get<LightAnimation>()->currentFrame;
        Light &current = *entity->get<Light>();

        currentFrame += elapsedFrames;

        double t = (double) currentFrame / duration;
        for (int i = 0; i < 4; i++) {
            current.diffuse[i] = lerp(start.diffuse[i], end.diffuse[i], t);
            current.specular[i] = lerp(start.specular[i], end.specular[i], t);
            current.ambient[i] = lerp(start.ambient[i], end.ambient[i], t);
        }
        current.attenuation = lerp(start.attenuation, end.attenuation, t);
    }
}
