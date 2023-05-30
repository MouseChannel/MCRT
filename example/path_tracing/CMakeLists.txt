# add_executable(path_tracing pathtracing.cpp)

# target_link_libraries(path_tracing PRIVATE examplelib)

file(GLOB_RECURSE PATH_TRACING ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(path_tracing ${PATH_TRACING})
target_link_libraries(path_tracing examplelib)
add_executable(pathtracing pathtracing.cpp)

target_link_libraries(pathtracing PRIVATE path_tracing)
