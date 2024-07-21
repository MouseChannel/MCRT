file(GLOB_RECURSE OFFSCREEN_PBR ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(offscreen_lib ${OFFSCREEN_PBR})

target_link_libraries(offscreen_lib rasterlib)

add_executable(offscreen offscreen.cpp)

target_link_libraries(offscreen PRIVATE offscreen_lib)
