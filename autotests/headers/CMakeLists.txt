# Test if all the installed headers can be included individually. Requires installed headers.

add_test(
    NAME HeadersTest
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/headers_test.sh
                "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}"  KReport kreport .
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Just show dependent files in the project:
add_custom_target(HeadersTest SOURCES headers_test.sh HeadersTestInclude.cmake)
