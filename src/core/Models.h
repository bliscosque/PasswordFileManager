#pragma once
#include <string>
#include <vector>

struct Config {
    int id;
    int file_id;
    std::string description;
    std::string content;
};

struct File {
    int id;
    std::string filename;
    std::vector<Config> configs;
};