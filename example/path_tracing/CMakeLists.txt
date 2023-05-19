# file(GLOB_RECURSE example ${CMAKE_CURRENT_SOURCE_DIR} App.cpp Path_tracing_Context.cpp)
# add_library(example1 ${example})

add_executable(path_tracing pathtracing.cpp)

target_link_libraries(path_tracing PRIVATE src)
