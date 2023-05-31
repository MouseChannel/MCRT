

file(GLOB_RECURSE EXAMPLELIB ${PROJECT_SOURCE_DIR}/example/base *.cpp)
add_library(examplelib ${EXAMPLELIB})
target_link_libraries(examplelib src)
add_subdirectory(ray_tracing_blinn_phong)
add_subdirectory(path_tracing)

add_subdirectory(ray_tracing_mirror)

# add_subdirectory(base)
add_subdirectory(cook_torrance)

add_subdirectory(raster)

# add_subdirectory(test)
