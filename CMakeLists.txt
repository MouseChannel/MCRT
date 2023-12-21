cmake_minimum_required(VERSION 3.27.0)
#if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
 

set(CMAKE_CUDA_ARCHITECTURES 75)
set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
#set(CMAKE_CUDA_COMPILER nvcc)

#endif()
#include(CheckLanguage)
#check_language(CUDA)
project(MCRT LANGUAGES CXX CUDA)
set(CMAKE_CXX_STANDARD 20)

# add_compile_options("< <<<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
include(cmake/FindVulkan.cmake)
include(cmake/FindGLFW.cmake)
include(cmake/FindIMGUI.cmake)
IF (WIN32)
    target_link_libraries(imgui glfw Vulkan)
ELSEIF (UNIX)
    # in linux link Vulkan::Vulkan
    target_link_libraries(imgui glfw Vulkan::Vulkan)
ENDIF ()
#find_program(GLSLANGV ${VULKAN_DIR}/Bin/glslangValidator.exe REQUIRED)
#file(GLOB_RECURSE RGEN ${CMAKE_SOURCE_DIR}/shader/*.rgen)
#file(GLOB_RECURSE RCHIT ${CMAKE_SOURCE_DIR}/shader/*.rchit)
#file(GLOB_RECURSE RMISS ${CMAKE_SOURCE_DIR}/shader/*.rmiss)
#file(GLOB_RECURSE VERT ${CMAKE_SOURCE_DIR}/shader/*.vert)
#file(GLOB_RECURSE FRAG
# message(${name})
# execute_process(COMMAND ${GL${CMAKE_SOURCE_DIR}/shader/*.frag)
#list(APPEND ALL_RT_SHADER ${RGEN} ${RCHIT} ${RMISS} ${VERT} ${FRAG})
#message(STATUS "run glslangValidator to compile shaders ...")
#message("-----all ray tracing shader------ ")
#
#foreach(var ${ALL_RT_SHADER})
#    # message(${var})
#    get_filename_component(file_name ${var} NAME)
#
#    string(CONCAT name ${file_name} .spv)SLANGV} -V --target-env vulkan1.3 ${var} -o ${CMAKE_SOURCE_DIR}/shader/${name})

# execute_process(COMMAND ${GLSLANGV} -V --target-env vulkan1.3 ${var} -o ${CMAKE_SOURCE_DIR}/shader/${name})
#endforeach()

message("---------------------------------")

# message(${USER_LIBS_PATH})

# execute_process(COMMAND ${GLSLANGV} -V ${CMAKE_SOURCE_DIR}/Shader/frag.frag -o ${CMAKE_SOURCE_DIR}/Shader/frag.spv)
message(STATUS "compile shader OK")
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/Tool)

# include(cmake/FindSrc.cmake)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.cu)
message(STATUS ${SRC})

# set(IMGUIDIR ${PROJECT_SOURCE_DIR}/lib/imgui)
add_library(src ${SRC})


target_link_libraries(src imgui)
find_package(CUDAToolkit REQUIRED)
if(CUDAToolkit_FOUND)
    include_directories(${CUDAToolkit_INCLUDE_DIRS})
    link_directories(${CUDAToolkit_LIBRARY_DIR})
#    message("CUDA 库文件目录: ${CUDAToolkit_INCLUDE_DIRS} ${CUDAToolkit_LIBRARY_DIR}")
    target_link_libraries(src cuda cudart)

endif()
#include_directories(/usr/local/cuda/include)
#target_link_directories(src PUBLIC /usr/local/cuda/lib64/)
add_subdirectory(example)
