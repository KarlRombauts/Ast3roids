#include <cmath>
#include "GameModel.h"
#include "Globals.h"
#include "Quaternion.h"
#include "ecs/Entity.h"
#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Platform/Time.h"

GameModel gameModel;

Matrix4 GameModel::viewMatrix() const {
    Vector3 &pos = activeCamera->get<Position>()->position;
    Quaternion &rot = activeCamera->get<Rotation>()->rotation;

    // Looking "from" the camera means transforming the world by the inverse of
    // the camera's transform: undo its rotation (conjugate) then undo its
    // translation. Right-to-left, the translation is applied first.
    return Matrix4::fromQuaternion(rot.conjugate())
           * Matrix4::translation(Vector3(-pos.x, -pos.y, -pos.z));
}

void GameModel::resizeWorld(double aspectRatio) {
    this->aspectRatio = aspectRatio;
    projection = Matrix4::perspective(60, aspectRatio, 0.1, 10000);
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
    double aspectRatio = (double) w / (double) h;
    gameModel.resizeWorld(aspectRatio);
}

double GameModel::getWorldToPixelRatioWidth() {
    return (double) width / (worldCoordinates.maxX * 2);
}

void GameModel::reset() {
    waveCount = 0;

    // Both clocks are taken from the same reading, so the elapsed-time readout
    // is "millis into this run" however many times a run gets started. Taking
    // resetTime from the previous run's elapsedTime instead made the second and
    // later runs read as time since the process started.
    elapsedTime = Time::millis();
    resetTime = elapsedTime;

    score = 0;
    state = GameState::PLAYING;
}

