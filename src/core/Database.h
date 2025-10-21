#pragma once
#include <string>
#include "Models.h"
#include <sqlite3.h>
#include <vector>
#include "Crypto.h"

class Database {
public:
    Database(const std::string& dbPath);
    ~Database();

    void initSchema();
    bool isFileLocked(const std::string& filePath);
    void setFileLocked(const std::string& filePath, bool locked);

    int getFileId(const std::string& FilePath);
    void insertOrUpdateFile(const std::string& filePath, bool locked);

    void saveFileModel(const File& file);

    // Now requires encryption key to encrypt/decrypt content field
    void saveFileModel(const File& file, const std::string& key);

    std::vector<Config> fetchLogEntriesByFilename(const std::string& fileName, const std::string& key);
    

private:
    sqlite3* db;

    void execute(const std::string& sql);
};