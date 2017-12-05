#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

find_package(Boost REQUIRED unit_test_framework)

##############################################################################
# Helper function to add tests
##############################################################################
function(add_radix_test target)
    set(radix_test_name test.radix.${target})
    add_executable(${radix_test_name} "${target}.cpp")
    target_link_libraries(${radix_test_name} PRIVATE radix Boost::unit_test_framework)
    add_test(NAME ${radix_test_name} COMMAND ${radix_test_name})
endfunction()

add_radix_test(static_ibitstream)
#add_radix_test(obitstream)
add_radix_test(encode)
add_radix_test(decode)
add_radix_test(round_trip)