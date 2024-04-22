# set(glfw3_DIR "D:\\a\\MCRT\\MCRT\\glfw\\CMake")
# find_package(glfw3 REQUIRED)
MESSAGE(STATUS "Find Windows glfw😊")

# find_package(glfw3 REQUIRED)
# FetchContent_Declare(
#     TBB
#     GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
#     GIT_TAG "master"
# 	GIT_SHALLOW ON
# 	CMAKE_ARGS "-DTBB_TEST:BOOL=OFF -DTBB_EXAMPLES:BOOL=OFF -DTBB_BENCH:BOOL=OFF -DTBB_BUILD:BOOL=ON -DTBB_FIND_PACKAGE:BOOL=OFF -DTBB_FUZZ_TESTING:BOOL=OFF -DTBB_INSTALL:BOOL=ON"
# 	OVERRIDE_FIND_PACKAGE
#     )
# set(TBB_TEST OFF CACHE INTERNAL "" FORCE)
# set(TBB_EXAMPLES OFF CACHE INTERNAL "" FORCE)
# set(TBB_BENCH OFF CACHE INTERNAL "" FORCE)
# set(TBB_BUILD ON CACHE INTERNAL "" FORCE)
# set(TBB_FIND_PACKAGE OFF CACHE INTERNAL "" FORCE)
# set(TBB_FUZZ_TESTING OFF CACHE INTERNAL "" FORCE)
# set(TBB_INSTALL ON CACHE INTERNAL "" FORCE)
# FetchContent_MakeAvailable(TBB)





IF(WIN32)
include(FetchContent)

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4  # 或指定特定的分支或标签
)
FetchContent_MakeAvailable(glfw3)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG "master"   
)
FetchContent_MakeAvailable(glm)

    # MESSAGE(STATUS "Find Windows glfw😊")
    # set(GLFW_DIR ${PROJECT_SOURCE_DIR}/glfw)
    # set(GLWF_INCLUDE_DIR "${GLFW_DIR}/include")
    # set(GLFW_LIB_LOCATION "${GLFW_DIR}/include/libglfw3.a")
    # add_library(glfw STATIC IMPORTED GLOBAL)
    # set_target_properties(
    #     glfw
    #     PROPERTIES
    #     IMPORTED_LOCATION ${GLFW_LIB_LOCATION}
    #     INTERFACE_INCLUDE_DIRECTORIES ${GLWF_INCLUDE_DIR}
    # )
    
    # MESSAGE(STATUS "link Windows glfw down😊")
ELSEIF(UNIX)
    MESSAGE(STATUS "Find linux glfw........")
    find_package(glfw3 REQUIRED)
ENDIF()