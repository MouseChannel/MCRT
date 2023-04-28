file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src *.cpp)

# set(IMGUIDIR ${PROJECT_SOURCE_DIR}/lib/imgui)
add_library(src ${SRC})
