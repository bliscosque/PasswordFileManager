#include <iostream>
#include <string>
#include "core/Database.h"
#include "core/FileParser.h"
#include "core/FileGenerator.h"
#include "core/Controller.h"

void printUsage() {
    std::cout << "Usage: PasswordFileManager <command> <filePath> <key>" << std::endl;
    std::cout << "Commands: " << std::endl;
    std::cout << "  lock <filePath> <key>   - Encrypt and store the file in the DB" << std::endl;
    std::cout << "  unlock <filePath> <key> - Decrypt and retrieve the file from the DB" << std::endl;
}


int main(int argc, char* argv[]) {
    //testDB();
    if (argc != 4) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];
    std::string filePath = argv[2];
    std::string key = argv[3];
    Controller controller;
    return controller.run(command, filePath, key, "pass.db");
}
