cmake_minimum_required(VERSION 3.15)
project(bess-daimon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

option(BUILD_PYTHON_MODULE "Construir o módulo Python" ON)
option(BUILD_TESTS "Construir os testes unitários" OFF)

# --- dependencies ---

# pybind11 config
if(BUILD_PYTHON_MODULE)
    FetchContent_Declare(
      pybind11
      GIT_REPOSITORY https://github.com/pybind/pybind11.git
      GIT_TAG        v2.13.1 
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# catch2 config
if(BUILD_TESTS)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v3.4.0 
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# --- targets ---

# core library
add_library(bess_core STATIC 
    src/bess.cpp
)
target_include_directories(bess_core PUBLIC include)
set_target_properties(bess_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# python module
if(BUILD_PYTHON_MODULE)
    pybind11_add_module(bess src/binding.cpp)
    target_link_libraries(bess PRIVATE bess_core)
    install(TARGETS bess DESTINATION .)
endif()

# tests
if(BUILD_TESTS)
    add_executable(unit_tests test/unit_tests.cpp)
    target_link_libraries(unit_tests PRIVATE bess_core Catch2::Catch2WithMain)

    enable_testing()
    add_test(NAME UnitTests COMMAND unit_tests)
endif()