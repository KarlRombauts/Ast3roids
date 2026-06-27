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

void RenderSystem::update(EntityManager &entities, double dt) {
    glLoadIdentity();

    switch (gameModel.state) {
        case GameState::START:
            drawStartScreen();
            break;
        case GameState::GAME_OVER:
        case GameState::PLAY_AGAIN:
            drawGameOver();
            break;
        case GameState::WAVE_OVER:
        case GameState::PLAYING:
        case GameState::GAME_OVER_TRANSITION:
            renderMainGame(entities);
            break;
    }

    updateFps();
    drawFps();
}

void RenderSystem::updateFps() {
    int now = glutGet(GLUT_ELAPSED_TIME);

    // First frame: just start the timing window, no sample yet.
    if (lastFpsUpdateTime == 0) {
        lastFpsUpdateTime = now;
        return;
    }

    frameCount++;
    int elapsed = now - lastFpsUpdateTime;

    // Recompute the displayed value at most twice a second so it stays readable.
    if (elapsed >= 500) {
        currentFps = frameCount * 1000.0 / elapsed;
        frameCount = 0;
        lastFpsUpdateTime = now;
    }
}

void RenderSystem::drawFps() {
    glColor3f(1, 1, 1);
    renderString(10, 20, "FPS: " + std::to_string((int) currentFps),
                 TextAlignment::LEFT, GLUT_BITMAP_HELVETICA_18);
}

void RenderSystem::renderMainGame(EntityManager &entities) {
    glEnable(GL_LIGHTING);

    applyCameraRotation();
    drawSkyBox(entities);
    applyCameraPosition();

    renderLights(entities);
    renderEntities(entities);
    renderTransparentEntities(entities);

    glLoadIdentity();
    drawScore();
    glDisable(GL_LIGHTING);
}

void RenderSystem::drawGameOver() {
    renderString(gameModel.width / 2, gameModel.height / 2 + 30, "GAME OVER",
                 TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_18);

    renderString(gameModel.width / 2, gameModel.height / 2 + 10 , "Press SPACE BAR to play again...",
                 TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_12);

    drawControls();
}

void RenderSystem::drawStartScreen() {
    int centerX = gameModel.width / 2;
    renderString(centerX, gameModel.height / 2 + 30, "AST3ROIDS",
                 TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_18);

    renderString(centerX, gameModel.height / 2 + 10, "Press SPACE BAR to start...",
                 TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_12);

    drawControls();
}

void RenderSystem::drawControls() {
    int centerX = gameModel.width / 2;
    renderString(centerX, gameModel.height / 2 - 50, "Controls",
                 TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_12);

    renderString(centerX, gameModel.height / 2 - 70, "F = Forward, A = Roll Left, D = Roll Right, SPACE = Shoot",
                 TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_12);

    renderString(centerX, gameModel.height / 2 - 90, "Mouse = Aim, I = Camara Forward, K = Camera Back",
                 TextAlignment::CENTER, GLUT_BITMAP_HELVETICA_12);
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
    GLfloat position[] = {(GLfloat) p.x, (GLfloat) p.y, (GLfloat) p.z, (GLfloat) 1};

    glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
    glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light->ambient);
    glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light->specular);
    glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light->diffuse);
    glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, light->attenuation);
    glEnable(GL_LIGHT0 + i);
}

void RenderSystem::renderEntities(EntityManager &entities) {

    for (Entity *entity: entities.getEntitiesWith<Position, Rotation, Scale>()) {
        // Skip objects that have transparency so that we can draw them last
        if (entity->has<Transparency>()) continue;

        drawEntity(entity);
    }
}

void RenderSystem::renderTransparentEntities(EntityManager &entities) {
    std::vector<Entity *> transparentEntities = entities.getEntitiesWith<Position, Rotation, Scale, Transparency>();

    // Transparent billboards must still depth-test against opaque geometry, but
    // must NOT write depth themselves - otherwise their (partly invisible) quads
    // occlude each other and produce rectangular cut-out artifacts.
    //
    // No back-to-front sort: with depth writes disabled every transparent
    // fragment blends, and the heavily-overlapping sprites (glow particles) are
    // all the same colour, so draw order makes no visible difference here.
    glDepthMask(GL_FALSE);
    for (Entity *entity: transparentEntities) {
        drawEntity(entity);
    }
    glDepthMask(GL_TRUE);
}

void RenderSystem::drawEntity(Entity *entity) {
    glPushMatrix();

    applyTransformations(entity);

    if (entity->has<Plane, Wall>()) {
        drawGridPlane(entity);
    } else if (entity->has<Geometry>()) {
        drawGeometry(entity);
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

void RenderSystem::renderString(GLdouble x, GLdouble y, const std::string &string,
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

double RenderSystem::getTextOffset(const std::string &string, void **font, const TextAlignment &alignment) const {
    if (alignment == TextAlignment::LEFT) {
        return 0;
    }

    double width = getStringWidth(string, font);

    if (alignment == TextAlignment::CENTER) {
        return width / 2;
    }

    return width;
}

double RenderSystem::getStringWidth(const std::string &string, void **font) const {
    double width = 0;

    for (char n : string) {
        width += glutBitmapWidth(font, n);
    }

    return width;
}

void RenderSystem::drawGeometry(Entity *entity) const {
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
        setTextureCoordinates(entity, face, geometry, i);

        const Vector3 &normal = geometry->normals[face.vertIndices[i]];
        glNormal3d(normal.x, normal.y, normal.z);

        const Vector3 &vert = geometry->vertices[face.vertIndices[i]];
        glVertex3d(vert.x, vert.y, vert.z);
    }
    glEnd();
}

void RenderSystem::setTextureCoordinates(Entity *entity, const Face &face, const Geometry *geometry, int vertIndex) const {
    if (geometry->uvs.empty()) return;

    const Vector2 &uv = geometry->uvs[face.uvIndices[vertIndex]];

    if (entity->has<AnimatedTexture>()) {
        AnimatedTexture *animTex = entity->get<AnimatedTexture>();
        double x = (uv.x + animTex->colOffset) / animTex->cols;
        double y = (uv.y + animTex->rowOffset) / animTex->rows;
        glTexCoord2d(x, y);
    } else {
        glTexCoord2d(uv.x, uv.y);
    }
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

void RenderSystem::drawSkyBox(EntityManager &entities) const {
    glDisable(GL_DEPTH_TEST);

    auto skyBoxes = entities.getEntitiesWith<Skybox, Geometry>();
    if (!skyBoxes.empty()) {
        Entity *skybox = skyBoxes[0];
        drawGeometry(skybox);
    }

    glEnable(GL_DEPTH_TEST);
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
