cmake_minimum_required(VERSION 3.15)
project(bess-daimon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

option(BUILD_PYTHON_MODULE "Construir o módulo Python" ON)

# --- dependencies ---

# pybind11 config
if(BUILD_PYTHON_MODULE)
    FetchContent_Declare(
      pybind11
      GIT_REPOSITORY https://github.com/pybind/pybind11.git
      GIT_TAG        v3.0.1 
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# --- targets ---

# core library
add_library(bess_core STATIC 
    src/bess.cpp
)
if(MSVC)
    target_compile_options(bess_core PRIVATE /W4)
else()
    target_compile_options(bess_core PRIVATE 
        -Wall 
        -Wextra 
	-Werror
	-Wpedantic
        -std=c++20
    )
endif()
target_include_directories(bess_core PUBLIC include)
set_target_properties(score PROPERTIES POSITION_INDEPENDENT_CODE ON)

# python module
if(BUILD_PYTHON_MODULE)
    pybind11_add_module(bess src/binding.cpp)
    target_link_libraries(bess PRIVATE bess_core)
    install(TARGETS bess DESTINATION .)
endif()