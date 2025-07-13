file(GLOB SCENE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

target_sources(MoonLander PRIVATE ${SCENE_SOURCES})