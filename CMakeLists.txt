cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

#create project
project(ttrm)

set(TEST_EXE_NAME tests)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(DEBUG True)

# Checking build type
if (DEBUG)
    add_compile_options(-Wall -Wpedantic -Wextra)
else ()
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message("Compiling at optimazation level O3")
        add_compile_options(-O3)
    endif()
endif()


#add deps
include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#9.1.0")
CPMAddPackage("gh:jbeder/yaml-cpp#yaml-cpp-0.7.0")
CPMAddPackage("gh:jarro2783/cxxopts@3.1.1")
CPMAddPackage("gh:gabime/spdlog@1.11.0")
CPMAddPackage("gh:google/googletest@1.13.0")
# also requires ncurses 

#variables
set (SOURCES 
    ../src/main.cpp
    ../src/config.cpp
    ../src/window.cpp
    ../src/search.cpp
    ../src/utils.cpp
)

set (DEPS 
    fmt::fmt
    yaml-cpp::yaml-cpp
    cxxopts::cxxopts
    spdlog::spdlog
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set (LINKER_FLAGS

    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set (LINKER_FLAGS
        -lncurses
    )
endif()




# main target
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ../inc/)
target_link_libraries(${PROJECT_NAME} ${DEPS} ${LINKER_FLAGS})

# testing target
# add_executable(${TEST_EXE_NAME} test/test.cpp ../src/utils.cpp)
# target_link_libraries(${TEST_EXE_NAME} ${DEPS} gtest_main ${LINKER_FLAGS})