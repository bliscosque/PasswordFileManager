#include "Controller.h"
#include "Database.h"
#include "FileParser.h"
#include "FileGenerator.h"
#include <iostream>

int Controller::run(const std::string& command, const std::string& filePath, const std::string& key, const std::string& dbPath) {
    try {
        Database db(dbPath);

        if (command == "lock") {
            if (db.isFileLocked(filePath)) {
                std::cerr << "File already locked or in use." << std::endl;
                return 1;
            }
            File file = FileParser::parseFile(filePath);
            db.saveFileModel(file, key);
            db.setFileLocked(filePath, true);
            if (std::remove(filePath.c_str()) != 0) {
                // Revert lock if deletion failed
                db.setFileLocked(filePath, false);
                std::cerr << "Failed to delete file: " << filePath << std::endl;
                return 1;
            }

        } else if (command == "unlock") {
            if (! db.isFileLocked(filePath)) {
                std::cerr << "File already unlocked" << std::endl;
                return 1;
            }
            FileGenerator generator(db);
            generator.generateFile(filePath, key);
            db.setFileLocked(filePath, false);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
