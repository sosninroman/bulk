cmake_minimum_required(VERSION 3.2)

project(async VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package (Threads)

file(GLOB ASYNC_H
        "${PROJECT_SOURCE_DIR}/include/*.h",
        "${PROJECT_SOURCE_DIR}/extern/spdlog/include/*.h"
        )

file(GLOB ASYNC_SRC
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        )
		
set(CMAKE_CXX_STANDARD 14)

if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
endif(WIN32)

include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("${PROJECT_SOURCE_DIR}/extern/spdlog/include/")

add_library(async SHARED ${ASYNC_H} ${ASYNC_SRC} )

set_target_properties(async PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 14
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)

target_link_libraries(async
  ${CMAKE_THREAD_LIBS_INIT}
  )


add_subdirectory(./example)

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
