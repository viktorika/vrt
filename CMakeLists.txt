cmake_minimum_required (VERSION 3.27.2)

project(Vrt VERSION 1.0.0)

set(TEST_PATH "test")
set(BENCH_PATH "benchmark")

include_directories(/usr/local/include third_party/phmap)
link_directories(/usr/local/lib)

add_compile_options(-O2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test_vrt ${TEST_PATH}/test_vrt.cc)
target_include_directories(test_vrt PUBLIC include/)
target_link_libraries(test_vrt gtest)

add_executable(test_vrt_node ${TEST_PATH}/test_vrt_node.cc)
target_include_directories(test_vrt_node PUBLIC include/)
target_link_libraries(test_vrt_node gtest)

add_executable(vrt_benchmark ${BENCH_PATH}/vrt_benchmark.cc)
target_include_directories(vrt_benchmark PUBLIC include/)
target_link_libraries(vrt_benchmark benchmark)
