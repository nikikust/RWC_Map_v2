cmake_minimum_required(VERSION 3.5)
project(RWC_Map_v2 LANGUAGES CXX)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_STANDARD 20)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})

file(COPY Data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY imgui.ini DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(Threads REQUIRED)

include_directories(
	Include/
    ${CMAKE_BINARY_DIR}/_deps/imgui-src/
    ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/
    ${CMAKE_BINARY_DIR}/_deps/imgui-src/misc/cpp/
    ${CMAKE_BINARY_DIR}/_deps/imgui-sfml-src/
    ${CMAKE_BINARY_DIR}/_deps/sfml-src/include/
    ${CMAKE_BINARY_DIR}/_deps/nlohmann-json-src/include/
)

add_subdirectory(third_party)


add_library(functions STATIC
    Sources/Utils/Functions.cpp
)
target_link_libraries(functions
    ImGui-SFML::ImGui-SFML
)

add_executable(${PROJECT_NAME}
    Main.cpp

    Sources/RWCMap.cpp

    Sources/DataOperations/DataEditor.cpp
    Sources/DataOperations/DataLoader.cpp
    Sources/DataOperations/DataStorage.cpp

    Sources/Modules/Interface.cpp
    Sources/Modules/Painter.cpp
    Sources/Modules/RailroadBricks.cpp
    Sources/Modules/Renderer.cpp
    Sources/Modules/Window.cpp
)
target_link_libraries(${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
    functions
)
