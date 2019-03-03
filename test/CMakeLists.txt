file(GLOB_RECURSE test_SRC
        "./*.cpp"
        )

file(GLOB_RECURSE test_HEADER
        "./*.h"
        )

include_directories(../async/)
include_directories(./)

add_executable(test_cli gtest.cpp ${test_SRC} ${test_HEADER} )

target_link_libraries(test_cli gtest)
