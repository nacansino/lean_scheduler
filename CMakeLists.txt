#==============================================================
# Simple CMake used to run the demo application
#==============================================================
set(APP_NAME "demo")

cmake_minimum_required(VERSION 3.8)   # set minimum

project(${APP_NAME} C CXX) # set project name

# C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Test Configuration
include(CTest)
enable_testing()

#Include root directory for header lookup
include_directories(..)

#include the scheduler library
add_subdirectory(scheduler)

#build the demo application
#add_executable(${APP_NAME} sample/demo.cpp)

#link the LEAN_SCHEDULER library
# target_link_libraries(
#     ${APP_NAME} 
#     PUBLIC LEAN_SCHEDULER
# )

# Pull CppUTest suite
#==============================================================
if(BUILD_TESTING)
    # Fetch CppUTest
    include(FetchContent)
    FetchContent_Declare(
        CppUTest
        GIT_REPOSITORY https://github.com/cpputest/cpputest.git
        GIT_TAG        latest-passing-build # or use release tag, eg. v3.8
    )
    # Set this to ON if you want to have the CppUTests in your project as well.
    set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
    FetchContent_MakeAvailable(CppUTest)

    #Build the test
    add_executable(TEST_LEAN_SCHEDULER 
        tests/AllTests.cpp
        tests/test_Lean_Scheduler.cpp)

    # The code below is NECESSARY to provide the subdirectories 
    # include access to the pulled resource (CppUTest)
    # Otherwise, the ff. line will not see the TARGET and hence will throw a CMake Error
    target_include_directories(TEST_LEAN_SCHEDULER PRIVATE ${CppUTest_SOURCE_DIR}/include)
    add_definitions(-ln -lg -c)

    target_include_directories(TEST_LEAN_SCHEDULER PRIVATE scheduler)

    # Link the code under test and the CppUTest libraryto the test suite
    target_link_libraries(TEST_LEAN_SCHEDULER PUBLIC 
        LEAN_SCHEDULER
        CppUTest 
        CppUTestExt
    )

    # Add test
    add_test(
        NAME TEST_LEAN_SCHEDULER
        COMMAND TEST_LEAN_SCHEDULER.exe -c
    )

endif()