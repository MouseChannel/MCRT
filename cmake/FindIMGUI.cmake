file(GLOB_RECURSE IMGUILIB ${CMAKE_CURRENT_LIST_DIR}/../lib/imgui/*.cpp)
# message(STATUS mom${CMAKE_CURRENT_LIST_DIR})
list(REMOVE_ITEM IMGUILIB ${CMAKE_CURRENT_LIST_DIR}/../lib/imgui/imgui_impl_android.cpp)
set(IMGUIDIR ${CMAKE_CURRENT_LIST_DIR}/../lib/imgui)
add_library(imgui ${IMGUILIB})
set_target_properties(
        imgui
        PROPERTIES
        INCLUDE_DIRECTORIES ${MCRT_INCLUDE_DIR}
)
target_include_directories(imgui PUBLIC ${MCRT_INCLUDE_DIR} ${IMGUIDIR})
# set_target_properties(
#      imgui
#     PROPERTIES
#     INCLUDE_DIRECTORIES 
# )
