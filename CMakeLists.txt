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
    src/core/Controller.cpp
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

add_executable(testRunner
    src/tests/testRunner.cpp
)

add_executable(testCrypto
    src/tests/testCrypto.cpp
    src/core/Crypto.cpp
)

add_executable(testController
    src/tests/testController.cpp
    src/core/Controller.cpp
    src/core/Database.cpp
    src/core/FileParser.cpp
    src/core/FileGenerator.cpp
    src/core/Models.cpp
)

target_include_directories(PasswordFileManager PRIVATE src)
target_link_libraries(PasswordFileManager PRIVATE sqlite3)

# Find OpenSSL and expose a small crypto static library so all targets
# link consistently to the same implementation object.
find_package(OpenSSL REQUIRED)
add_library(crypto STATIC src/core/Crypto.cpp)
target_include_directories(crypto PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(crypto PUBLIC OpenSSL::Crypto)

# Link crypto into the main executable
target_link_libraries(PasswordFileManager PRIVATE crypto)

target_include_directories(testDatabase PRIVATE src)
target_link_libraries(testDatabase PRIVATE sqlite3 crypto)

target_include_directories(testFileParser PRIVATE src)

target_include_directories(testFileGenerator PRIVATE src)
target_link_libraries(testFileGenerator PRIVATE sqlite3 crypto)

# Ensure testFileParser can include headers and link crypto if needed
target_link_libraries(testFileParser PRIVATE crypto)

# Configure new test targets
target_include_directories(testRunner PRIVATE src)

target_include_directories(testCrypto PRIVATE src)
target_link_libraries(testCrypto PRIVATE OpenSSL::Crypto)

target_include_directories(testController PRIVATE src)
target_link_libraries(testController PRIVATE sqlite3 crypto)