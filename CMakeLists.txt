cmake_minimum_required(VERSION 3.0.0)
project(MCRT VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

# add_compile_options("< <<<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
include(cmake/FindVulkan.cmake)
include(cmake/FindGLFW.cmake)
include(cmake/FindIMGUI.cmake)
target_link_libraries(imgui glfw Vulkan)

find_program(GLSLANGV ${VULKAN_DIR}/Bin/glslangValidator.exe REQUIRED)
file(GLOB_RECURSE RGEN ${CMAKE_SOURCE_DIR}/shader/*.rgen)
file(GLOB_RECURSE RCHIT ${CMAKE_SOURCE_DIR}/shader/*.rchit)
file(GLOB_RECURSE RMISS ${CMAKE_SOURCE_DIR}/shader/*.rmiss)
file(GLOB_RECURSE VERT ${CMAKE_SOURCE_DIR}/shader/*.vert)
file(GLOB_RECURSE FRAG ${CMAKE_SOURCE_DIR}/shader/*.frag)
list(APPEND ALL_RT_SHADER ${RGEN} ${RCHIT} ${RMISS} ${VERT} ${FRAG})
message(STATUS "run glslangValidator to compile shaders ...")
message("-----all ray tracing shader------ ")

foreach(var ${ALL_RT_SHADER})
    message(${var})
    get_filename_component(file_name ${var} NAME)

    string(CONCAT name ${file_name} .spv)

    # message(${name})
    execute_process(COMMAND ${GLSLANGV} -V --target-env vulkan1.3 ${var} -o ${CMAKE_SOURCE_DIR}/shader/${name})

    # execute_process(COMMAND ${GLSLANGV} -V --target-env vulkan1.3 ${var} -o ${CMAKE_SOURCE_DIR}/shader/${name})
endforeach()

message("---------------------------------")

# message(${USER_LIBS_PATH})

# execute_process(COMMAND ${GLSLANGV} -V ${CMAKE_SOURCE_DIR}/Shader/frag.frag -o ${CMAKE_SOURCE_DIR}/Shader/frag.spv)
message(STATUS "compile shader OK")
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)

# include(cmake/FindSrc.cmake)
file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src *.cpp)

# set(IMGUIDIR ${PROJECT_SOURCE_DIR}/lib/imgui)
add_library(src ${SRC})

# add_executable(MCRT main.cpp)
target_link_libraries(src imgui)
add_subdirectory(example)
