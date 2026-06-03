cmake_minimum_required(VERSION 3.20)

project(GraphRushCore LANGUAGES CXX)

find_package(OpenMP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(graphrush_core
    src/CsrGraph.cpp
    src/GraphLoader.cpp
    src/EdgeListParser.cpp
    src/BinaryGraphIO.cpp
    src/Algorithms.cpp
    src/ParallelAlgorithms.cpp
    src/DeltaStepping.cpp
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

add_executable(test_sequential_algorithms
    tests/test_sequential_algorithms.cpp
)

target_link_libraries(test_sequential_algorithms
    PRIVATE graphrush_core
)

add_executable(test_parallel_algorithms
    tests/test_parallel_algorithms.cpp
)

target_link_libraries(test_parallel_algorithms
    PRIVATE graphrush_core
)

add_executable(test_delta_stepping
    tests/test_delta_stepping.cpp
)

target_link_libraries(test_delta_stepping
    PRIVATE graphrush_core
)

add_executable(test_weighted_graph_io
    tests/test_weighted_graph_io.cpp
)

target_link_libraries(test_weighted_graph_io
    PRIVATE graphrush_core
)

enable_testing()
add_test(NAME test_graph_io COMMAND test_graph_io)
add_test(NAME test_sequential_algorithms COMMAND test_sequential_algorithms)
add_test(NAME test_parallel_algorithms COMMAND test_parallel_algorithms)
add_test(NAME test_delta_stepping COMMAND test_delta_stepping)
add_test(NAME test_weighted_graph_io COMMAND test_weighted_graph_io)


if(OpenMP_CXX_FOUND)
    target_link_libraries(graphrush_core PUBLIC OpenMP::OpenMP_CXX)
    target_compile_definitions(graphrush_core PUBLIC GRAPHRUSH_HAS_OPENMP=1)
endif()
