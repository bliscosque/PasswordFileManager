#include "../core/Controller.h"
#include "../core/Database.h"
#include "../core/FileParser.h"
#include "../core/FileGenerator.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdexcept>

// Helper to create a test file
void createTestFile(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to create test file");
    }
    out << content;
    out.close();
}

// Helper to check if file exists
bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

int main() {
    std::cout << "\n=== Starting Controller Tests ===\n";
    
    const std::string testFile = "controller_test.txt";
    const std::string dbFile = "controller_test.db";
    const std::string testKey(32, 'K');
    
    // Cleanup any existing files from previous runs
    std::remove(testFile.c_str());
    std::remove(dbFile.c_str());
    
    try {
        // Test 1: Lock flow
        std::cout << "Testing lock operation..." << std::endl;
        const std::string content = 
            "# Test config\n"
            "key1=value1\n"
            "\n"
            "# Block 2\n"
            "key2=value2\n";
        
        createTestFile(testFile, content);
        
        Controller controller;
        // Garantir que o arquivo está desbloqueado primeiro
        std::remove(testFile.c_str());
        createTestFile(testFile, content);
        
        int result = controller.run("lock", testFile, testKey, dbFile);
        assert(result == 0 && "Lock operation failed");
        assert(!fileExists(testFile) && "File should be deleted after lock");
        
        // Test 2: Unlock flow (deve ser executado logo após o lock)
        std::cout << "Testing unlock operation..." << std::endl;
        result = controller.run("unlock", testFile, testKey, dbFile);
        assert(result == 0 && "Unlock operation failed");
        assert(fileExists(testFile) && "File should exist after unlock");
        
        // Test 3: Invalid command
        std::cout << "Testing invalid command..." << std::endl;
        result = controller.run("invalid", testFile, testKey, dbFile);
        assert(result == 1 && "Invalid command should return error");
        
        // Test 4: Lock already locked file
        std::cout << "Testing lock on locked file..." << std::endl;
        createTestFile(testFile, content);
        controller.run("lock", testFile, testKey, dbFile);
        result = controller.run("lock", testFile, testKey, dbFile);
        assert(result == 1 && "Locking already locked file should fail");
        
        // Test 5: Unlock with wrong key
        std::cout << "Testing unlock with wrong key..." << std::endl;
        result = controller.run("unlock", testFile, "wrongkey12345678901234567890123456789", dbFile);
        assert(result == 1 && "Unlock with wrong key should fail");
        
        // Cleanup
        std::remove(testFile.c_str());
        std::remove(dbFile.c_str());
        
        std::cout << "Controller tests completed successfully!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in Controller tests: " << e.what() << std::endl;
        std::remove(testFile.c_str());
        std::remove(dbFile.c_str());
        return 1;
    }
}