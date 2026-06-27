#ifndef UNTITLED_GAMEMODEL_H
#define UNTITLED_GAMEMODEL_H

#include "Coordinates.h"
#include "Arena.h"
#include "Matrix4.h"

enum class GameState {
    GAME_OVER,
    WAVE_OVER,
    PLAYING,
    START,
    PLAY_AGAIN,
    GAME_OVER_TRANSITION,
};

enum class Difficulty {
    EASY,
    HARD
};

struct GameModel {
    int worldSize = 100;
    int arenaSize = worldSize;
    int waveCount = 0;
    int elapsedTime = 0;
    int score = 0;
    int resetTime = 0;
    int timeSinceGameOver = 0;

    Entity *activeCamera = nullptr;

    Arena arena;

    Difficulty difficulty = Difficulty::EASY;

    GameState state = GameState::START;

    int width;
    int height;
    double aspectRatio;
    Matrix4 projection;
    CoordinateSpace worldCoordinates;

    void resizeWorld(double aspectRatio);

    void resizeScreen(int w, int h);

    bool isSphereFullyInsideArena(Vector3 pos, double radius);

    double getWorldToPixelRatioWidth();

    void reset();

    bool isSphereFullyOutsideArena(Vector3 pos, double radius);

};

extern GameModel gameModel;

#endif //UNTITLED_GAMEMODEL_H
