cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

#create project
project(romManager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wpedantic -Wextra)

#add deps
include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#10.0.0")
CPMAddPackage("gh:jbeder/yaml-cpp#yaml-cpp-0.7.0")
CPMAddPackage("gh:jarro2783/cxxopts@3.1.1")


#variables
set (SOURCES 
    ../src/main.cpp
    ../src/logging.cpp
    ../src/config.cpp
)

set (DEPS 
    fmt::fmt
    yaml-cpp::yaml-cpp
)

set (LINKER_FLAGS
    -lncurses
)

#add exe
add_executable(${PROJECT_NAME} ${SOURCES})

#link deps
target_include_directories(${PROJECT_NAME} PUBLIC ../inc/)
target_link_libraries(${PROJECT_NAME} ${DEPS} ${LINKER_FLAGS})
