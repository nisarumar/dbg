cmake_minimum_required(VERSION 3.22)
project(dbg
        VERSION 1.0
	DESCRIPTION "dbg built on ptrace" )

add_compile_options(-std=c++14)
add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(ext/libelfin ext/linenoise include)

add_executable(dbg)
target_sources (dbg PRIVATE "src/dbg.cpp" "ext/linenoise/linenoise.c")

add_executable(hello examples/hello.cpp)
set_target_properties(hello
                      PROPERTIES COMPILE_FLAGS "-g -O0")

add_executable(variable examples/variable.cpp)
set_target_properties(variable
                      PROPERTIES COMPILE_FLAGS "-gdwarf-2 -O0")

add_executable(unwinding examples/stack_unwinding.cpp)
set_target_properties(unwinding
                      PROPERTIES COMPILE_FLAGS "-g -O0")


add_custom_target(
   libelfin
   COMMAND make
   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ext/libelfin
)
target_link_libraries(dbg
                      ${PROJECT_SOURCE_DIR}/ext/libelfin/dwarf/libdwarf++.so
                      ${PROJECT_SOURCE_DIR}/ext/libelfin/elf/libelf++.so)
add_dependencies(dbg libelfin)
