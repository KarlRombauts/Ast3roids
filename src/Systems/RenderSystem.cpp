#include <Components/Camera.h>
#include <Components/Position.h>
#include <Components/Plane.h>
#include <Components/Wall.h>
#include <Components/Asteroid.h>
#include <Components/Geometry.h>
#include <Components/Scale.h>
#include <Components/Skybox.h>
#include <Components/Transparency.h>
#include "RenderSystem.h"
#include "../OpenGL.h"
#include "../Components/Line.h"
#include "../Components/Health.h"
#include "../Components/Particle.h"
#include "../GameModel.h"
#include "../Helpers.h"
#include "../Components/Rotation.h"
#include "Components/Material.h"
#include <algorithm>
#include <Components/AnimatedTexture.h>
#include <Components/Light.h>

bool compareDistanceFromCamera(Entity *a, Entity *b) {
    Quaternion camRotation = gameModel.activeCamera->get<Rotation>()->rotation;
    Vector3 camPosition = gameModel.activeCamera->get<Position>()->position;
    Vector3 camForward = camRotation * Vector3::forward();

    Vector3 posA = a->get<Position>()->position;
    Vector3 posB = b->get<Position>()->position;

    double distA = Vector3::fromTo(camPosition, posA).dot(camForward);
    double distB = Vector3::fromTo(camPosition, posB).dot(camForward);

    return distA < distB;
}

void RenderSystem::update(EntityManager &entities, double dt) {


    glLoadIdentity();
    //    renderLight(nullptr);
    switch (gameModel.state) {
        case GameState::START:
            renderString(gameModel.width / 2, gameModel.height / 2, "Press SPACE BAR to start...",
                         TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_18);
            break;
        case GameState::GAME_OVER:
        case GameState::PLAY_AGAIN:
            renderString(gameModel.width / 2, gameModel.height / 2, "Game Over. Press SPACE BAR to play again...",
                         TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_18);
            break;
        case GameState::WAVE_OVER:
        case GameState::PLAYING:
        case GameState::GAME_OVER_TRANSITION:
            glEnable(GL_LIGHTING);
            applyCameraRotation();
            drawSkyBox(entities);
            applyCameraPosition();

            renderLights(entities);

            drawEntities(entities);
            drawTransparentEntities(entities);
            glLoadIdentity();
            drawScore();
            glDisable(GL_LIGHTING);
            break;

    }
}

void RenderSystem::renderLights(EntityManager &entities) const {
    GLfloat lm_ambient[] = {0.2, 0.2, 0.2, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);

    for (int i = 0; i < GL_MAX_LIGHTS; i++) {
        glDisable(GL_LIGHT0 + i);
    }

    int i = 0;
    for (Entity *entity : entities.getEntitiesWith<Light, Position>()) {
        renderLight(entity, i++);
    }
}

void RenderSystem::renderLight(Entity *entity, int i) const {
    Light *light = entity->get<Light>();
    Vector3 &p = entity->get<Position>()->position;
    GLfloat position[] = {
            (GLfloat) p.x,
            (GLfloat) p.y,
            (GLfloat) p.z,
            (GLfloat) 1
    };
    glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
    glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light->ambient);
    glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light->specular);
    glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light->diffuse);
    glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, light->attenuation);
    glEnable(GL_LIGHT0 + i);
}

void RenderSystem::drawEntities(EntityManager &entities) {

    for (Entity *entity: entities.getEntitiesWith<Position, Rotation, Scale>()) {
        // Skip objects that have transparency so that we can draw them last
        if (entity->has<Transparency>()) continue;

        drawEntity(entity);
    }
}

void RenderSystem::drawTransparentEntities(EntityManager &entities) {
    std::vector<Entity *> transparentEntities = entities.getEntitiesWith<Position, Rotation, Scale, Transparency>();
    std::sort(transparentEntities.begin(), transparentEntities.end(),
              compareDistanceFromCamera);

    for (Entity *entity: transparentEntities) {
        drawEntity(entity);
    }
}

void RenderSystem::drawEntity(Entity *entity) {
    glPushMatrix();

    applyTransformations(entity);

    if (entity->has<Plane, Wall>()) {
        drawGridPlane(entity);
    } else if (entity->has<Geometry>()) {
        drawShape(entity);
    }

    glPopMatrix();
}

void RenderSystem::applyTransformations(Entity *entity) {
    Vector3 &position = entity->get<Position>()->position;
    Vector3 &scale = entity->get<Scale>()->scale;
    Quaternion &rotation = entity->get<Rotation>()->rotation;

    glTranslatef(position.x, position.y, position.z);
    glScalef(scale.x, scale.y, scale.z);
    glRotateQuaternion(rotation);
}

