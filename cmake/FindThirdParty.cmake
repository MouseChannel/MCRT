# set(old_build_type ${CMAKE_BUILD_TYPE})

# message(${old_build_type})

MESSAGE(STATUS "Find Windows glfw😊")

find_package(glfw3 QUIET)
if (NOT glfw3_FOUND)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../thirdParty/glfw)
ENDIF ()
find_package(glm QUIET)
IF (NOT glm_FOUND)
    include_directories(${CMAKE_CURRENT_LIST_DIR}/../thirdParty/glm)
ENDIF ()

find_package(assimp QUIET)
IF (NOT assimp_FOUND)
    set(ASSIMP_BUILD_TESTS OFF)
    set(ASSIMP_INSTALL OFF)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../thirdParty/assimp)
ENDIF ()


add_subdirectory(thirdParty/VulkanMemoryAllocator EXCLUDE_FROM_ALL)
   



