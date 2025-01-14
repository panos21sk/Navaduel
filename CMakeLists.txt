#Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
#
#This software is provided "as-is", without any express or implied warranty. In no event 
#will the authors be held liable for any damages arising from the use of this software.
#
#Permission is granted to anyone to use this software for any purpose, including commercial 
#applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not claim that you 
#  wrote the original software. If you use this software in a product, an acknowledgment 
#  in the product documentation would be appreciated but is not required.
#
#  2. Altered source versions must be plainly marked as such, and must not be misrepresented
#  as being the original software.
#
#  3. This notice may not be removed or altered from any source distribution.

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
target_link_libraries(${PROJECT_NAME} PRIVATE raylib inih cjson)