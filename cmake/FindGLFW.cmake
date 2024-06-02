
MESSAGE(STATUS "Find Windows glfw😊")

IF(WIN32)
    include(FetchContent)
    Set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Declare(
        glfw3
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(glfw3)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git

        # GIT_TAG "master"
        GIT_TAG 1.0.1
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(glm)
    OPTION(ASSIMP_BUILD_TESTS
        "If the test suite for Assimp is built in addition to the library."
        OFF
    )
#     FetchContent_Declare(
#         assimp
#         GIT_REPOSITORY https://github.com/assimp/assimp
#         GIT_PROGRESS TRUE
#     )
#     FetchContent_MakeAvailable(assimp)

# MESSAGE(STATUS "Find Windows glfw😊")
# set(GLFW_DIR ${PROJECT_SOURCE_DIR}/glfw)
# set(GLWF_INCLUDE_DIR "${GLFW_DIR}/include")
# set(GLFW_LIB_LOCATION "${GLFW_DIR}/include/libglfw3.a")
# add_library(glfw STATIC IMPORTED GLOBAL)
# set_target_properties(
# glfw
# PROPERTIES
# IMPORTED_LOCATION ${GLFW_LIB_LOCATION}
# INTERFACE_INCLUDE_DIRECTORIES ${GLWF_INCLUDE_DIR}
# )

# MESSAGE(STATUS "link Windows glfw down😊")
ELSEIF(UNIX)
    MESSAGE(STATUS "Find linux glfw........")
    find_package(assimp REQUIRED)
    find_package(glfw3 REQUIRED)
ENDIF()