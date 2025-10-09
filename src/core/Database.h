#pragman once
#include "Models.h"
#include <sqlite3.h>
#include <vector>

class Database {
public:
    Database(const std::string& dbPath);
    ~Database();

    void insertFileWithConfigs(const File& file);
    File getFileWithConfigs(int fileId);

private:
    sqlite3* db;
}