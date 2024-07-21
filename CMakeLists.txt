cmake_minimum_required(VERSION 3.22.0)
 
project(MCRT)
set(CMAKE_CXX_STANDARD 20)

 
include(cmake/FindVulkan.cmake)
include(cmake/FindGLFW.cmake)
include(cmake/FindIMGUI.cmake)
IF (WIN32)
    target_link_libraries(imgui PUBLIC glfw3 Vulkan::Vulkan glmf32)
ELSEIF (UNIX)
    # in linux link Vulkan::Vulkan
    target_link_libraries(imgui glfw Vulkan::Vulkan)
ENDIF ()
 

 
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/Tool)

# include(cmake/FindSrc.cmake)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp  )
# message(STATUS ${SRC})

# set(IMGUIDIR ${PROJECT_SOURCE_DIR}/lib/imgui)
add_library(src ${SRC})


# find_package(TBB REQUIRED COMPONENTS tbb)
# target_link_libraries(src imgui TBB::tbb)

target_link_libraries(src imgui assimp)

 
add_subdirectory(example)
