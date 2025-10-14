#include "core/FileParser.h"
#include <iostream>
#include <fstream>
#include <cassert>

//Helper function - create temporary test file
void createTestFile(const std::string& path) {
    std::ofstream out(path);
    out << "# Config 1 description\n";
    out << "username=admin\n";
    out << "password=1234\n";
    out << "\n";
    out << "# Config 2 description\n";
    out << "server=example.com\n";
    out << "port=8080\n";
    out.close();
}

int main() {
    const std::string testPath = "test_config.txt";
    createTestFile(testPath);

    std::cout << "Testing FileParser with file: " << testPath << std::endl;

    //parse the file
    File parsedFile = FileParser::parseFile(testPath);

    assert(parsedFile.filename == testPath);
    assert(!parsedFile.locked);
    assert(parsedFile.configs.size() == 2);

    //1st block
    const auto& cfg1 = parsedFile.configs[0];
    assert(cfg1.description.find("# Config 1 description") != std::string::npos);
    assert(cfg1.content.find("username=admin") != std::string::npos);
    assert(cfg1.content.find("password=1234") != std::string::npos);

    //2nd block
    const auto& cfg2 = parsedFile.configs[1];
    assert(cfg2.description.find("# Config 2 description") != std::string::npos);
    assert(cfg2.content.find("server=example.com") != std::string::npos);
    assert(cfg2.content.find("port=8080") != std::string::npos);    

    std::cout << "FileParser test passed successfully!" << std::endl;

    //cleaning the file
    std::remove(testPath.c_str());    
}