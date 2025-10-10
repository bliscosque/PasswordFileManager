#pragma once
#include "Models.h"
#include <string>

class FileParser {
public:
    static File parse(const std::string& filePath);
};