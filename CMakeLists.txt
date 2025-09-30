cmake_minimum_required(VERSION 3.10)
project(RGBX)

set(CMAKE_C_FLAGS_DEBUG "-fsanitize=address,undefined -g3")
set(CMAKE_C_FLAGS_RELEASE "-O2 -s")

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS} include)

set(SOURCES
    src/main.c
    src/argparse.c
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
)

add_executable(rgbx ${SOURCES})

target_link_libraries(rgbx ${PNG_LIBRARIES})
