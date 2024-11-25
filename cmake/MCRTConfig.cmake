set(MCRT_FOUND FALSE)
set(MCRT_LIBRARIES)
set(MCRT_INCLUDE_DIRS)


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options("-fstandalone-debug")
    endif ()
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -w")

    set(Clang ON)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(GNU ON)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(MSVC ON)

    # add_compile_options(/GS-)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GS-")


endif ()

include(${CMAKE_CURRENT_LIST_DIR}/FindThirdParty.cmake)


set (MCRT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

set(MCRT_INCLUDE_DIR ${MCRT_SOURCE_DIR}/include)
file(GLOB_RECURSE MCRT_LIBRARY ${MCRT_SOURCE_DIR}/src/*.cpp)


list(REMOVE_ITEM MCRT_LIBRARY ${MCRT_SOURCE_DIR}/src/Imgui/imgui_impl_android.cpp)

add_library(MCRT  ${MCRT_LIBRARY})
target_include_directories(MCRT PUBLIC ${MCRT_INCLUDE_DIR})




target_link_libraries(MCRT PUBLIC glfw  Vulkan::Vulkan assimp::assimp glm::glm VulkanMemoryAllocator)




if (MCRT_LIBRARY AND MCRT_INCLUDE_DIR)
    set(MCRT_FOUND TRUE)
    set(MCRT_LIBRARIES ${MCRT_LIBRARY})
    set(MCRT_INCLUDE_DIRS ${MCRT_INCLUDE_DIR})
endif ()

if (MCRT_FOUND)
    message(STATUS "Found PackageName: ${MCRT_LIBRARIES}")
    message(STATUS "${MCRT_INCLUDE_DIRS}")
else ()
    message(STATUS "PackageName not found.")
endif ()

# mark_as_advanced(MCRT_FOUND MCRT_LIBRARIES MCRT_INCLUDE_DIRS)