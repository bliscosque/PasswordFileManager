cmake_minimum_required(VERSION 3.16)
project(PasswordFileManager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sqlite3 STATIC external/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC external/sqlite)

add_executable(PasswordFileManager 
    src/main.cpp
    src/core/Database.cpp
    src/core/FileParser.cpp
    src/core/FileGenerator.cpp
    src/core/Models.cpp
)

add_executable(testDatabase
    src/tests/testDatabase.cpp
    src/core/Database.cpp
)

add_executable(testFileParser
    src/tests/testFileParser.cpp
    src/core/FileParser.cpp
    src/core/Models.cpp
)

add_executable(testFileGenerator
    src/tests/testFileGenerator.cpp
    src/core/FileGenerator.cpp
    src/core/Models.cpp
    src/core/Database.cpp
)

target_include_directories(PasswordFileManager PRIVATE src)
target_link_libraries(PasswordFileManager PRIVATE sqlite3)

target_include_directories(testDatabase PRIVATE src)
target_link_libraries(testDatabase PRIVATE sqlite3)

target_include_directories(testFileParser PRIVATE src)

target_include_directories(testFileGenerator PRIVATE src)
target_link_libraries(testFileGenerator PRIVATE sqlite3)