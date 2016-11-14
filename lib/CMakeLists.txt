project(CocosGif)
cmake_minimum_required(VERSION 3.1)

get_filename_component(CCGIF_ROOT "./" ABSOLUTE)

set(CCGIF_INCLUDE_LIST
    ${CCGIF_ROOT}/include
    CACHE LIST "Set the include list for ${PROJECT_NAME}"
)

include_directories(
    ${CCGIF_ROOT}/include
    ${CCGIF_ROOT}/include/CocosGif
    ${CCGIF_ROOT}/include/gif_lib
)

set(CCGIF_SRC
    src/Bitmap.cpp
    src/CacheGif.cpp
    src/GifBase.cpp
    src/GifMovie.cpp
    src/InstantGif.cpp
    src/Movie.cpp
    # Insert gif_lib
    src/gif_lib/dgif_lib.c
    src/gif_lib/gif_err.c
    src/gif_lib/gif_font.c
    src/gif_lib/gif_hash.c
    src/gif_lib/gifalloc.c
    src/gif_lib/quantize.c
)

set(CCGIF_HEADERS
    include/CocosGif/Bitmap.h
    include/CocosGif/CacheGif.h
    include/CocosGif/GifBase.h
    include/CocosGif/GifMacros.h
    include/CocosGif/GifUtils.h
    include/CocosGif/InstantGif.h
    include/CocosGif/Movie.h
    # Insert gif_lib
    include/gif_lib/gif_hash.h
    include/gif_lib/gif_lib.h
    include/gif_lib/gif_lib_private.h
)

add_library(${PROJECT_NAME} STATIC
    ${CCGIF_SRC}
    ${CCGIF_HEADERS}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
