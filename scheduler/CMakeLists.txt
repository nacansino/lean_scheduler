#==============================================================
# Project Information
#==============================================================

cmake_minimum_required(VERSION 3.8)   # set minimum

#==============================================================
# Compiler standards
#==============================================================
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#==============================================================
# include directory setting
#==============================================================
include_directories(..)

#==============================================================
# Compile as library
#==============================================================

#device under test, including common
add_library(LEAN_SCHEDULER STATIC Scheduler.cpp)