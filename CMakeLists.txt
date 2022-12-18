cmake_minimum_required(VERSION 3.20)
project(Snake C)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.c snake.c snake.h geometry.c geometry.h)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

file(
        COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/mixkit-retro-game-notification-212.wav
        DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}
)

#
#cmake_minimum_required(VERSION 3.20)
#project(Snake C)
#
#set(CMAKE_C_STANDARD 99)
#
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(SDL2_LIB REQUIRED IMPORTED_TARGET SDL2)
#
#add_executable(${PROJECT_NAME} main.c snake.c snake.h geometry.c geometry.h)
#
#target_link_libraries(${PROJECT_NAME} PkgConfig::SDL2_LIB)
#