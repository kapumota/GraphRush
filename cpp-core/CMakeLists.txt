cmake_minimum_required(VERSION 3.20)

project(GraphRushCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(graphrush_core
    src/CsrGraph.cpp
    src/GraphLoader.cpp
    src/EdgeListParser.cpp
    src/BinaryGraphIO.cpp
)

target_include_directories(graphrush_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test_graph_io
    tests/test_graph_io.cpp
)

target_link_libraries(test_graph_io
    PRIVATE graphrush_core
)

enable_testing()
add_test(NAME test_graph_io COMMAND test_graph_io)
