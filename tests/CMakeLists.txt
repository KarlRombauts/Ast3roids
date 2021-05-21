set(BINARY ${CMAKE_PROJECT_NAME}_test)

add_executable(${BINARY} QuaternionTest.cpp VectorTest.cpp ../src/Components/Camera.h ../src/Components/Position.h ../src/Components/Plane.h ../src/Factory/IcoSphere.cpp ../src/Factory/IcoSphere.h ../src/Components/Geometry.h ../src/Helpers/ObjParser.cpp ../src/Helpers/ObjParser.h ObjParserTest.cpp ../src/Helpers/NoiseDistortion.cpp ../src/Helpers/NoiseDistortion.h ../src/Helpers/Normals.cpp ../src/Helpers/Normals.h)

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest gtest_main)
