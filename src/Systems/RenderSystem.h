//
// Created by Karl Rombauts on 14/3/21.
//

#ifndef UNTITLED_RENDERSYSTEM_H
#define UNTITLED_RENDERSYSTEM_H


#include <OpenGL/OpenGL.h>
#include "System.h"
#include "../Components/Transform.h"
#include "../Components/Texture.h"
#include "../Quaternion.h"


class RenderSystem: public System {

public:
    void update(EntityManager &entities, double dt) override;

    void drawHealthBars(Entity *entity) const;

    void drawShape(Entity *entity) const;

    void drawLine(Entity *entity) const;

    void drawParticle(Entity *entity) const;

    void renderString(GLdouble x, GLdouble y, const std::string &string,
                      TextAlignment alignment);

    void drawScore();

    void drawEntities(EntityManager &entities);

    void drawDifficulty();

    void glRotateQuaternion(const Quaternion &q);

    void drawAxis() const;

    void updateCamera(EntityManager &entities);

    void drawLine(const Vector3 &start, const Vector3 &end) const;

    void drawGridPlane(Entity *entity) const;

    void drawTestCube() const;
};


#endif //UNTITLED_RENDERSYSTEM_H
