//
// Created by Karl Rombauts on 3/5/21.
//

#ifndef STARFOX_RAYCASTER_H
#define STARFOX_RAYCASTER_H


#include <ecs/Entity.h>
#include <Vector2.h>

class RayCaster {
public:
    void setFromCamera(Vector2 mouse, Entity * cameraEntity) {
        camera->get<Camera>()
    }

};


#endif //STARFOX_RAYCASTER_H
