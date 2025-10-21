#include <iostream>
#include "../core/Database.h"

int main() {
    try {
    Database db("test.db");
    const std::string testKey(32, 'K');
        std::string file1 = "c:/Thiago/example1.txt";

        std::cout << "Testing file: " << file1 << std::endl;

        bool locked = db.isFileLocked(file1);
        std::cout << "File1 is is initially locked? " << locked << std::endl;

        db.setFileLocked(file1, true);
        std::cout << "After locking:  " << db.isFileLocked(file1) << std::endl;
        
        db.setFileLocked(file1, false);
        std::cout << "After unlocking:  " << db.isFileLocked(file1) << std::endl;

        int id=db.getFileId(file1);
        std::cout << "Database file ID: " << id << std::endl;

        std::cout << "All tests completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during tests: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}