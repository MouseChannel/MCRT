# add_executable(path_tracing pathtracing.cpp)

# target_link_libraries(path_tracing PRIVATE examplelib)

file(GLOB_RECURSE PATH_TRACING_LIGHT ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(path_tracing_light_lib ${PATH_TRACING_LIGHT})
target_link_libraries(path_tracing_light_lib examplelib)
add_executable(pathtracing_light pathtracing_light.cpp)

target_link_libraries(pathtracing_light PRIVATE path_tracing_light_lib)
