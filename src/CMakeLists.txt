target_include_directories(MoonLander PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(HEADERS
    "include/MLCore.h"
)

set(SOURCES
    "MLCore.c"
)


target_sources(MoonLander PRIVATE "${HEADERS}" "${SOURCES}")