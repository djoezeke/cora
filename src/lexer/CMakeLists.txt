cmake_minimum_required(VERSION 3.12)

project(toy)

message(STATUS "=====================")
message(STATUS "CONFIGURING Toy Lexer")
message(STATUS "=====================")

# Setup the make environment. #
################################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Activate very verbose makefiles.
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Allow else() and endif() constructs (with empty parentheses)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

SET(CMAKE_BUILD_TYPE Debug CACHE STRING "default to debug" FORCE)

# Setup the cmake directory containing numerous cmake scripts and macros.
set(CMAKE_MODULE_PATH "${toy_SOURCE_DIR}/cmake")

# Search for the different needed libs. #
#########################################
FIND_PACKAGE(FLEX)
FIND_PACKAGE(BISON)

if(MSVC)
    if(NOT CMAKE_CL_64)
        message(FATAL_ERROR "Currently only 64bit compilation is supported!")
    endif()
endif()

# Put all source files into one variable. #
##########################################
set(LEXER_SOURCES
            tokens.cpp
            lexer.cpp
            main.cpp
   )

set(LEXER_HEADERS
            tokens.h
            source.hpp
            location.hpp
    )

set(VER_MAJ 0)
set(VER_MIN 0)
set(VER_REV 1)

configure_file(config.in ${toy_SOURCE_DIR}/config.h)


FLEX_TARGET(Scanner lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp )
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

add_executable(toylex ${LEXER_SOURCES} ${LEXER_HEADERS} ${FLEX_Scanner_OUTPUTS})

# Compiler-dependent and build-depended flags:
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(toylex PRIVATE -Wall)
elseif(MSVC)
    target_compile_options(toylex PRIVATE /W4 /permissive-)
    target_compile_definitions(toylex PRIVATE YY_NO_UNISTD_H)
    target_compile_definitions(toylex PRIVATE _SCL_SECURE_NO_WARNINGS)
    target_compile_definitions(toylex PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(toylex PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/tokens.cpp PROPERTIES COMPILE_DEFINITIONS __STDC_VERSION__=199901L)
endif()

# Debug or release ?
target_compile_definitions(toylex PRIVATE $<$<CONFIG:Debug>:_DEBUG>)

target_compile_features(toylex PRIVATE cxx_std_17)


if(MSVC)
    source_group(Header\ Files FILES ${LEXER_HEADERS})
    source_group(Source\ Files FILES ${LEXER_SOURCES})
endif()
