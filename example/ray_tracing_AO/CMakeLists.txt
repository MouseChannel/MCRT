# add_executable(path_tracing pathtracing.cpp)

# target_link_libraries(path_tracing PRIVATE examplelib)

file(GLOB_RECURSE RAY_TRACING_AO ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(ray_tracing_ao_lib ${RAY_TRACING_AO})
target_link_libraries(ray_tracing_ao_lib examplelib)
add_executable(ray_tracing_ao raytracing_AO)

target_link_libraries(ray_tracing_ao PRIVATE ray_tracing_ao_lib)
