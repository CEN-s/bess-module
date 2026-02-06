cmake_minimum_required(VERSION 3.15)
project(bess-daimon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_UNIT_TESTS "Construir os testes unitários" OFF)
option(BUILD_PYTHON_MODULE "Construir o módulo Python" OFF)

# Biblioteca Core 
add_library(bess_core STATIC 
    src/bess.cpp
)
target_include_directories(bess_core PUBLIC include)
set_target_properties(bess_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Módulo Python
if(BUILD_PYTHON_MODULE)
    add_subdirectory(extern/pybind11) 
    pybind11_add_module(bess src/binding.cpp)
    target_link_libraries(bess PRIVATE bess_core)
endif()

# Testes
if(BUILD_UNIT_TESTS)
    add_subdirectory(extern/catch2) 
    add_executable(unit_tests test/unit_tests.cpp)
    target_link_libraries(unit_tests PRIVATE bess_core Catch2::Catch2WithMain)

    enable_testing()
    add_test(NAME UnitTests COMMAND unit_tests)
endif()