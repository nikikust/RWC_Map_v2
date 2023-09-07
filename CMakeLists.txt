cmake_minimum_required(VERSION 3.5)
project(RWC_Map_v2 LANGUAGES CXX)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

file(COPY Data DESTINATION ${CMAKE_BINARY_DIR})
file(COPY imgui.ini DESTINATION ${CMAKE_BINARY_DIR})


include_directories(Include
    ${CMAKE_BINARY_DIR}/_deps/imgui-src
    ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends
    ${CMAKE_BINARY_DIR}/_deps/imgui-src/misc/cpp/
    ${CMAKE_BINARY_DIR}/_deps/imgui-sfml-src/
    ${CMAKE_BINARY_DIR}/_deps/sfml-src/include/
    ${CMAKE_BINARY_DIR}/_deps/nlohmann-json-src/include/
)

add_subdirectory(third_party)

find_package(Threads REQUIRED)


add_library(functions STATIC
    Sources/Utils/Functions.cpp
)
target_link_libraries(functions
    imgui
    SFML
    ImGui-SFML::ImGui-SFML
    nlohmann-json
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

string(REGEX REPLACE "/W4" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
    functions
)
# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/PROFILE")
