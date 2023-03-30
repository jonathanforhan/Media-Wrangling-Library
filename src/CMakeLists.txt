add_executable(${CMAKE_PROJECT_NAME} main.c ${GLAD_SOURCE})

add_library(IMAGE_LIB STATIC
        image_handler.c
        ppm.c
        qoi.c
        )
target_include_directories(IMAGE_LIB PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC IMAGE_LIB)