void RenderSystem::applyTransformations(const Shape &shape) const {
    const Vector3 &position = shape.position;
    const Vector3 &scale = shape.scale;
    const Quaternion &rotation = shape.rotation;

    glTranslatef(position.x, position.y, position.z);
    glScalef(scale.x, scale.y, scale.z);
    glRotateQuaternion(rotation);
}

void RenderSystem::drawScore() {
    glColor3f(1, 1, 1);
    int arenaSize = gameModel.arenaSize;
    renderString(10, gameModel.height - 20,
                 "Score: " + std::to_string(gameModel.score),
                 TextAlignment::LEFT, GLUT_BITMAP_HELVETICA_18);

    renderString(gameModel.width / 2, gameModel.height - 20,
                 "Wave: " + std::to_string(gameModel.waveCount),
                 TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_18);

    renderString(gameModel.width - 10, gameModel.height - 20, "Time: " + formatTime(
            gameModel.elapsedTime - gameModel.resetTime),
                 TextAlignment::RIGHT, GLUT_BITMAP_HELVETICA_18);
}

void
RenderSystem::renderString(GLdouble x, GLdouble y, const std::string &string,
                           TextAlignment alignment, void **font) {
    Material material;
    material.setEmission(1, 1, 1);

    applyMaterial(&material);
    glMaterialf(GL_FRONT, GL_EMISSION, 1);
    double offset = getTextOffset(string, font, alignment);

    x = map(x - offset, {0, gameModel.width}, {-1, 1});
    y = map(y, {0, gameModel.height}, {-1, 1});

    double z = 10; // Value is not important just set text at some z depth
    x = z * tan(30 * M_PI / 180) * x * gameModel.aspectRatio;
    y = z * tan(30 * M_PI / 180) * y;

    glRasterPos3d(x, y, -z);

    for (char n : string) {
        glutBitmapCharacter(font, n);
    }
}

double RenderSystem::getTextOffset(const std::string &string, void **font,
                                   const TextAlignment &alignment) const {
    double width = getStringWidth(string, font);

    double offset = 0;
    if (alignment == TextAlignment::CENTER) {
        offset = width / 2;
    } else if (alignment == TextAlignment::RIGHT) {
        offset = width;
    }

    return offset;
}

double
RenderSystem::getStringWidth(const std::string &string, void **font) const {
    double width = 0;

    for (char n : string) {
        width += glutBitmapWidth(font, n);
    }

    return width;
}


void RenderSystem::drawParticle(Entity *entity) const {
    int size = entity->get<Particle>()->size;
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);
    glEnd();
}

void RenderSystem::drawAxis() const {
    float colorx[4] = {1, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colorx);
    drawLine({0, 0, 0}, {3, 0, 0});

    float colory[4] = {0, 1, 0, 1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colory);
    drawLine({0, 0, 0}, {0, 3, 0});

    float colorz[4] = {0, 0, 1, 1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colorz);
    drawLine({0, 0, 0}, {0, 0, 3});
}

void RenderSystem::drawLine(Entity *entity) const {
    Line *line = entity->get<Line>();
    glLineWidth(2.0);
    glBegin(GL_LINES);
    {
        glVertex3f(line->start.x, line->start.y, 0);
        glVertex3f(line->end.x, line->end.y, 0);
    }
    glEnd();
}

void RenderSystem::drawShape(Entity *entity) const {
    Geometry *geometry = entity->get<Geometry>();

    glEnable(GL_TEXTURE_2D);
    for (Face &face: geometry->faces) {
        applyMaterial(face.material);
        glPushMatrix();
        applyTransformations(geometry->shapes[face.shapeIndex]);
        drawFace(entity, face);
        glPopMatrix();
    }
    glDisable(GL_TEXTURE_2D);
}

void RenderSystem::drawFace(Entity *entity, const Face &face) const {
    Geometry *geometry = entity->get<Geometry>();
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 3; i++) {
        if (!geometry->uvs.empty()) {
            const Vector2 &t1 = geometry->uvs[face.uvIndices[i]];

            if (entity->has<AnimatedTexture>()) {
                AnimatedTexture *animTex = entity->get<AnimatedTexture>();
                double offsetX =
                        ((double) 1 / animTex->cols) * animTex->colOffset;
                double offsetY =
                        ((double) 1 / animTex->rows) * animTex->rowOffset;
                glTexCoord2d(t1.x + offsetX, t1.y + offsetY);
            } else {
                glTexCoord2d(t1.x, t1.y);
            }
        }

        const Vector3 &n1 = geometry->normals[face.vertIndices[i]];
        glNormal3d(n1.x, n1.y, n1.z);

        const Vector3 &v1 = geometry->vertices[face.vertIndices[i]];
        glVertex3d(v1.x, v1.y, v1.z);
    }
    glEnd();
}

void RenderSystem::applyMaterial(const Material *material) const {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material->diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material->ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material->specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, material->emission);
    glBindTexture(GL_TEXTURE_2D, material->textureId);
}

