# Project configuration
cmake_minimum_required(VERSION 3.15)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Dependencies
find_package(pybind11 CONFIG REQUIRED)
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(Catch2)


# Core library
add_library(bess STATIC
    src/bess.cpp
    src/utils.cpp
)
set_target_properties(bess PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(bess PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Python module
pybind11_add_module(_bess src/binding.cpp)
target_link_libraries(_bess PRIVATE bess)
install(TARGETS _bess DESTINATION bess_module)

# Tests
enable_testing()
add_executable(UnitTests test/tests.cpp)
target_link_libraries(UnitTests PRIVATE bess Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(UnitTests)