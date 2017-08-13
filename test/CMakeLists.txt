#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

# add_custom_target(tests COMMENT "Build all the unit tests.")
# add_dependencies(check tests)
find_package(Boost REQUIRED unit_test_framework)


##############################################################################
# Generate tests that include each public header.
# The headers that were excluded above due to caveats are ignored here.
##############################################################################
# file(GLOB_RECURSE PUBLIC_HEADERS
#     RELATIVE "${Boost.Hana_SOURCE_DIR}/include"
#     "${Boost.Hana_SOURCE_DIR}/include/*.hpp"
# )
# list(REMOVE_ITEM PUBLIC_HEADERS ${PUBLIC_HEADERS_REQUIRING_BOOST})

# add_custom_target(test.headers COMMENT "Build all the header-inclusion unit tests.")
# add_dependencies(tests test.headers)
# include(TestHeaders)
# generate_standalone_header_tests(
#     HEADERS ${PUBLIC_HEADERS}
#     EXCLUDE ${EXCLUDED_PUBLIC_HEADERS}
#     LINK_LIBRARIES hana
#     MASTER_TARGET test.headers
#     EXCLUDE_FROM_ALL
# )
# generate_standalone_header_tests(
#     HEADERS ${PUBLIC_HEADERS_REQUIRING_BOOST}
#     EXCLUDE ${EXCLUDED_PUBLIC_HEADERS}
#     LINK_LIBRARIES hana Boost::boost
#     MASTER_TARGET test.headers
#     EXCLUDE_FROM_ALL
# )

##############################################################################
# Check for ODR violations when linking several translation units
# (GitHub issue 75)
##############################################################################
# list(APPEND EXCLUDED_UNIT_TESTS "issues/github_75/*.cpp")
# boost_hana_target_name_for(github_75 "${CMAKE_CURRENT_LIST_DIR}/issues/github_75")
# add_executable(${github_75} EXCLUDE_FROM_ALL "issues/github_75/tu1.cpp" "issues/github_75/tu2.cpp")
# boost_hana_set_test_properties(${github_75})
# add_test(${github_75} "${CMAKE_CURRENT_BINARY_DIR}/${github_75}")
# add_dependencies(tests ${github_75})


##############################################################################
# Helper function to add tests
##############################################################################
function(add_radix_test target)
    set(radix_test_name test.radix.${target})
    add_executable(${radix_test_name} "${target}.cpp")
    target_link_libraries(${radix_test_name} PRIVATE radix Boost::boost Boost::unit_test_framework)
    add_test(NAME ${radix_test_name} COMMAND ${radix_test_name})
endfunction()

add_radix_test(ibitstream)
add_radix_test(obitstream)
add_radix_test(encode)
add_radix_test(decode)
add_radix_test(round_trip)