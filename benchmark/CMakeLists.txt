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
ExternalProject_Add(google_benchmark 
	GIT_REPOSITORY https://github.com/google/benchmark.git
	GIT_TAG v1.3.0
	PREFIX "google_benchmark"
	INSTALL_COMMAND ""
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  	UPDATE_DISCONNECTED 1
  	UPDATE_COMMAND "")

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
    target_include_directories(${radix_test_name} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/google_benchmark/src/google_benchmark/include")
    add_test(NAME ${radix_test_name} COMMAND ${radix_test_name})
    add_dependencies(${radix_test_name} google_benchmark)
endfunction()

link_directories("${CMAKE_CURRENT_BINARY_DIR}/google_benchmark/src/google_benchmark-build/src")
#add_radix_bench(codec/rfc4648)
#add_radix_bench(codec/base64_reference)
add_radix_bench(codec/base64_beast)
