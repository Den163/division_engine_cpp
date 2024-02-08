cmake_minimum_required(VERSION 3.5)
project(division_engine_cpp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-c++98-compat -Wno-c++98-compat-pedantic")
set(CMAKE_INSTALL_MESSAGE ALWAYS)

include(FetchContent)

FetchContent_Declare(
    division_engine_core
    GIT_REPOSITORY https://github.com/Den163/division_engine_core
    GIT_TAG 7687281
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.0
)

FetchContent_MakeAvailable(division_engine_core)
FetchContent_MakeAvailable(glm)

set(DIVISION_ENGINE_CPP_SOURCES
    src/core/context_helper.cpp
)

add_library(division_engine_cpp ${DIVISION_ENGINE_CPP_SOURCES})
target_compile_definitions(division_engine_cpp PUBLIC)

target_include_directories(
    division_engine_cpp 
    PUBLIC include 
)
target_link_libraries(division_engine_cpp 
    PUBLIC glm::glm
    PUBLIC division_engine_core
)

if(DEFINED ENV{DIVISION_ENGINE_CPP_EXAMPLES})
    add_subdirectory(examples)
endif()