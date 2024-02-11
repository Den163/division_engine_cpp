cmake_minimum_required(VERSION 3.5)
project(division_engine_cpp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-c++98-compat -Wno-c++98-compat-pedantic")
set(CMAKE_INSTALL_MESSAGE ALWAYS)

include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.0
)

FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs
    GIT_TAG v3.2.11
)

FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(flecs)

add_subdirectory(division_engine_core)

set(DIVISION_ENGINE_CPP_SOURCES
    src/core/context_helper.cpp
    src/core/render_pass_descriptor_builder.cpp
    src/core/render_pass_instance_builder.cpp
    src/canvas/rect_drawer.cpp
    src/canvas/render_queue.cpp
)

add_library(division_engine_cpp ${DIVISION_ENGINE_CPP_SOURCES})
target_compile_definitions(division_engine_cpp PUBLIC)

target_include_directories(
    division_engine_cpp 
    PUBLIC include 
    PRIVATE include/division_engine
)
target_link_libraries(division_engine_cpp 
    PUBLIC glm::glm
    PUBLIC division_engine_core
    PUBLIC flecs::flecs_static
)

if(DEFINED ENV{DIVISION_ENGINE_CPP_EXAMPLES})
    add_subdirectory(examples)
endif()