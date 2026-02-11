cmake_minimum_required(VERSION 3.15)
project(bess_module LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(bess_module)

target_include_directories(bess_module PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(bess_module PRIVATE 
    src/bess.cpp
    src/binding.cpp 
    src/utils.cpp
)

install(TARGETS bess_module DESTINATION .)