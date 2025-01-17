cmake_minimum_required(VERSION 3.0)
project(Navaduel C)
set(CMAKE_C_STANDARD 99)

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE         # Show progress during the download
    GIT_SHALLOW TRUE          # Only fetch the latest commit for faster cloning
)
FetchContent_MakeAvailable(raylib)

# Add inih
FetchContent_Declare(
    inih
    GIT_REPOSITORY "https://github.com/benhoyt/inih"
    GIT_TAG "master"
)
FetchContent_MakeAvailable(inih)

# Disable uninstall target for cJSON to avoid conflicts
set(ENABLE_CJSON_UNINSTALL OFF CACHE BOOL "" FORCE)
# Disable warnings as errors
set(ENABLE_CJSON_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
# Disable cJSON tests
set(ENABLE_CJSON_TEST OFF CACHE BOOL "" FORCE)
# Add cJSON
FetchContent_Declare(
    cJSON
    GIT_REPOSITORY "https://github.com/DaveGamble/cJSON"
    GIT_TAG "master"
)
FetchContent_MakeAvailable(cJSON)

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.c") # Define PROJECT_SOURCES as a list of all source files
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/") # Define PROJECT_INCLUDE to be the path to the include directory of the project

# Declaring our executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib cjson)

# Add inih headers and source file directly to the executable, since inih is a smaill single file lib
target_include_directories(${PROJECT_NAME} PRIVATE ${inih_SOURCE_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${inih_SOURCE_DIR}/ini.c)

# Suppress warnings for cJSON 
if(TARGET cjson)
    target_compile_options(cjson PRIVATE
        $<$<C_COMPILER_ID:GNU>:-w>               # Disable all warnings for GCC/MinGW
        $<$<C_COMPILER_ID:MSVC>:/w>              # Disable all warnings for MSVC
        $<$<C_COMPILER_ID:GNU>:-Wno-float-conversion>  # Specifically disable float-conversion warning
    )
endif()