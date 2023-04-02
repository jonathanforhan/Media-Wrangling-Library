set(MWL_IMAGE_SOURCES
    mwl_image.c
    qoi.c
    util.c
    )

set(MWL_IMAGE_INCLUDE_DIRS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/"
    )

add_library(MWL_IMAGE STATIC ${MWL_IMAGE_SOURCES})
target_include_directories(MWL_IMAGE PUBLIC ${MWL_IMAGE_INCLUDE_DIRS})

