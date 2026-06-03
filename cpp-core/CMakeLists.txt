cmake_minimum_required(VERSION 3.20)

project(GraphRushCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(graphrush_core
    src/CsrGraph.cpp
)

target_include_directories(graphrush_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test_csr_graph
    tests/test_csr_graph.cpp
)

target_link_libraries(test_csr_graph
    PRIVATE graphrush_core
)

enable_testing()
add_test(NAME test_csr_graph COMMAND test_csr_graph)
