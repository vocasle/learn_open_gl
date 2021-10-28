cmake_minimum_required(VERSION 3.20)
project(learn_open_gl)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
set(LIBS ${GLFW3_LIBRARY} GL glad dl)

find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})

add_subdirectory(thirdparties)

add_executable(learn_open_gl src/main.cpp)
target_link_libraries(learn_open_gl ${LIBS})