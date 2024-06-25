file(GLOB_RECURSE RASTER_PBR ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(raster_pbr_lib ${RASTER_PBR})

target_link_libraries(raster_pbr_lib rasterlib)

add_executable(raster_pbr raster_pbr.cpp)

target_link_libraries(raster_pbr PRIVATE raster_pbr_lib)
