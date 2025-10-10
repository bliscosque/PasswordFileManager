#include <sqlite3.h>
#include <iostream>
#include "testDB.h"

void testDB() {
    sqlite3* db;
    if(sqlite3_open("test.db", &db) == SQLITE_OK) {
        std::cout << "Database created successfully!" << std::endl;
        sqlite3_close(db);
    } else {
        std::cerr << "Failed to create database" << std::endl;
    }
}