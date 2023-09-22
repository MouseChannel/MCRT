cmake_minimum_required(VERSION 3.0.0)
project(mocheng_vulkan)
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

find_library(log-lib
        log)




set(PROJECT_ROOT_PATH  D:/MoCheng/MoChengRT)

file(GLOB_RECURSE IMGUILIB ${PROJECT_ROOT_PATH}/lib/imgui/imgui.cpp
        ${PROJECT_ROOT_PATH}/lib/imgui/imgui_draw.cpp
        ${PROJECT_ROOT_PATH}/lib/imgui/imgui_demo.cpp
        ${PROJECT_ROOT_PATH}/lib/imgui/imgui_impl_vulkan.cpp

        ${PROJECT_ROOT_PATH}/lib/imgui/imgui_impl_android.cpp
        ${PROJECT_ROOT_PATH}/lib/imgui/imgui_tables.cpp
        ${PROJECT_ROOT_PATH}/lib/imgui/imgui_widgets.cpp

        )

set(IMGUIDIR ${PROJECT_SOURCE_DIR}/lib/imgui)
add_library(imgui ${IMGUILIB})

##copy vulkan header
#file(COPY ${PROJECT_ROOT_PATH}/lib/vulkan/Include  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/ )
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
#
##copy source code
#file(COPY ${PROJECT_ROOT_PATH}/include DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp )
#file(COPY ${PROJECT_ROOT_PATH}/src DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp )
#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/include
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/include/Tool)
#
## copy asset and shader
#file(COPY ${PROJECT_ROOT_PATH}/shaders DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/shaders/lib )
#file(COPY ${PROJECT_ROOT_PATH}/assets DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets )
#
#
#
#
#file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/src *.cpp)
#add_library(src ${SRC})
#
#target_link_libraries(src Vulkan::Vulkan)

find_package(Vulkan REQUIRED)
file(GLOB_RECURSE SRC ${PROJECT_ROOT_PATH}/src/*.cpp ${PROJECT_ROOT_PATH}/example/base/raster_context.cpp)
add_library(src ${SRC})
message(AUTHOR_WARNING ${SRC})
target_link_libraries(imgui Vulkan::Vulkan   android)
target_link_libraries(src imgui )
include_directories(
        ${ANDROID_NDK}/sources/android/native_app_glue
        )


set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}    -u ANativeActivity_onCreate ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -DVK_USE_PLATFORM_ANDROID_KHR")

#add_library(mochengtest SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/moche.cpp)
add_library(mocheng_vulkan SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/start.cpp)
file(GLOB_RECURSE EXAMPLELIB ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/raster_pbr_context.cpp)
add_library(examplelib SHARED ${EXAMPLELIB})
target_link_libraries(examplelib src ${log-lib} )


message(AUTHOR_WARNING ${PROJECT_ROOT_PATH})

include_directories(
        ${PROJECT_ROOT_PATH}
        ${PROJECT_ROOT_PATH}/include
        ${PROJECT_ROOT_PATH}/include/Tool
        ${PROJECT_ROOT_PATH}/lib/vulkan/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
        ${ANDROID_NDK}/sources/android/native_app_glue
        )



target_link_libraries(mocheng_vulkan

        examplelib
        native_app_glue
        android
        )