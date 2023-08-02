cmake_minimum_required(VERSION 3.16)
project(v-cloud)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/glew)
add_subdirectory(external/glfw)
file(GLOB SRC_FILES
        src/*.c
        src/*.h
        src/*.cpp)

add_executable(v-cloud
        ${SRC_FILES})

target_include_directories(v-cloud PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include)
target_include_directories(v-cloud PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glew/include)
target_link_libraries(v-cloud glfw libglew_static)
