cmake_minimum_required(VERSION 3.16)
project(Vector_Lan VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for language servers (clangd, cquery, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Copy `compile_commands.json` to project root for tools that expect it there (clangd, etc.)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_SOURCE_DIR}/compile_commands.json)
  endif()
endif()

# Create a CMake File API query so tools like clangd or IDE extensions can read the CMake codemodel
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/.cmake/api/v1/query")
file(WRITE "${CMAKE_BINARY_DIR}/.cmake/api/v1/query/codemodel-v2" "")

option(USE_SYSTEM_RAYLIB "Use system-installed raylib" ON)

if(USE_SYSTEM_RAYLIB)
  find_path(RAYLIB_INCLUDE_DIR raylib.h)
  find_library(RAYLIB_LIBRARY NAMES raylib)
  if(RAYLIB_INCLUDE_DIR AND RAYLIB_LIBRARY)
    message(STATUS "Found system raylib")
    add_library(raylib::raylib UNKNOWN IMPORTED)
    set_target_properties(raylib::raylib PROPERTIES IMPORTED_LOCATION ${RAYLIB_LIBRARY} INTERFACE_INCLUDE_DIRECTORIES ${RAYLIB_INCLUDE_DIR})
  else()
    message(STATUS "System raylib not found; disabling USE_SYSTEM_RAYLIB")
    set(USE_SYSTEM_RAYLIB OFF CACHE BOOL "Use system-installed raylib" FORCE)
  endif()
endif()

if(NOT USE_SYSTEM_RAYLIB)
  include(FetchContent)
  FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
  )
  FetchContent_MakeAvailable(raylib)
endif()

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking # Recommended branch for flexible UI layouts
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    
    # CRUCIAL: Manually define the target and ONLY include the core source files.
    # This prevents CMake from scanning the 'backends' folder and throwing the API error.
    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    )
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
endif()

FetchContent_Declare(rlimgui GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git GIT_TAG main)
FetchContent_MakeAvailable(rlimgui)
add_library(rlimgui STATIC ${rlimgui_SOURCE_DIR}/rlImGui.cpp)
target_include_directories(rlimgui PUBLIC ${rlimgui_SOURCE_DIR})
target_link_libraries(rlimgui PUBLIC raylib imgui)

find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1
  )
  FetchContent_MakeAvailable(spdlog)
endif()

add_executable(vector
src/main.cpp
src/app.h
src/app.cpp
src/ui/screens.h
src/ui/screens.cpp
src/Networking/enet_implementation.cpp
src/context.h
src/Networking/Packets.h
src/client.cpp
src/client.h
src/server.h
src/player.h
src/server.cpp
src/player.cpp
)

target_include_directories(vector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
if(TARGET raylib)
  target_link_libraries(vector PRIVATE raylib)
else()
  target_link_libraries(vector PRIVATE raylib::raylib)
endif()

target_link_libraries(vector PRIVATE m pthread dl)

target_link_libraries(vector PRIVATE imgui)

target_link_libraries(vector PRIVATE rlimgui)
target_link_libraries(vector PRIVATE spdlog::spdlog fmt)