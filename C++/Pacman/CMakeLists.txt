cmake_minimum_required(VERSION 3.20)
project(Pacman VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System Audio)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Pacman ${SOURCES})

target_include_directories(Pacman PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Pacman PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)
