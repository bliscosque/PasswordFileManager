#include <iostream>
#include <string>
#include "core/Database.h"
#include "core/FileParser.h"
//#include "tests/testDB.h"

void printUsage() {
    std::cout << "Usage: PasswordFileManager <command> <filePath> " << std::endl;
    std::cout << "Commands: " << std::endl;
    std::cout << "  lock <filePath>   - Encrypt and store the file in the DB" << std::endl;
    std::cout << "  unlock <filePath> - Decrypt and retrieve the file from the DB" << std::endl;
}


int main(int argc, char* argv[]) {
    //testDB();
    if (argc != 3) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];
    std::string filePath = argv[2];

    try {
        Database db("pass.db");
        
        if (command == "lock") {
            
            if (db.isFileLocked(filePath)) {
                std::cerr << "File already locked or in use." << std::endl;
                return 1;
            }
            File file = FileParser::parseFile(filePath);
            db.saveFileModel(file);
            db.setFileLocked(filePath, true);

        // } else if (command == "unlock") {
        //     if (! db.isFileLocked(filePath)) {
        //         std::cerr << "File already unlocked" << std::endl;
        //         return 1;
        //     }
        //     FileGenerator generator(db);
        //     generator.generate(filePath);
        //     db.setFileLocked(filePath, false);
        } else {
            printUsage();
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}