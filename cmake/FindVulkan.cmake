find_package(Vulkan QUIET)
IF (NOT Vulkan_FOUND)

    set(Vulkan_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdParty/vulkan)
    set(Vulkan_LIB_LOCATION ${CMAKE_SOURCE_DIR}/thirdParty/vulkan-1.lib)
    #     # message(${glfw3_LIB_LOCATION})
    add_library(Vulkan::Vulkan STATIC IMPORTED GLOBAL)
    set_target_properties(
            Vulkan::Vulkan
            PROPERTIES
            IMPORTED_LOCATION ${Vulkan_LIB_LOCATION}
            INTERFACE_INCLUDE_DIRECTORIES ${Vulkan_INCLUDE_DIR}

            #    COMPILE_FLAGS "-Wno-unknown-pragmas -w"
    )
ENDIF ()

 