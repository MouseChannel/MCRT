

file(GLOB_RECURSE RASTERLIB ${PROJECT_SOURCE_DIR}/example/base/raster_context.cpp)
add_library(rasterlib ${RASTERLIB})

file(GLOB_RECURSE RAYTRACINGLIB ${PROJECT_SOURCE_DIR}/example/base/raytracing_context.cpp)
add_library(raytracinglib ${RAYTRACINGLIB})

target_link_libraries(rasterlib src)
target_link_libraries(raytracinglib src)

# add_subdirectory(ray_tracing_blinn_phong)
# add_subdirectory(path_tracing)

# add_subdirectory(ray_tracing_mirror)
# add_subdirectory(cook_torrance)
add_subdirectory(raster)
add_subdirectory(rt_pbr)
# add_subdirectory(gbuffer)

# add_subdirectory(ray_tracing_AO)
# add_subdirectory(path_tracing_light)
# add_subdirectory(rt_skybox)
# add_subdirectory(rt_pbr)
# add_subdirectory(cloud)

# add_subdirectory(tiled_rendering)

# add_subdirectory(rt_pbr_test)
