cmake_minimum_required(VERSION 3.5)

project(BankTransfer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories($PROJECT_SOURCE_DIR)

add_executable(BankTransfer main.cpp algorithm.cpp threadpool.cpp tool.cpp)
