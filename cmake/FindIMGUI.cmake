file(GLOB_RECURSE IMGUILIB ${PROJECT_SOURCE_DIR}/lib/imgui/*.cpp)
list(REMOVE_ITEM IMGUILIB ${PROJECT_SOURCE_DIR}/lib/imgui/imgui_impl_android.cpp)
set(IMGUIDIR ${PROJECT_SOURCE_DIR}/lib/imgui)
add_library(imgui ${IMGUILIB})
