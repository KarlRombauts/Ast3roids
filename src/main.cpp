#include <cstdlib>
#include <Systems/MouseLookSystem.h>
#include <Systems/SmoothFollowSystem.h>
#include <Systems/LookAtSystem.h>
#include <Systems/AnimatedTextureSystem.h>
#include <Systems/AnimationSystem.h>
#include "OpenGL.h"
#include "Platform/Window.h"
#include "Platform/Time.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
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

Window window;
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
    window.swap();

#ifdef __EMSCRIPTEN__
    // Push state to the HTML overlay (score/wave/time + menu visibility).
    EM_ASM({
        if (typeof gameUpdate === 'function') { gameUpdate($0, $1, $2, $3); }
    }, (int) gameModel.state, gameModel.score, gameModel.waveCount,
       gameModel.elapsedTime - gameModel.resetTime);
#endif
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
}

void handleGamePlay() {
    int thisTime = Time::millis();
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
        // Drop the space so it isn't read as "fire" on the first gameplay frame.
        keyboardState.clearPressedKeys();
    }
}

#ifdef __EMSCRIPTEN__
// Called from the HTML Play button so it acts like pressing space.
extern "C" EMSCRIPTEN_KEEPALIVE void web_press_space() {
    keyboardState.setPressedKey(' ');
}
#endif

void handleGameOver() {
    keyboardState.clearPressedKeys();
    entities.destroyAll();
    gameModel.activeCamera = nullptr;
    gameModel.timeSinceGameOver = 0;
    gameModel.state = GameState::PLAY_AGAIN;
}

void init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    materialLibrary.init();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    gameModel.resizeScreen(w, h);
}

// Translate one SDL event into the existing keyboard/mouse state. The GLUT
// button/state encoding (left=0/right=2, down=0/up=1) is preserved so
// MouseState/PlayerInput need no changes.
void handleEvent(const SDL_Event &event, bool &running) {
    switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN: {
            SDL_Keycode sym = event.key.keysym.sym;
            if (sym == SDLK_ESCAPE || sym == SDLK_q) {
                running = false;
            } else if (sym >= 0 && sym < 256) {
                keyboardState.setPressedKey((char) sym);
            }
            break;
        }
        case SDL_KEYUP: {
            SDL_Keycode sym = event.key.keysym.sym;
            if (sym >= 0 && sym < 256) {
                keyboardState.releaseKey((unsigned char) sym);
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            int glutButton = event.button.button == SDL_BUTTON_RIGHT ? 2 : 0;
            int glutState = event.type == SDL_MOUSEBUTTONDOWN ? 0 : 1;
            mouseState.onMouseButton(glutButton, glutState, event.button.x, event.button.y);
            break;
        }
        case SDL_MOUSEMOTION:
            if (event.motion.state & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) {
                mouseState.onMouseDrag(event.motion.x, event.motion.y);
            } else {
                mouseState.onMouseMove(event.motion.x, event.motion.y);
            }
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                reshape(event.window.data1, event.window.data2);
            }
            break;
    }
}

static bool running = true;

// One iteration of the game loop. On the web, the browser drives this via
// requestAnimationFrame; natively we call it in a while loop.
void mainLoopFrame() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        handleEvent(event, running);
    }
    idle();
    display();
}

int main(int argc, char **argv) {
    if (!window.create("Asteroids", 600, 600)) {
        return EXIT_FAILURE;
    }

    reshape(600, 600);
    init();

#ifdef __EMSCRIPTEN__
    // 0 fps = use requestAnimationFrame; 1 = simulate an infinite loop.
    emscripten_set_main_loop(mainLoopFrame, 0, 1);
#else
    while (running) {
        mainLoopFrame();
    }
    window.destroy();
#endif
    return EXIT_SUCCESS;
}