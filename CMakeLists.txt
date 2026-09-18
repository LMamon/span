cmake_minimum_required(VERSION 3.22)

project(span LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(span
    src/main.cpp
    src/core.cpp
    src/grid.cpp
    src/astar.cpp
    src/agent.cpp
    src/mission.cpp
    src/recorder.cpp
)

target_include_directories(span PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
