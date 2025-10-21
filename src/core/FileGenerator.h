#pragma once
#include "Database.h"
#include "Models.h"
#include <string>
#include <vector>


class FileGenerator {
public:
    explicit FileGenerator(Database& db);
    // generateFile now takes encryption key to decrypt content before writing
    bool generateFile(const std::string& fileName, const std::string& key);

private:
    Database& db;
    static void writeFile(const File& file);
};