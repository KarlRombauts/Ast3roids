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

    std::string MODEL_DIR = "./Assets/Models";
    std::string TEXTURE_DIR = "./Assets/Textures";

    // Camera Config
    int MOUSE_SENSITIVITY = 100;
    double CAMERA_STIFFNESS = 3;
    double MAX_CAMERA_DISTANCE = 40;
    double MIN_CAMERA_DISTANCE = 10;
};


#endif //UNTITLED_GAMECONFIG_H
