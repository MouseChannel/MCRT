# add_executable(cook_torrance cook_torrance.cpp)

# target_link_libraries(cook_torrance PRIVATE examplelib)

file(GLOB_RECURSE COOK_TORRANCE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(cook_torrance_lib ${COOK_TORRANCE})
target_link_libraries(cook_torrance_lib examplelib)
add_executable(cook_torrance cook_torrance)

target_link_libraries(cook_torrance PRIVATE cook_torrance_lib)
