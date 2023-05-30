file(GLOB_RECURSE BLINN_PHONG ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(blinn_phong_lib ${BLINN_PHONG})
target_link_libraries(blinn_phong_lib examplelib)
add_executable(blinn_phong blinn_phong.cpp)

target_link_libraries(blinn_phong PRIVATE blinn_phong_lib)
