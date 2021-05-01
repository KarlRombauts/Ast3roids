cmake_minimum_required(VERSION 3.10)
project(StarFox)

set(CMAKE_CXX_STANDARD 17)
set(GL_SILENCE_DEPRECATION)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(lib/googletest)

