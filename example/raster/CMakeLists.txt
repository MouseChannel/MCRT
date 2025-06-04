
add_executable(raster_pbr raster_pbr.cpp App.cpp raster_pbr_context.cpp)
set_target_properties(raster_pbr PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

target_link_libraries(raster_pbr MCRT)

