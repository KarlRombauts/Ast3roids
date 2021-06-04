#ifndef UNTITLED_GAMECONFIG_H
#define UNTITLED_GAMECONFIG_H

#include <Helpers/TextureLoader.h>
#include "Components/Color.h"

struct GameConfig {
    // Player Config
    char PLAYER_FORWARD = 'w';
    char PLAYER_LEFT = 'a';
    char PLAYER_RIGHT = 'd';
    char PLAYER_SHOOT = ' ';
    double PLAYER_SPEED = 1000;
    double PLAYER_TURN_SPEED = 180; // Degrees per second

    int PLAYER_FIRING_RATE = 200; // Bullets per second
    int BULLET_DAMAGE = 20; // Hit points
    double BULLET_SPEED = 300;
    Color BULLET_COLOR = {1, 1, 0};

    int EXHAUST_PARTICLE_RATE = 3;
    double EXHAUST_PARTICLE_DECAY = 3;
    double EXPLOSION_DECAY_RATE = 0.5;

    double WARNING_DISTANCE = 40; // Game world units


    // Asteroid Config
    double ASTEROID_MAX_SPEED = 20;
    double ASTEROID_MIN_SPEED = 20;

    double ASTEROID_MAX_ROTATION = 180; // Degrees per second
    double ASTEROID_MIN_ROTATION = -180; // Degrees per second

    int ASTEROID_MIN_START_RADIUS = 6;
    int ASTEROID_MAX_START_RADIUS = 20;
    double ASTEROID_MIN_SIZE = 6;

    Color ASTEROID_COLOR = {1, 1, 1};

    double BLACK_HOLE_PULSE_RATE = 0.5; // times per second;
    int BLACK_HOLE_STRENGTH = 40000;
    Color BLACK_HOLE_COLOR = {0.5, 0.5, 0.5};

    std::string MODEL_DIR = "./Assets/Models";
    std::string TEXTURE_DIR = "./Assets/Textures";
    int MOUSE_SENSITIVITY = 100;
    double CAMERA_STIFFNESS = 3;
};


#endif //UNTITLED_GAMECONFIG_H
