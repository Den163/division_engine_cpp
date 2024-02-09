add_executable(division_core_example core_example.cpp)
target_link_libraries(division_core_example division_engine_cpp)

add_executable(division_canvas_example canvas_example.cpp)
target_link_libraries(division_canvas_example division_engine_cpp)

file(
    GLOB_RECURSE 
    DIVISION_RESOURCES_GLOB
    CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/resources/*"
)

foreach(D_FILE_REL ${DIVISION_RESOURCES_GLOB})
    file(RELATIVE_PATH D_FILE_REL ${CMAKE_SOURCE_DIR} ${D_FILE_REL})
    get_filename_component(D_DIR ${D_FILE_REL} DIRECTORY)
    get_filename_component(D_FILE ${D_FILE_REL} NAME)
    configure_file(
        ${CMAKE_SOURCE_DIR}/${D_FILE_REL}
        ${CMAKE_CURRENT_BINARY_DIR}/${D_DIR}/${D_FILE}
    )
    message(${D_FILE})
endforeach()