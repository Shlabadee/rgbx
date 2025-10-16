cmake_minimum_required(VERSION 3.10)
project(RGBX)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS} include external/argo)

set(SOURCES
    src/main.c
    src/bin_handler.c
    src/decoder.c
    src/encoder_helper.c
    src/encoder.c
    src/ezpng.c
    src/legal.c
    src/magic_number.c
    src/rgb_bits.c
    src/rgbaf.c
    src/rgbx.c
    external/argo/argo.c
)

add_executable(rgbx ${SOURCES})

# Don't rely on 100% Windows support yet. I need to get a Windows laptop
# first to see if this actually works.
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    include(${PROJECT_SOURCE_DIR}/cmake/windows.cmake)
else()
    target_compile_options(rgbx PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined -g3>
        $<$<CONFIG:Release>:-O2>
    )

    target_link_options(rgbx PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
        $<$<CONFIG:Release>:-s>
    )
endif()

target_link_libraries(rgbx ${PNG_LIBRARIES})

# Installation
install(TARGETS rgbx
    RUNTIME DESTINATION bin
)
