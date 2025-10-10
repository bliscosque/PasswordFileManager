#pragma once
#include "Models.h"
#include <string>

class FileGenerator {
public:
    static void generate(const File& file, const std::string& outputPath);
};