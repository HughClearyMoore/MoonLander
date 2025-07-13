file(GLOB SCENE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

target_sources(MoonLander PRIVATE ${SCENE_HEADERS})