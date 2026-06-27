cmake_minimum_required(VERSION 3.10)
project(StarFox)

set(CMAKE_CXX_STANDARD 17)
set(GL_SILENCE_DEPRECATION)

include_directories(src)

if (EMSCRIPTEN)
    # Emscripten provides SDL2 + WebGL2/GLES3 via link flags (see src/CMakeLists.txt).
    add_subdirectory(src)
else()
    if (APPLE)
        list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    endif()

    find_package(OpenGL REQUIRED)
    find_package(SDL2 REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

    add_subdirectory(src)
    add_subdirectory(tests)
    add_subdirectory(lib/googletest)
endif()

