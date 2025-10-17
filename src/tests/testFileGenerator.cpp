#include "../core/FileGenerator.h"
#include "../core/Models.h"
#include "../core/Database.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>


//Helper function - read full file
std::string readFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int main() {
    std::cout << "[TEST] Starting testFileGenerator..." << std::endl;

    try {
        const std::string testFilename = "test_output.txt";

        Database db(":memory:");

        File file;
        file.filename = testFilename;

        Config cfg1;
        cfg1.description = "# Config 1 description\n# Config 1 description 2";
        cfg1.content = "username=admin\npassword=1234";

        Config cfg2;
        cfg2.description = "# Config 2 description";
        cfg2.content = "server=example.com\nport=8080";

        file.configs = {cfg1, cfg2};

        db.saveFileModel(file);

        FileGenerator generator(db);
        generator.generateFile(testFilename);

        std::string content = readFile(testFilename);
        std::string expected =
            "# Config 1 description\n"
            "# Config 1 description 2\n"
            "username=admin\n"
            "password=1234\n"
            "\n"
            "# Config 2 description\n"
            "server=example.com\n"
            "port=8080\n";

        std::cout << "\n[DEBUG] File content:\n" << content << std::endl;
        assert(content == expected && "Generated file content does not match expected output");

        // cleanup

        std::cout << "FileGenerator test passed successfully!" << std::endl;

        std::remove(testFilename.c_str());
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
    }
    return 0;
}