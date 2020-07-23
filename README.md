# lean_scheduler

A lean, lightweight, and portable scheduler designed for embedded C++ applications.

## Target specifications

1. Ultra lean and lightweight (only xxx bytes of code). 
2. Extremely portable for any embedded C++ applications.
3. Uses cooperative task scheduling.
4. **No external dependency**. Only uses `stdint.h` for standard types.
5. Included automated host build through CMake, and test suite via CppUTest

## Requirements

- C++ compiler
- Timer peripheral