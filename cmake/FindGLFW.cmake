
MESSAGE(STATUS "Find Windows glfw😊")

IF(Clang OR GNU)

find_package(glfw3 REQUIRED)
# pacman -S mingw-w64-ucrt-x86_64-glfw
find_package(glm REQUIRED)
# pacman -S mingw-w64-ucrt-x86_64-glm
find_package(assimp REQUIRED)
# pacman -S mingw-w64-ucrt-x86_64-assimp

#todo MSVC
ELSEIF(UNIX)
    MESSAGE(STATUS "Find linux glfw........")
    find_package(assimp REQUIRED)
    find_package(glfw3 REQUIRED)
ENDIF()