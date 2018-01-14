#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

include(ExternalProject)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "")
set(benchmark_args "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external/benchmark")
list(APPEND benchmark_args "-DCMAKE_BUILD_TYPE=Release")
ExternalProject_Add(google_benchmark 
	GIT_REPOSITORY https://github.com/google/benchmark.git
	GIT_TAG v1.3.0
	CMAKE_ARGS ${benchmark_args})

##############################################################################
# Helper function to add benchmarks
##############################################################################
function(add_radix_bench target_file)
	string(REPLACE "/" "." target_name ${target_file}) 
    set(radix_test_name boost.radix.bench.${target_name})
    add_executable(${radix_test_name} "${target_file}.cpp")
    target_link_libraries(${radix_test_name} PRIVATE radix benchmark)
    if(WIN32)
   		target_link_libraries(${radix_test_name} PRIVATE Shlwapi)
   	else()
   		target_link_libraries(${radix_test_name} PRIVATE pthread)
   	endif()
    target_include_directories(${radix_test_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/external/benchmark/include)
    add_test(NAME ${radix_test_name} COMMAND ${radix_test_name})
endfunction()

link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/benchmark/lib)
add_radix_bench(codec/rfc4648)
add_radix_bench(codec/base64_reference)
add_radix_bench(codec/base64_beast)
