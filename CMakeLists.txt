cmake_minimum_required(VERSION 3.22)

project(tiny-unicode  LANGUAGES C)

set(CMAKE_C_STANDARD 23)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(include)

add_library(tiny-unicode src/tiny-unicode.c)

enable_testing()

add_executable(tiny-unicode-tests test/test.c)
target_link_libraries(tiny-unicode-tests tiny-unicode)

add_test(tiny-unicode-tests tiny-unicode-tests)
