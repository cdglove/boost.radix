#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.8)
project(boost.radix VERSION 0.1 LANGUAGES CXX)

##############################################################################
# Global Settings
##############################################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

##############################################################################
# Options
##############################################################################
option (Radix_BUILD_EXAMPLES "Build examples" OFF)
option (Radix_BUILD_TESTS "Build tests" ON)
option (Radix_BUILD_BENCHMARKS "Build benchmarks" OFF)
option (Radix_BUILD_DOCS "Build benchmarks" OFF)

##############################################################################
# Look for the rest of Boost
##############################################################################
find_package(Boost REQUIRED)

##############################################################################
# Setup the 'radix' header-only library target, along with its install target
# and exports.
##############################################################################
add_library(radix INTERFACE)
target_include_directories(
    radix INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    $<INSTALL_INTERFACE:include/*.hpp>)

target_link_libraries(
	radix INTERFACE Boost::boost)

file(GLOB_RECURSE BOOST_RADIX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR} include/*.hpp)
target_sources(radix INTERFACE ${BOOST_RADIX_HEADERS})

#
#  HACK: have the files showing in the IDE, under the name 'moduleA_ide'
#
add_custom_target(boost.radix.ide SOURCES ${BOOST_RADIX_HEADERS})

##############################################################################
# Setup subdirectories and testing
##############################################################################
enable_testing()

if(Radix_BUILD_BENCHMARKS)
	add_subdirectory(benchmark)
endif()

if(Radix_BUILD_DOCS)
	add_subdirectory(doc)
endif()

if(Radix_BUILD_EXAMPLES)
	add_subdirectory(example)
endif()

if(Radix_BUILD_TESTS)
	add_subdirectory(test)
endif()
