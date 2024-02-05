project(division_engine_cpp_example VERSION 0.1.0 LANGUAGES C CXX)

add_executable(division_engine_cpp_example ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(division_engine_cpp_example division_engine_cpp)