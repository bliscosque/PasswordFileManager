#include "FileGenerator.h"
#include <fstream>
#include <iostream>

FileGenerator::FileGenerator(Database &db): db(db) {}

bool FileGenerator::generateFile(const std::string &filename, const std::string &key)
{
    std::vector<Config> configs = db.fetchLogEntriesByFilename(filename, key);

    File file;
    file.filename = filename;
    file.configs = configs;

    writeFile(file);

    std::cout << "[FileGenerator] File generated successfully: " << filename <<std::endl;

    return true;
}

void FileGenerator::writeFile(const File &file)
{
    std::ofstream out(file.filename);
    if(!out.is_open()) {
        throw std::runtime_error("Filed to open output file: " + file.filename);
    } 

    for (size_t i=0;i<file.configs.size();++i) {
        const auto& cfg = file.configs[i];

        if (!cfg.description.empty()) {
            out << cfg.description << "\n";
        }
        if (!cfg.content.empty()) {
            out << cfg.content << "\n";
        }
        if (i+1 < file.configs.size()) {
            out << "\n";
        }
    }

    out.close();
}
