#pragma once
#include "Models.h"
#include <string>
#include <vector>

class FileParser {
public:
    static File parseFile(const std::string& filePath);

private:
    static std::vector<Config> parseConfigs(const std::string& filePath);
};