#include "Database.h"
#include <iostream>

Database::Database(const std::string& dbPath) : db(nullptr) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }
    initSchema();
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

void Database::initSchema() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS file (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            filename TEXT UNIQUE NOT NULL,
            locked INTEGER NOT NULL DEFAULT 0,
            last_modified DATETIME
        );

        CREATE TABLE IF NOT EXISTS config (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_id INTEGER NOT NULL,
            description TEXT,
            content TEXT NOT NULL,
            FOREIGN KEY (file_id) REFERENCES file(id) ON DELETE CASCADE
        );
    )";
    execute(sql);
}

void Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string msg = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("SQLite exec error: " + msg);
    }
}

bool Database::isFileLocked(const std::string& filePath) {
    const char* sql = "SELECT locked FROM file WHERE filename = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool locked = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        locked = sqlite3_column_int(stmt, 0) != 0;
    }

    sqlite3_finalize(stmt);
    return locked;
}

void Database::setFileLocked(const std::string& filePath, bool locked) {
    insertOrUpdateFile(filePath, locked);
}

int Database::getFileId(const std::string& filePath) {
    const char* sql = "SELECT id FROM file WHERE filename = ?;";
    sqlite3_stmt* stmt = nullptr;
    int id = -1;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return id;    
}

void Database::insertOrUpdateFile(const std::string& filePath, bool locked) {
    const char* sql = "INSERT INTO file (filename, locked) VALUES (?, ?) "
                      "ON CONFLICT(filename) DO UPDATE SET locked=excluded.locked;";
    
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, locked ? 1 : 0);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert/update file entry");
    }

    sqlite3_finalize(stmt);
}