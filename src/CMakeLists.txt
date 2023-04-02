set(GLAD_SOURCE "${CMAKE_SOURCE_DIR}/extern/glad/src/glad.c")

include_directories(
        SYSTEM # exclude from gcc warnings
        ${CMAKE_SOURCE_DIR}/extern/glad/include/
        ${CMAKE_SOURCE_DIR}/extern/glfw/include/
        ${CMAKE_SOURCE_DIR}/include/
)

# GLFW config
option(GLFW_INSTALL OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/glfw glfw)

add_library(MWL_IMAGE STATIC
    ${GLAD_SOURCE}
    mwl_image.c
    qoi.c
    util.c
    )

target_link_libraries(MWL_IMAGE PUBLIC
    ${GLAD_LIBRARIES}
    glfw
    )

target_include_directories(MWL_IMAGE PUBLIC
    ${CMAKE_SOURCE_DIR}/src/
    )

