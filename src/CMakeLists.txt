target_include_directories(MoonLander PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(HEADERS
    "include/MLCore.h"
    "include/MLTypes.h"
    "include/MLShader.h"
    "include/MLColours.h"
    "include/MLMesh.h"
    "include/MLModel.h"
)

set(SOURCES
    "MLCore.c"
    "MLShader.c"
    "MLMesh.c"
    "MLModel.c"
)

set(DEFS
    "defs/MLColours.defs"
)


target_sources(MoonLander PRIVATE ${HEADERS} ${SOURCES} ${DEFS})