#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required(VERSION 3.8)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

##############################################################################
# Setup the 'radix' header-only library target, along with its install target
# and exports.
##############################################################################
add_library(radix INTERFACE)
target_include_directories(
    radix INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    $<INSTALL_INTERFACE:include>)

# Export the `radix` library into a RadixConfig.cmake file
install(TARGETS radix EXPORT RadixConfig)
install(EXPORT RadixConfig      DESTINATION lib/cmake/radix)
install(DIRECTORY include/boost DESTINATION include FILES_MATCHING PATTERN "*.hpp")

##############################################################################
# Look for the rest of Boost
##############################################################################
find_package(Boost REQUIRED)

##############################################################################
# Setup the `check` target to build and then run all the tests and examples.
##############################################################################
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Build and then run all the tests and examples.")

##############################################################################
# Setup subdirectories and testing
##############################################################################
enable_testing()
find_program(MEMORYCHECK_COMMAND valgrind)
if (MEMORYCHECK_COMMAND)
    message(STATUS "Found Valgrind: ${MEMORYCHECK_COMMAND}")
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")
else()
    message("Valgrind not found")
endif()
include(CTest)

#add_subdirectory(benchmark)
#add_subdirectory(doc)
#add_subdirectory(example)
add_subdirectory(test)