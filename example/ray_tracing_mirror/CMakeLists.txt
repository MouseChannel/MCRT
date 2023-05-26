# file(GLOB_RECURSE example ${CMAKE_CURRENT_SOURCE_DIR} App.cpp Path_tracing_Context.cpp)
# add_library(example1 ${example})

add_executable(mirror mirror.cpp)

target_link_libraries(mirror PRIVATE src)
