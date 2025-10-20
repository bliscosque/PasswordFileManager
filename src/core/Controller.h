#pragma once

#include <string>

class Controller {
public:
    // Execute the command using given database file
    // Returns 0 on success, non-zero on error
    int run(const std::string& command, const std::string& filePath, const std::string& dbPath = "pass.db");
};
