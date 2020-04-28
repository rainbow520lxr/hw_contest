cmake_minimum_required(VERSION 3.5)

project(BankTransfer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FILE(GLOB_RECURSE CURRENT_SOURCES src/*.cpp)

add_executable(BankTransfer
    main.cpp
    )