void RenderSystem::drawGridPlane(Entity *entity) const {
    Plane *plane = entity->get<Plane>();

    if (!entity->has<Material>()) {
        entity->assign<Material>();
    }
    Material *material = entity->get<Material>();
    applyMaterial(material);

    glLineWidth(2.0);
    int numLines = 10;

    // Vertical grid lines
    for (int i = 0; i <= numLines; i++) {
        double t = (double) i / (double) numLines;
        Vector3 start = Vector3::lerp(plane->bottomLeft, plane->bottomRight, t);
        Vector3 end = Vector3::lerp(plane->topLeft, plane->topRight, t);
        drawLine(start, end);
    }

    // Horizontal grid lines
    for (int i = 0; i <= numLines; i++) {
        double t = (double) i / (double) numLines;
        Vector3 start = Vector3::lerp(plane->bottomLeft, plane->topLeft, t);
        Vector3 end = Vector3::lerp(plane->bottomRight, plane->topRight, t);
        drawLine(start, end);
    }
}

void RenderSystem::drawLine(const Vector3 &start, const Vector3 &end) const {
    glBegin(GL_LINES);
    glVertex3d(start.x, start.y, start.z);
    glVertex3d(end.x, end.y, end.z);
    glEnd();
}

void RenderSystem::drawHealthBars(Entity *entity) const {
    Vector3 &position = entity->get<Position>()->position;
    Vector3 &scale = entity->get<Position>()->position;

    glPushMatrix();
    glTranslatef(position.x, position.y, 0);
    glScalef(scale.x, scale.y, 1);

    int width = 10;
    Health *health = entity->get<Health>();
    double healthWidth =
            width * ((double) health->health / (double) health->maxHealth);

    glBegin(GL_POLYGON);
    {
        glColor3f(1, 1, 1);
        glVertex3f(-5, -0.2, 0);
        glVertex3f(5, -0.2, 0);
        glVertex3f(5, 0.2, 0);
        glVertex3f(-5, 0.2, 0);

        glColor3f(1, 0, 0);
        glVertex3f(-5, -0.2, 0);
        glVertex3f(healthWidth - 5, -0.2, 0);
        glVertex3f(healthWidth - 5, 0.2, 0);
        glVertex3f(-5, 0.2, 0);
    }
    glEnd();
    glPopMatrix();
}

void RenderSystem::drawDifficulty() {
    int arenaSize = gameModel.arenaSize;
    switch (gameModel.difficulty) {
        case Difficulty::HARD:
            glColor3f(1, 0, 0);
            return renderString(0, 0, "Difficulty: HARD",
                                TextAlignment::CENTER,
                                GLUT_BITMAP_HELVETICA_18);
        case Difficulty::EASY:
            glColor3f(1, 1, 1);
            return renderString(gameModel.width / (double) 2, gameModel.height - 18, "Difficulty: EASY",
                                TextAlignment::CENTER,
                                GLUT_BITMAP_HELVETICA_18);
    }
}

void RenderSystem::glRotateQuaternion(const Quaternion &q) const {
    double w = q.w;
    double x = q.v.x;
    double y = q.v.y;
    double z = q.v.z;

    double matrix[16] = {1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0,
                         0, 0, 0, 1};


    double sqw = w * w;
    double sqx = x * x;
    double sqy = y * y;
    double sqz = z * z;

    // Column 1
    matrix[0] = sqw + sqx - sqy - sqz;
    matrix[1] = (2 * x * y) + (2 * w * z);
    matrix[2] = (2 * x * z) - (2 * w * y);

    // Column 2
    matrix[4] = (2 * x * y) - (2 * w * z);
    matrix[5] = sqw - sqx + sqy - sqz;
    matrix[6] = (2 * y * z) + (2 * w * x);

    // Column 3
    matrix[8] = (2 * x * z) + (2 * w * y);
    matrix[9] = (2 * y * z) - (2 * w * x);
    matrix[10] = sqw - sqx - sqy + sqz;

    glMultMatrixd(matrix);
}

void RenderSystem::drawSkyBox(EntityManager &entities) const {
    glDisable(GL_DEPTH_TEST);

    auto skyBoxes = entities.getEntitiesWith<Skybox, Geometry>();
    if (!skyBoxes.empty()) {
        Entity *skybox = skyBoxes[0];
        drawShape(skybox);
    }

    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::applyCameraRotation() {
    if (gameModel.activeCamera) {
        Quaternion &rotation = gameModel.activeCamera->get<Rotation>()->rotation;
        glRotateQuaternion(rotation.conjugate());
    }
}

void RenderSystem::applyCameraPosition() const {
    if (gameModel.activeCamera) {
        Vector3 &position = gameModel.activeCamera->get<Position>()->position;
        glTranslated(-position.x, -position.y, -position.z);
    }
}
