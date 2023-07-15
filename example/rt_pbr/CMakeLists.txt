file(GLOB_RECURSE rt_pbr ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(rt_pbr_lib ${rt_pbr})
target_link_libraries(rt_pbr_lib examplelib)
add_executable(rt_pbr rt_pbr.cpp)

target_link_libraries(rt_pbr PRIVATE rt_pbr_lib)
