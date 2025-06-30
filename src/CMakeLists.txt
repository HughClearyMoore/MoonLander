target_include_directories(MoonLander PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(HEADERS
    "include/MLCore.h"
    "include/MLTypes.h"
    "include/MLShader.h"
    "include/MLColours.h"
    "include/MLMesh.h"
    "include/MLModel.h"
    "include/MLMeshManager.h"    
    "include/MLFileIO.h"
    "include/MLOBJLoader.h"
    "include/MLAssets.h"
    "include/MLShaderProgramManager.h"
    "include/MLScript.h"
    "include/MLScriptManager.h"
)

set(SOURCES
    "MLCore.c"
    "MLShader.c"
    "MLMesh.c"
    "MLModel.c"
    "MLMeshManager.c"    
    "MLFileIO.c"
    "MLOBJLoader.c"
    "MLAssets.c"
    "MLShaderProgramManager.c"
    "MLScript.c"
    "MLScriptManager.c"
)

set(DEFS
    "defs/MLColours.defs"
    "defs/MLAssetTypes.defs"
    "defs/MLScripts.defs"
)

add_subdirectory(include)
add_subdirectory(components)
add_subdirectory(scripts)


target_sources(MoonLander PRIVATE ${HEADERS} ${SOURCES} ${DEFS})