#include <Components/Kinematics.h>
#include <Components/Position.h>
#include <Components/Rotation.h>
#include <Components/Geometry.h>
#include <Components/Scale.h>
#include <Factory/Primatives/PlaneFactory.h>
#include <Globals.h>
#include <Components/Transparency.h>
#include <Components/Collision.h>
#include <Components/LookAt.h>
#include <GameModel.h>
#include <Components/Bullet.h>
#include <Components/Damage.h>
#include <Components/AnimatedTexture.h>
#include "BulletFactory.h"

Entity *BulletFactory::create(EntityManager &entities, Vector3 position,
                              Vector3 velocity) {
    Entity *bullet = entities.create();

    bullet->assign<Bullet>();
    bullet->assign<Damage>(gameConfig.BULLET_DAMAGE);

    setKinematics(bullet, velocity);
    setTransformations(bullet, position);
    setGeometry(bullet);
    setCollisions(bullet);

    return bullet;
}

void BulletFactory::setCollisions(Entity *bullet) {
    bullet->assign<Collision>(CollisionType::TRIGGER);
    bullet->assign<CircleCollision>(1);
}

void BulletFactory::setTransformations(Entity *bullet, Vector3 &position) {
    bullet->assign<Position>(position);
    bullet->assign<Rotation>();
    bullet->assign<Scale>();
    bullet->assign<LookAt>(gameModel.activeCamera);
}

void BulletFactory::setGeometry(Entity *bullet) {
    bullet->assign<Geometry>(PlaneFactory::create(materialLibrary.BULLET));
    bullet->assign<Transparency>();
    bullet->assign<AnimatedTexture>(6, 4, AnimationBehaviour::DEATH);
}

void BulletFactory::setKinematics(Entity *bullet, Vector3 &velocity) {
    Kinematics kinematics;
    kinematics.velocity = velocity;

    bullet->assign<Kinematics>(kinematics);
}
