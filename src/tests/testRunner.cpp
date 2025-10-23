#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

bool runTest(const std::string& name, const std::string& executable) {
    std::cout << "\n=== Running " << name << " ===\n";
    
    std::string cmd;
    #ifdef _WIN32
    cmd = executable + ".exe";
    #else
    cmd = "./" + executable;
    #endif
    
    fs::path execPath = fs::current_path() / cmd;
    
    if (!fs::exists(execPath)) {
        std::cout << "Error: Test executable not found: " << execPath.string() << "\n";
        return false;
    }
    
    int result = std::system(cmd.c_str());
    bool success = (result == 0);
    std::cout << name << ": " << (success ? "PASSED" : "FAILED") << "\n";
    return success;
}

int main() {
    std::cout << "\n=== Starting All Tests ===\n";
    
    // Define test executables without extension
    std::vector<std::pair<std::string, std::string>> tests = {
        {"Crypto Tests", "testCrypto"},
        {"Database Tests", "testDatabase"},
        {"FileParser Tests", "testFileParser"},
        {"FileGenerator Tests", "testFileGenerator"},
        {"Controller Tests", "testController"}
    };
    
    int passed = 0;
    std::vector<std::string> failed;
    
    for (const auto& test : tests) {
        if (runTest(test.first, test.second)) {
            passed++;
        } else {
            failed.push_back(test.first);
        }
        std::cout << std::string(40, '-') << "\n";
    }
    
    // Summary
    std::cout << "\n=== Test Summary ===\n";
    std::cout << "Total Tests: " << tests.size() << "\n";
    std::cout << "Passed: " << passed << "\n";
    std::cout << "Failed: " << failed.size() << "\n";
    
    if (!failed.empty()) {
        std::cout << "\nFailed Tests:\n";
        for (const auto& test : failed) {
            std::cout << "- " << test << "\n";
        }
        return 1;
    }
    
    std::cout << "\nAll tests passed successfully!\n";
    return 0;
}