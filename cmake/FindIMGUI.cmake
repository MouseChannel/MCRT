file(GLOB_RECURSE IMGUILIB ${PROJECT_SOURCE_DIR}/lib/imgui *.cpp)
set(IMGUIDIR ${PROJECT_SOURCE_DIR}/lib/imgui)
add_library(imgui ${IMGUILIB})
