#include <cstdlib>
#include <Systems/MouseLookSystem.h>
#include <Systems/SmoothFollowSystem.h>
#include <Systems/LookAtSystem.h>
#include <Systems/AnimatedTextureSystem.h>
#include <Systems/AnimationSystem.h>
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
#include "Systems/DestroySystem.h"
#include "Systems/MouseLookSystem.h"
#include <filesystem>

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
AnimatedTextureSystem animatedTextureSystem;
AnimationSystem animationSystem;
ShipImpactSystem shipImpactSystem;
ParticleSystem particleSystem;
DestroySystem destroySystem;
MouseLookSystem mouseLookSystem;
SmoothFollowSystem smoothFollowSystem;
LookAtSystem lookAtSystem;


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
        case GameState::START:
        case GameState::PLAY_AGAIN:
            handleMenu();
            break;
        case GameState::PLAYING:
        case GameState::GAME_OVER_TRANSITION:
            handleGamePlay();
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

    smoothFollowSystem.update(entities, dt);
    firingSystem.update(entities, dt);
    lookAtSystem.update(entities);
    collisionSystem.update(entities, dt);
    damageSystem.update(entities);
    particleSystem.update(entities, dt);
    physicsSystem.update(entities, dt);
    warningSystem.update(entities);
    bulletCleanupSystem.update(entities, dt);

    if (gameModel.state == GameState::PLAYING) {
        mouseLookSystem.update(entities, dt);
        playerInputSystem.update(entities, dt);
        shipImpactSystem.update(entities);
    }

    outOfBoundsSystem.update(entities);
    animatedTextureSystem.update(entities, dt);
    animationSystem.update(entities, dt);
    impactCleanupSystem.update(entities, dt);
    destroySystem.update(entities);

    gameModel.elapsedTime = thisTime;

    if (gameModel.state == GameState::GAME_OVER_TRANSITION) {
        gameModel.timeSinceGameOver += dt;

        if (gameModel.timeSinceGameOver > 1500) {
            gameModel.state = GameState::GAME_OVER;
        }
    } else if (entities.getEntitiesWith<Asteroid>().empty()) {
        gameModel.state = GameState::WAVE_OVER;
    }
}

void handleWaveOver() {
    asteroidSystem.startWave(entities, ++gameModel.waveCount);
    gameModel.state = GameState::PLAYING;
}

void handleMenu() {
    if (keyboardState.isKeyPressed(' ')) {
        entities.createWorld();
        gameModel.reset();
        gameModel.state = GameState::PLAYING;
    }
}

void handleGameOver() {
    keyboardState.clearPressedKeys();
    entities.destroyAll();
    gameModel.activeCamera = nullptr;
    gameModel.timeSinceGameOver = 0;
    gameModel.state = GameState::PLAY_AGAIN;
}

void init() {
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_SMOOTH);
    materialLibrary.init();
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