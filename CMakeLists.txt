cmake_minimum_required(VERSION 3.20)

project(media_wrangler_library)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_VERBOSE_MAKEFILE 0) # 1 should be used for debugging
set(CMAKE_SUPPRESS_REGENERATION TRUE) # Suppresses ZERO_CHECK
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
else()
    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
    if(CMAKE_BUILD_TYPE STREQUAL Release)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
    elseif(CMAKE_BUILD_TYPE STREQUAL Debug)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -std=c99")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
    endif()
endif()

add_subdirectory(src)

# Test app
add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/test/test.c)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC IMAGE_LIB)
