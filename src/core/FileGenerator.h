#pragma once
#include "Database.h"
#include "Models.h"
#include <string>
#include <vector>


class FileGenerator {
public:
    explicit FileGenerator(Database& db);
    bool generateFile(const std::string& fileName);

private:
    Database& db;
    static void writeFile(const File& file);
};