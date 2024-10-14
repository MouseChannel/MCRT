
MESSAGE(STATUS "Find Windows glfw😊")

add_subdirectory(thirdParty/glfw)
include_directories(${CMAKE_SOURCE_DIR}/thirdParty/glm)
# find_package(assimp)
IF (Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -w")
ENDIF ()
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)

add_subdirectory(thirdParty/assimp)
   



