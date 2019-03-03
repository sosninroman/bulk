file(GLOB_RECURSE example_SRC
        "./*.cpp"
        )

file(GLOB_RECURSE example_HEADER
        "./*.h"
        )

include_directories(../include/)
include_directories(./)

add_executable(example_cli ${example_SRC} ${example_HEADER} )

target_link_libraries(example_cli async)
