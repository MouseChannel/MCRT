file(GLOB_RECURSE RT_SKYBOX ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(rt_skybox_lib ${RT_SKYBOX})
target_link_libraries(rt_skybox_lib examplelib)
add_executable(rt_skybox rt_skybox.cpp)

target_link_libraries(rt_skybox PRIVATE rt_skybox_lib)
