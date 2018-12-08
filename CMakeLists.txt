cmake_minimum_required(VERSION 3.2)

project(bulk VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

file(GLOB BULK_H
        "${PROJECT_SOURCE_DIR}/*.h",
        "${PROJECT_SOURCE_DIR}/extern/spdlog/include/*.h"
        )

file(GLOB BULK_SRC
        "${PROJECT_SOURCE_DIR}/*.cpp"
        )
		
include_directories("${PROJECT_SOURCE_DIR}/extern/spdlog/include")
include_directories("${PROJECT_SOURCE_DIR}/extern/spdlog")
add_executable(bulk ${BULK_SRC} ${BULK_H} )

set(CMAKE_CXX_STANDARD 14)

set_target_properties(bulk PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 14
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)

install(TARGETS bulk RUNTIME DESTINATION bin)

macro(install)
endmacro()

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest")

macro(install)
    _install(${ARGN})
endmacro()

add_subdirectory(./test)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT sosninroman92@gmail.com)

include (CPack)

enable_testing()

add_test(NAME test_cli COMMAND test_cli)
