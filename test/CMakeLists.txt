#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017-2018
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

find_package(Boost REQUIRED unit_test_framework)

##############################################################################
# Helper function to add tests
##############################################################################
function(add_radix_test target_file)
	string(REPLACE "/" "." target_name ${target_file}) 
    set(radix_test_name boost.radix.test.${target_name})
    add_executable(${radix_test_name} "${target_file}.cpp")
    target_link_libraries(${radix_test_name} PRIVATE radix Boost::unit_test_framework)
    target_compile_definitions(${radix_test_name} PRIVATE BOOST_ERROR_CODE_HEADER_ONLY BOOST_ALL_NO_LIB)
    add_test(NAME ${radix_test_name} COMMAND ${radix_test_name})
endfunction()

add_radix_test(bitstreams)
add_radix_test(encode)
add_radix_test(decode)
add_radix_test(codec/rfc4648)
