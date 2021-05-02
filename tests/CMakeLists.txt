set(BINARY ${CMAKE_PROJECT_NAME}_test)

add_executable(${BINARY} QuaternionTest.cpp VectorTest.cpp ../RayCaster.cpp ../RayCaster.h ../src/Components/Camera.h ../src/Components/Position.h)

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest gtest_main)
