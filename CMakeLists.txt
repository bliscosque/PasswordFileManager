cmake_minimum_required(VERSION 3.16)
project(PasswordFileManager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sqlite3 STATIC external/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC external/sqlite)

add_executable(PasswordFileManager 
    src/main.cpp
    src/core/Database.cpp
)

target_include_directories(PasswordFileManager PRIVATE src)
target_link_libraries(PasswordFileManager PRIVATE sqlite3)