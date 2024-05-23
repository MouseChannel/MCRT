 
MESSAGE(STATUS "Find Windows glfw😊")

 

IF(WIN32)
include(FetchContent)

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4  
)
FetchContent_MakeAvailable(glfw3)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG "master"   
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY     https://github.com/assimp/assimp
  
)
FetchContent_MakeAvailable(assimp)
 

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