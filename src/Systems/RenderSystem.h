//
// Created by Karl Rombauts on 14/3/21.
//

#ifndef UNTITLED_RENDERSYSTEM_H
#define UNTITLED_RENDERSYSTEM_H


#include <OpenGL/OpenGL.h>
#include "System.h"
#include "../Components/Transform.h"
#include "../Components/Color.h"
#include "../Quaternion.h"
#include "../Components/Geometry.h"
#include "Components/Material.h"


class RenderSystem: public System {

public:
    void update(EntityManager &entities, double dt) override;

    void drawHealthBars(Entity *entity) const;

    void drawGeometry(Entity *entity) const;

    void drawLine(Entity *entity) const;

    void drawParticle(Entity *entity) const;

    void renderString(GLdouble x, GLdouble y, const std::string &string,
                      TextAlignment alignment, void **font);

    void drawScore();

    void renderEntities(EntityManager &entities);

    void drawDifficulty();

    void drawAxis() const;

    void drawLine(const Vector3 &start, const Vector3 &end) const;

    void drawGridPlane(Entity *entity) const;

    void drawEntity(Entity *entity);

    void renderTransparentEntities(EntityManager &entities);

    void applyTransformations(Entity *entity);

    double getStringWidth(const std::string &string, void **font) const;

    double getTextOffset(double width, const TextAlignment &alignment) const;

    double
    getTextOffset(const std::string& string, void **font,
                  const TextAlignment &alignment) const;

    void applyMaterial(const Material *material) const;

    void drawFace(const Geometry *geometry, const Face &face) const;

    void applyCameraPosition() const;

    void applyCameraRotation();

    void drawSkyBox(EntityManager &entities) const;

    void drawFace(Entity *entity, const Face &face) const;

    void glRotateQuaternion(const Quaternion &q) const;

    void applyTransformations(const Shape &shape) const;

    void renderLight(Entity *entity, int i) const;

    void renderLights(EntityManager &entities) const;

    void setTextureCoordinates(Entity *entity, const Face &face,
                               const Geometry *geometry, int vertIndex) const;

    void drawStartScreen();

    void drawGameOver();

    void renderMainGame(EntityManager &entities);

    void drawControls();

    void updateFps();

    void drawFps();

private:
    unsigned int frameCount = 0;
    int lastFpsUpdateTime = 0;
    double currentFps = 0;
};


#endif //UNTITLED_RENDERSYSTEM_H
