#pragma once
#include <string>
#include "Models.h"
#include <sqlite3.h>
#include <vector>

class Database {
public:
    Database(const std::string& dbPath);
    ~Database();

    void initSchema();
    bool isFileLocked(const std::string& filePath);
    void setFileLocked(const std::string& filePath, bool locked);

    int getFileId(const std::string& FilePath);
    void insertOrUpdateFile(const std::string& filePath, bool locked);
    

private:
    sqlite3* db;

    void execute(const std::string& sql);
};