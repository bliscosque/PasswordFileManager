#include "FileParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

File FileParser::parseFile(const std::string& filePath) {
    File file;
    file.filename = filePath;
    file.locked = false;
    file.configs = parseConfigs(filePath);
    return file;
}

std::vector<Config> FileParser::parseConfigs(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("File to open file: "+ filePath);
    }

    std::vector<Config> configs;
    std::string line;
    std::ostringstream section;
    std::string description;

    while (std::getline(in, line)) {
        if (line.empty()) {
            //full block
            if (!section.str().empty()) {
                Config cfg;
                cfg.description = description;
                cfg.content = section.str();
                configs.push_back(cfg);
                section.str("");
                section.clear();
                description.clear();
            }
        //comment
        } else if (line[0] == '#') {
            if (!description.empty()) description += "\n";
            description+=line;
        } else {
            if (!section.str().empty()) section << "\n";
            section << line;
        }
    }

    //last block
    if (!section.str().empty()) {
        Config cfg{0,description, section.str()};
        configs.push_back(cfg);
    }
    return configs;
}