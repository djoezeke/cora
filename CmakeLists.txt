# ===================================================================================
# Copyright © 2026 Sackey Ezekiel Etrue
# Distributed under the terms of the Apache License, Version 2.0.
# ===================================================================================

cmake_minimum_required(VERSION 3.10...4.1)

#------------------------------------------------------------------------------------
# Project and version.
#------------------------------------------------------------------------------------

project(Cora LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


#------------------------------------------------------------------------------------
# Add Project subdirectories
#------------------------------------------------------------------------------------

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(tools)

# ===================================================================================
