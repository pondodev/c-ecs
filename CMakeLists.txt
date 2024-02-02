cmake_minimum_required(VERSION 3.11 FATAL_ERROR) # 3.11+ for FetchContent

#### compile_commands.json generation ####

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#### project settings ####

project (c-ecs VERSION 0.1 LANGUAGES C)
set     (CMAKE_BUILD_TYPE Release)
set     (CMAKE_C_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic)

#### project libraries ####

set(RAYLIB_VERSION 4.2.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND) # download raylib if not present
  include(FetchContent)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # is raylib downloaded yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} raylib)
if (APPLE)
    set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES}
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL")
endif()

#### project files ####

file(GLOB_RECURSE PROJECT_SOURCES src/*.c)

set(PROJECT_INCLUDE_DIRS src/)
set(PLATFORM_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} src/platform)
set(GAME_API_HEADER src/game/api/game_interface.h)
set(GAME_PRIVATE_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} src/game)

#### targets ####
add_executable              (${PROJECT_NAME})

target_include_directories  (${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE_DIRS})
target_sources              (${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_link_libraries       (${PROJECT_NAME} PRIVATE ${PROJECT_LIBRARIES})
target_compile_definitions  (${PROJECT_NAME} PRIVATE ${PROJECT_COMPILE_DEFINITIONS})
target_compile_options      (${PROJECT_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})
set_target_properties       (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_GENERATE_SCHEME TRUE
        XCODE_SCHEME_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

add_custom_target(run COMMAND ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

