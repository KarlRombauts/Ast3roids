#include <stdlib.h>
#include <Systems/RayCastingSystem.h>
#include <Systems/SmoothFollowSystem.h>
#include "OpenGL.h"
#include "Globals.h"
#include "GameModel.h"
#include "ecs/EntityManager.h"
#include "Systems/RenderSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/PlayerInputSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/FiringSystem.h"
#include "Components/SpaceShip.h"
#include "Systems/ImpactCleanupSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/BulletCleanupSystem.h"
#include "Systems/WarningSystem.h"
#include "Components/Asteroid.h"
#include "Systems/AsteroidSystem.h"
#include "Systems/OutOfBoundsSystem.h"
#include "Systems/ShipImpactSystem.h"
#include "Systems/ParticleSystem.h"
#include "Systems/BlackHoleSystem.h"
#include "Systems/DestroySystem.h"
#include "Systems/RayCastingSystem.h"

EntityManager entities;
RenderSystem renderSystem;
CollisionSystem collisionSystem;
PhysicsSystem physicsSystem;
PlayerInputSystem playerInputSystem;
FiringSystem firingSystem;
ImpactCleanupSystem impactCleanupSystem;
DamageSystem damageSystem;
BulletCleanupSystem bulletCleanupSystem;
WarningSystem warningSystem;
AsteroidSystem asteroidSystem;
OutOfBoundsSystem outOfBoundsSystem;
ShipImpactSystem shipImpactSystem;
ParticleSystem particleSystem;
BlackHoleSystem blackHoleSystem;
DestroySystem destroySystem;
RayCastingSystem rayCastingSystem;
SmoothFollowSystem smoothFollowSystem;


void handleGameOver();

void handleMenu();

void handleWaveOver();

void handleGamePlay();

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderSystem.update(entities, 0);
    glutSwapBuffers();
}

static void idle() {
    switch (gameModel.state) {
        case GameState::GAME_OVER:
            handleGameOver();
            break;
        case GameState::WAVE_OVER:
            handleWaveOver();
            break;
        case GameState::PLAYING:
            handleGamePlay();
            break;
        case GameState::START:
        case GameState::PLAY_AGAIN:
            handleMenu();
            break;
    }

    glutPostRedisplay();
}

void handleGamePlay() {
    int thisTime = glutGet(GLUT_ELAPSED_TIME);
    int dt = thisTime - gameModel.elapsedTime;

    // if more than 100ms have passed between frames, skip updating to ensure
    // physics engine does not produce side effects
    if (dt > 100) {
        gameModel.elapsedTime = thisTime;
        return;
    }


//    if (keyboardState.isKeyPressed('A')) {
//        camera.pos += Vector3::left() * 50 * dt / 1000;
//    }
//    if (keyboardState.isKeyPressed('D')) {
//        camera.pos += Vector3::right() * 50 * dt / 1000;
//    }
//    if (keyboardState.isKeyPressed('W')) {
//        camera.pos += Vector3::up() * 50 * dt / 1000;
//    }
//    if (keyboardState.isKeyPressed('S')) {
//        camera.pos += Vector3::down() * 50 * dt / 1000;
//    }
//    if (keyboardState.isKeyPressed('I')) {
//        camera.pos += Vector3::forward() * 50 * dt / 1000;
//    }
//    if (keyboardState.isKeyPressed('K')) {
//        camera.pos += Vector3::back() * 50 * dt / 1000;
//    }

//    if(keyboardState.isKeyPressed('I')) {
//        camera.pos += Vector3::back() * 50 * dt / 1000;
//    }
//    if(keyboardState.isKeyPressed('K')) {
//        camera.pos += Vector3::forward() * 50 * dt / 1000;
//    }

    playerInputSystem.update(entities, dt);
    rayCastingSystem.update(entities);
    smoothFollowSystem.update(entities, dt);
//    firingSystem.update(entities, dt);
    collisionSystem.update(entities, dt);
//    particleSystem.update(entities, dt);
    physicsSystem.update(entities, dt);
    warningSystem.update(entities);
//    damageSystem.update(entities);
//    bulletCleanupSystem.update(entities, dt);
//    shipImpactSystem.update(entities);
//    blackHoleSystem.update(entities, dt);
//    outOfBoundsSystem.update(entities);
    impactCleanupSystem.update(entities, dt);
//    destroySystem.update(entities);

    gameModel.elapsedTime = thisTime;

    if (entities.getEntitiesWith<SpaceShip>().empty()) {
        gameModel.state = GameState::GAME_OVER;
    } else if (entities.getEntitiesWith<Asteroid>().empty()) {
        gameModel.state = GameState::WAVE_OVER;
    }
}

void handleWaveOver() {
    asteroidSystem.startWave(entities, ++gameModel.waveCount);
    gameModel.state = GameState::PLAYING;
}

void handleMenu() {
    if (keyboardState.isKeyPressed('1')) {
        gameModel.difficulty = Difficulty::EASY;
    }
    if (keyboardState.isKeyPressed('2')) {
        gameModel.difficulty = Difficulty::HARD;
    }
    if (keyboardState.isKeyPressed(' ')) {
        entities.createWorld();
        gameModel.reset();
        gameModel.state = GameState::PLAYING;
    }
}

void handleGameOver() {
    keyboardState.clearPressedKeys();
    entities.destroyAll();
    gameModel.state = GameState::PLAY_AGAIN;
}

void init() {
    glMatrixMode(GL_PROJECTION);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    gameModel.resizeScreen(w, h);
}

void onKeyPress(unsigned char key, int x, int y) {
    keyboardState.setPressedKey(key);
    switch (key) {
        case 27:
        case 'q':
            exit(EXIT_SUCCESS);
        default:
            break;
    }
}

void onKeyRelease(unsigned char key, int x, int y) {
    keyboardState.releaseKey(key);
}

void onMouseButton(int btn, int state, int x, int y) {
    mouseState.onMouseButton(btn, state, x, y);
}


void onMouseMove(int x, int y) {
    mouseState.onMouseMove(x, y);
}


void onMouseDrag(int x, int y) {
    mouseState.onMouseDrag(x, y);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
//    glEnable(GL_DEPTH_TEST);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Asteroids");
    reshape(500, 600);
    init();

    glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // Keyboard Callbacks
    glutKeyboardFunc(onKeyPress);
    glutKeyboardUpFunc(onKeyRelease);

    // Mouse Callbacks
    glutMouseFunc(onMouseButton);
    glutMotionFunc(onMouseDrag);
    glutPassiveMotionFunc(onMouseMove);

    // Let glut takeover
    glutMainLoop();
}