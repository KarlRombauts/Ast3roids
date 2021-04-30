#include "GameModel.h"
#include "OpenGL.h"

GameModel gameModel;

void GameModel::resizeWorld(double aspectRatio) {
    if (aspectRatio > 1)
        resizeWorld(-worldSize, worldSize, -worldSize * aspectRatio, worldSize * aspectRatio);
    else
        resizeWorld(-worldSize / aspectRatio, worldSize / aspectRatio, -worldSize, worldSize);
}

void GameModel::resizeWorld(double minX, double maxX, double minY, double maxY) {
    worldCoordinates.minX = minX;
    worldCoordinates.maxX = maxX;
    worldCoordinates.minY = minY;
    worldCoordinates.maxY = maxY;

    glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        glOrtho(minX, maxX, minY, maxY, -60.0, 60.0);

    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

    glutPostRedisplay();
}

bool GameModel::isCircleInArena(Vec3 pos, double radius) {
    return (pos.x - radius) > -arenaSize &&
           (pos.x + radius) < arenaSize &&
           (pos.y - radius > -arenaSize) &&
           (pos.y + radius) < arenaSize;
}

void GameModel::resizeScreen(int w, int h) {
    width = w;
    height = h;
    GLdouble aspectRatio = (GLfloat) h / (GLfloat) w;
    gameModel.resizeWorld(aspectRatio);
}

double GameModel::getWorldToPixelRatioWidth() {
    return (double) width / (worldCoordinates.maxX * 2);
}

void GameModel::reset() {
    waveCount = 0;
    resetTime = elapsedTime;
    elapsedTime = 0;
    score = 0;
    state = GameState::PLAYING;
}

