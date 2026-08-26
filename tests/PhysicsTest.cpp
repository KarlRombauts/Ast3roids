#include <gtest/gtest.h>
#include <Systems/PhysicsSystem.h>
#include <ecs/EntityManager.h>
#include <Components/Kinematics.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Globals.h>

namespace {
    const double SHIP_DRAG = 1.0; // matches SpaceShipFactory

    // Flies a ship flat out for `seconds` at a fixed frame time and returns the
    // speed it settled at.
    double topSpeed(int frameMillis, double seconds) {
        EntityManager entities;
        PhysicsSystem physics;

        Entity *ship = entities.create();
        ship->assign<Position>(Vector3());
        ship->assign<Rotation>();
        ship->assign<Kinematics>(Vector3(), Vector3(), 1);
        ship->get<Kinematics>()->drag = SHIP_DRAG;

        int frames = (int) (seconds * 1000 / frameMillis);
        for (int i = 0; i < frames; i++) {
            // What PlayerInputSystem does while the thrust key is held.
            ship->get<Kinematics>()->acceleration += Vector3(0, 0, -gameConfig.PLAYER_THRUST);
            physics.update(entities, frameMillis);
        }
        return ship->get<Kinematics>()->velocity.magnitude();
    }
}

// Thrust is an acceleration, so how fast the ship ends up going must not depend
// on how often the game happens to update. It used to: the thrust was scaled by
// the frame time a second time, which made a 120 fps machine fly at half the
// speed of a 60 fps one.
TEST(PhysicsTest, TopSpeedIsTheSameAtAnyFrameRate) {
    double at30 = topSpeed(33, 10);
    double at60 = topSpeed(16, 10);
    double at240 = topSpeed(4, 10);

    EXPECT_NEAR(at30, at60, 0.5);
    EXPECT_NEAR(at60, at240, 0.5);
}

// And that speed is the one the config describes: thrust divided by drag.
TEST(PhysicsTest, TopSpeedIsThrustOverDrag) {
    double expected = gameConfig.PLAYER_THRUST / SHIP_DRAG;
    EXPECT_NEAR(topSpeed(16, 10), expected, expected * 0.02);
}

// Let go and drag bleeds the speed off: most of it inside 1 / drag seconds.
TEST(PhysicsTest, DragSlowsTheShipWhenThrustStops) {
    EntityManager entities;
    PhysicsSystem physics;

    Entity *ship = entities.create();
    ship->assign<Position>(Vector3());
    ship->assign<Rotation>();
    ship->assign<Kinematics>(Vector3(0, 0, -gameConfig.PLAYER_THRUST), Vector3(), 1);
    ship->get<Kinematics>()->drag = SHIP_DRAG;

    for (int i = 0; i < 60; i++) { // one second of coasting at 60 fps
        physics.update(entities, 16);
    }

    double remaining = ship->get<Kinematics>()->velocity.magnitude();
    EXPECT_LT(remaining, gameConfig.PLAYER_THRUST * 0.45);
    EXPECT_GT(remaining, gameConfig.PLAYER_THRUST * 0.25);
}
