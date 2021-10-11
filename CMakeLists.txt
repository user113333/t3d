cmake_minimum_required(VERSION 3.12)
project(t3d)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_SUPPRESS_REGENERATION ON)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(BUILD_SHARED_LIBS ON)
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
ENDIF(CMAKE_BUILD_TYPE MATCHES RELEASE)

set(INCLUDE
    "src"
    "lib/assimp/include"
    "lib/flags/include"
    "lib/glm"
    "lib/json/single_include/nlohmann"
)

set(MAIN_SOURCES
    "src/t3d_export.cpp"
    "src/Util.cpp"
    "src/Main.cpp"
)
set(SOURCES ${MAIN_SOURCES})

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library.")

add_subdirectory(lib/assimp)

include_directories(${INCLUDE})
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} assimp)
