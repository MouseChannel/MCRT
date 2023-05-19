# file(GLOB_RECURSE example ${CMAKE_CURRENT_SOURCE_DIR} App.cpp Path_tracing_Context.cpp)
# add_library(example1 ${example})

add_executable(blinn_phone blinn_phone.cpp)

target_link_libraries(blinn_phone PRIVATE src)
