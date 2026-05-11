cmake_minimum_required(VERSION 3.20)
project(BioSimulation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_mixer EXCLUDE_FROM_ALL)

add_executable(BioSimulation src/main.cpp
        include/grid/Grid.h
        src/grid/Grid.cpp
        src/entities/Entities.cpp
        include/entities/Entities.h
)

target_include_directories(BioSimulation PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendored/SDL/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendored/SDL_image/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendored/SDL_ttf/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendored/SDL_mixer/include"
)

target_link_libraries(BioSimulation PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf SDL3_mixer::SDL3_mixer)
