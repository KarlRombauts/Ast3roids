//
// Created by Karl Rombauts on 4/6/21.
//

#include <Components/AnimatedTexture.h>
#include <Components/Geometry.h>
#include <Components/Destroy.h>
#include <Components/Light.h>
#include "AnimatedTextureSystem.h"

void AnimatedTextureSystem::update(EntityManager &entities, double dt) {

    if (timeSinceLastFrame < 1000 / 40) {
        timeSinceLastFrame += dt;
        return;
    }

    for (Entity *entity: entities.getEntitiesWith<AnimatedTexture, Geometry>()) {
        AnimatedTexture *animTex = entity->get<AnimatedTexture>();

        int currentIndex = animTex->rowOffset * animTex->cols + animTex->colOffset;
        int totalFrames = animTex->rows * animTex->cols;

        currentIndex++;
        if (currentIndex >= totalFrames) {
            if (animTex->endBehaviour == AnimationBehaviour::DEATH) {
                entity->assign<Destroy>();
                continue;
            } if (animTex->endBehaviour == AnimationBehaviour::LOOP) {
                animTex->rowOffset = 0;
                animTex->colOffset = 0;
            }
        }

        animTex->rowOffset = currentIndex / animTex->cols;
        animTex->colOffset = currentIndex % animTex->cols;
        timeSinceLastFrame = 0;
    }
}
