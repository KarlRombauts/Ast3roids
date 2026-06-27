set(BINARY ${CMAKE_PROJECT_NAME})
set(SOURCES Factory/ImpactFactory.cpp Factory/ImpactFactory.h Factory/ExplosionFactory.cpp Factory/ExplosionFactory.h Factory/BoundingCircleFactory.cpp Factory/BoundingCircleFactory.h Factory/AsteroidFactory.cpp Factory/AsteroidFactory.h Components/LightAnimation.h MaterialLibrary.cpp MaterialLibrary.h Helpers/StbImage.h Components/Skybox.h Components/Transparency.h Helpers/PerlinNoise.hpp Helpers/Normals.cpp Helpers/Normals.h Helpers/NoiseDistortion.cpp Helpers/NoiseDistortion.h Components/Transform.h Vector3.h Arena.cpp Arena.h Helpers/ObjParser.cpp Helpers/ObjParser.h Factory/Primatives/IcoSphere.cpp Factory/Primatives/IcoSphere.h Vector3.cpp Vector2.cpp Vector2.h Components/PlaneCollision.h Systems/SmoothFollowSystem.cpp Systems/SmoothFollowSystem.h MouseState.cpp MouseState.h OpenGL.h KeyboardState.h KeyboardState.cpp Globals.h GameModel.cpp GameModel.h Coordinates.h ecs/Entity.cpp ecs/Entity.h ecs/Component.h ecs/EntityManager.cpp ecs/EntityManager.h ecs/Service.cpp ecs/Service.h Components/PlayerInput.h Systems/RenderSystem.cpp Systems/RenderSystem.h Systems/System.h Systems/PlayerInputSystem.cpp Systems/PlayerInputSystem.h Systems/MouseLookSystem.cpp Systems/MouseLookSystem.h Components/Kinematics.h Systems/PhysicsSystem.cpp Systems/PhysicsSystem.h Coordinates.cpp Components/Color.h Systems/CollisionSystem.cpp Systems/CollisionSystem.h Components/Collision.h Components/Asteroid.h Components/SpaceShip.h Components/Bullet.h Helpers.cpp Helpers.h Components/Destroy.h Systems/OutOfBoundsSystem.cpp Systems/OutOfBoundsSystem.h Components/FiringBullet.h Systems/FiringSystem.cpp Systems/FiringSystem.h Components/GravityForce.h Components/BlackHole.h Components/Line.h Components/Impact.h Systems/ImpactCleanupSystem.cpp Systems/ImpactCleanupSystem.h Systems/DamageSystem.cpp Systems/DamageSystem.h Components/Damage.h Components/Health.h Components/HealthBar.h Components/SplitOnDeath.h Systems/BulletCleanupSystem.cpp Systems/BulletCleanupSystem.h Components/Child.h Components/BoundingCircle.h Components/Wall.h Systems/WarningSystem.cpp Systems/WarningSystem.h Systems/AsteroidSystem.cpp Systems/AsteroidSystem.h Components/OutsideArena.h Systems/ShipImpactSystem.cpp Systems/ShipImpactSystem.h Components/ParticleSource.h Components/Particle.h Systems/ParticleSystem.cpp Systems/ParticleSystem.h Systems/DestroySystem.cpp Systems/DestroySystem.h GameConfig.h GameConfig.cpp enum/TextAlignment.h Quaternion.cpp Quaternion.h Types.h Components/Rotation.h World.h World.cpp Components/SmoothFollow.h Components/Scale.h Helpers/TextureLoader.cpp Helpers/TextureLoader.h Helpers/MaterialParser.cpp Helpers/MaterialParser.h Factory/Primatives/PlaneFactory.cpp Factory/Primatives/PlaneFactory.h Components/LookAt.h Systems/LookAtSystem.cpp Systems/LookAtSystem.h Components/AnimatedTexture.h Systems/AnimatedTextureSystem.cpp Systems/AnimatedTextureSystem.h Components/Light.h Systems/AnimationSystem.cpp Systems/AnimationSystem.h Factory/BulletFactory.cpp Factory/BulletFactory.h Factory/ArenaFactory.cpp Factory/ArenaFactory.h Factory/SpaceShipFactory.cpp Factory/SpaceShipFactory.h Factory/CameraFactory.cpp Factory/CameraFactory.h)

set(GL_SILENCE_DEPRECATION)

#file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

# Shared sources go in both the executable and the test library.
list(APPEND SOURCES Matrix4.cpp Matrix4.h Render/Shader.cpp Render/Shader.h Render/Mesh.cpp Render/Mesh.h Render/LineMesh.cpp Render/LineMesh.h Platform/Time.cpp Platform/Time.h Factory/Primatives/UVSphere.cpp Factory/Primatives/UVSphere.h)

set(SOURCES ${SOURCES})

# Platform/Window pulls in SDL2 and is app-only, so it lives on the executable,
# not in the static lib the tests link against.
add_executable(${BINARY}_run main.cpp ${SOURCES} Platform/Window.cpp Platform/Window.h)

if (EMSCRIPTEN)
    # SDL2 + WebGL2/GLES3 come from Emscripten; assets/shaders are baked into the
    # virtual filesystem, and main() runs via emscripten_set_main_loop.
    target_compile_options(${BINARY}_run PRIVATE -sUSE_SDL=2 -sFULL_ES3)
    target_link_options(${BINARY}_run PRIVATE
            -sUSE_SDL=2 -sFULL_ES3 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 -sALLOW_MEMORY_GROWTH=1
            "SHELL:-sEXPORTED_FUNCTIONS=['_main','_web_press_space','_web_set_aim']"
            "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/Assets@/Assets"
            "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/Shaders@/Shaders"
            "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/web/shell.html")
    set_target_properties(${BINARY}_run PROPERTIES SUFFIX ".html")
else()
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})

    # The static lib (for tests) mirrors the executable's shared sources. It
    # links OpenGL PUBLIC-ly so the test executable resolves the GL symbols some
    # of those sources (e.g. TextureLoader) reference.
    add_library(${BINARY}_lib STATIC ${SOURCES})
    target_link_libraries(${BINARY}_lib PUBLIC ${OPENGL_LIBRARIES})

    # Copy assets/shaders next to the native binary so the relative paths resolve.
    add_custom_target(copy_assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Assets ${CMAKE_CURRENT_BINARY_DIR}/Assets)
    add_dependencies(${BINARY}_run copy_assets)

    add_custom_target(copy_shaders
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Shaders ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
    add_dependencies(${BINARY}_run copy_shaders)

    target_link_libraries(${BINARY}_run PUBLIC ${OPENGL_LIBRARIES} SDL2::SDL2)
endif()

