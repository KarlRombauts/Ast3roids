#include <cmath>
#include "GameModel.h"
#include "OpenGL.h"
#include "Globals.h"

GameModel gameModel;

void GameModel::resizeWorld(double aspectRatio) {
    this->aspectRatio = aspectRatio;
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, aspectRatio, 0.1, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glutPostRedisplay();
}

bool GameModel::isSphereFullyInsideArena(Vector3 pos, double radius) {
    return pos.absMaxComponent() + radius < arenaSize;
}

bool GameModel::isSphereFullyOutsideArena(Vector3 pos, double radius) {
    return pos.absMaxComponent() - radius > arenaSize;
}

void GameModel::resizeScreen(int w, int h) {
    width = w;
    height = h;
    GLdouble aspectRatio = (GLfloat) w / (GLfloat) h;
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

