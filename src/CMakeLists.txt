set(BINARY ${CMAKE_PROJECT_NAME})
set(SOURCES Components/Transform.h Vector3.h Arena.cpp Arena.h Helpers/ObjParser.cpp Helpers/ObjParser.h Factory/IcoSphere.cpp Factory/IcoSphere.h Vector3.cpp Vector2.cpp Vector2.h Components/PlaneCollision.h Systems/SmoothFollowSystem.cpp Systems/SmoothFollowSystem.h MouseState.cpp MouseState.h OpenGL.h KeyboardState.h KeyboardState.cpp Globals.h GameModel.cpp GameModel.h Coordinates.h ecs/Entity.cpp ecs/Entity.h ecs/Component.h ecs/EntityManager.cpp ecs/EntityManager.h ecs/Service.cpp ecs/Service.h Components/PlayerInput.h Systems/RenderSystem.cpp Systems/RenderSystem.h Systems/System.h Systems/PlayerInputSystem.cpp Systems/PlayerInputSystem.h Systems/RayCastingSystem.cpp Systems/RayCastingSystem.h Components/Kinematics.h Systems/PhysicsSystem.cpp Systems/PhysicsSystem.h Coordinates.cpp Components/Texture.h Systems/CollisionSystem.cpp Systems/CollisionSystem.h Components/Collision.h Components/Asteroid.h Components/SpaceShip.h Components/Bullet.h Helpers.cpp Helpers.h Components/Destroy.h Systems/OutOfBoundsSystem.cpp Systems/OutOfBoundsSystem.h Components/FiringBullet.h Systems/FiringSystem.cpp Systems/FiringSystem.h Components/GravityForce.h Components/BlackHole.h Components/Shape.h Components/Line.h Components/Impact.h Systems/ImpactCleanupSystem.cpp Systems/ImpactCleanupSystem.h Systems/DamageSystem.cpp Systems/DamageSystem.h Components/Damage.h Components/Health.h Components/HealthBar.h Components/SplitOnDeath.h Systems/BulletCleanupSystem.cpp Systems/BulletCleanupSystem.h Components/Child.h Components/BoundingCircle.h Components/Wall.h Systems/WarningSystem.cpp Systems/WarningSystem.h Systems/AsteroidSystem.cpp Systems/AsteroidSystem.h Components/OutsideArena.h Systems/ShipImpactSystem.cpp Systems/ShipImpactSystem.h Components/ParticleSource.h Components/Particle.h Systems/ParticleSystem.cpp Systems/ParticleSystem.h Systems/BlackHoleSystem.cpp Systems/BlackHoleSystem.h Systems/DestroySystem.cpp Systems/DestroySystem.h GameConfig.h GameConfig.cpp enum/TextAlignment.h Quaternion.cpp Quaternion.h Types.h Components/Rotation.h World.h World.cpp Components/SmoothFollow.h)

set(GL_SILENCE_DEPRECATION)

find_package(OpenGL REQUIRED)

find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES})

#file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

set(SOURCES ${SOURCES})

add_executable(${BINARY}_run main.cpp ${SOURCES})

add_library(${BINARY}_lib STATIC ${SOURCES})

target_link_libraries(${BINARY}_run PUBLIC ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

