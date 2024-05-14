cmake_minimum_required(VERSION 3.5)
project(division_engine VERSION 0.1.0 LANGUAGES C CXX)

set(IGNORE_ERRORS 
    "-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-bitwise-instead-of-logical"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall ${IGNORE_ERRORS}")
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

set(DIVISION_SHADER_COMPILER_EXECUTABLE ON CACHE BOOL "Build shader compiler executable")

add_subdirectory(division_engine_core)
add_subdirectory(tools/division_shader_compiler)

set(DIVISION_ENGINE_SOURCES
    src/core/context.cpp
    src/core/core_runner.cpp
    src/core/font_texture.cpp
    src/core/render_pass_descriptor_builder.cpp
    src/core/render_pass_instance_builder.cpp
    src/canvas/rect_drawer.cpp
    src/canvas/render_queue.cpp
    src/canvas/text_drawer.cpp
)

add_library(division_engine ${DIVISION_ENGINE_SOURCES})
target_compile_definitions(division_engine PUBLIC)

target_include_directories(
    division_engine 
    PUBLIC include 
    PRIVATE include/division_engine
)
target_link_libraries(division_engine
    PUBLIC glm::glm
    PUBLIC division_engine_core
    PUBLIC flecs::flecs_static
)

if(DEFINED ENV{DIVISION_ENGINE_CPP_EXAMPLES})
    add_subdirectory(examples)
endif()

### Compiling shaders from GLSL to MSL

file(
    GLOB_RECURSE 
    DIVISION_RESOURCES_GLOB
    CONFIGURE_DEPENDS
    "${CMAKE_BINARY_DIR}/*/resources/shaders/*.vert" 
    "${CMAKE_BINARY_DIR}/*/resources/shaders/*.frag"
)

foreach(D_FILE_ABS ${DIVISION_RESOURCES_GLOB})
    file(RELATIVE_PATH D_FILE_REL ${CMAKE_BINARY_DIR} ${D_FILE_ABS})
    get_filename_component(D_FILE_EXT ${D_FILE_REL} EXT)
    get_filename_component(D_FILE_NAME ${D_FILE_REL} NAME)

    if (${D_FILE_EXT} STREQUAL ".vert")
        set(SHADER_TYPE "vertex")
    elseif(${D_FILE_EXT} STREQUAL ".frag")
        set(SHADER_TYPE "fragment")
    else()
        continue()
    endif()

    set(D_FILE_ABS_OUTPUT ${D_FILE_ABS}.metal)
    add_custom_command(
        OUTPUT ${D_FILE_ABS_OUTPUT}
        COMMAND ${CMAKE_BINARY_DIR}/tools/division_shader_compiler/division_shader_compiler 
                --${SHADER_TYPE} ${D_FILE_ABS} ${D_FILE_ABS_OUTPUT}
        DEPENDS ${D_FILE_ABS}
        COMMENT "Recompiling ${D_FILE_ABS} to MSL..."
        VERBATIM
    )

    list(APPEND SHADER_SOURCES ${D_FILE_ABS_OUTPUT})
endforeach()

add_custom_target(division_engine_shaders DEPENDS division_shader_compiler ${SHADER_SOURCES})
add_dependencies(division_engine division_engine_shaders)