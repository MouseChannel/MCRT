file(GLOB_RECURSE cloud ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(cloud_lib ${cloud})
target_link_libraries(cloud_lib examplelib)
add_executable(cloud cloud.cpp)

target_link_libraries(cloud PRIVATE cloud_lib)
