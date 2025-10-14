#pragma once
#include <string>
#include <vector>

struct Config {
    int id = 0;
    std::string description;
    std::string content;
};

struct File {
    int id = 0;
    std::string filename;
    bool locked = false;
    std::vector<Config> configs;
};