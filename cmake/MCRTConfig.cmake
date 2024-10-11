set(MCRT_FOUND FALSE)
set(MCRT_LIBRARIES)
set(MCRT_INCLUDE_DIRS)




if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options("-fstandalone-debug")
    endif()
    set(Clang ON)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(GNU ON)

endif()


set(MCRT_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
message(status ${MCRT_INCLUDE_DIR})
file(GLOB_RECURSE MCRT_LIBRARY ${CMAKE_CURRENT_LIST_DIR}/../src/*.cpp)

add_library(MCRT STATIC ${MCRT_LIBRARY})
set_target_properties(
    MCRT
    PROPERTIES
    INCLUDE_DIRECTORIES ${MCRT_INCLUDE_DIR}
)
include(${CMAKE_CURRENT_LIST_DIR}/FindGLFW.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/FindIMGUI.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/FindVulkan.cmake)

IF (Clang OR GNU)
    target_link_libraries(imgui PUBLIC glfw3 Vulkan::Vulkan glm::glm)
ELSEIF (UNIX)
    target_link_libraries(imgui glfw Vulkan::Vulkan)
ENDIF ()



find_package(assimp REQUIRED)
target_link_libraries(MCRT imgui assimp)


if(MCRT_LIBRARY AND MCRT_INCLUDE_DIR)
    set(MCRT_FOUND TRUE)
    set(MCRT_LIBRARIES ${MCRT_LIBRARY})
    set(MCRT_INCLUDE_DIRS ${MCRT_INCLUDE_DIR})
endif()

if(MCRT_FOUND)
    message(STATUS "Found PackageName: ${MCRT_LIBRARIES}")
else()
    message(STATUS "PackageName not found.")
endif()

mark_as_advanced(MCRT_FOUND MCRT_LIBRARIES MCRT_INCLUDE_DIRS)