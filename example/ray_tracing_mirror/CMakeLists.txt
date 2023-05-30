file(GLOB_RECURSE MIRROR ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(mirror_lib ${MIRROR})
target_link_libraries(mirror_lib examplelib)
add_executable(mirror mirror.cpp)

target_link_libraries(mirror PRIVATE mirror_lib)
