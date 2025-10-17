#include "Database.h"
#include "Models.h"
#include <iostream>
#include <vector>

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
    int id = getFileId(filePath);

    sqlite3_stmt* stmt = nullptr;

    // new file
    if (id==-1) {
        //std::cout << "[DB] Inserting new file: " << filePath << " locked=" << locked << std::endl;
        const char* sql = "INSERT INTO file (filename, locked) VALUES (?, ?); ";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
           throw std::runtime_error("Failed to prepare INSERT statement");
        }
        sqlite3_bind_text(stmt, 1, filePath.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, locked ? 1 : 0);
    } else {
        //std::cout << "[DB] Updating file id=" << id << " locked=" << locked << std::endl;
        const char* sql = "UPDATE file SET locked= ? WHERE id = ?;";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
           throw std::runtime_error("Failed to prepare UPDATE statement");
        }
        sqlite3_bind_int(stmt, 1, locked ? 1 : 0);
        sqlite3_bind_int(stmt, 2, id);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert/update file entry");
    }

    sqlite3_finalize(stmt);
}

void Database::saveFileModel(const File &file)
{
    execute("BEGIN TRANSACTION;");

    try {
        //ensure that file is in "file" table
        insertOrUpdateFile(file.filename, file.locked);
        int fileId = getFileId(file.filename);
        if (fileId == -1) {
            throw std::runtime_error("Failed to retrieve file ID after insert/update");
        }

        // clean existing configs
        {
            sqlite3_stmt* delStmt = nullptr;
            const char* sqlDel = "DELETE FROM config WHERE file_id = ?;";
            if (sqlite3_prepare_v2(db, sqlDel, -1, &delStmt, nullptr) != SQLITE_OK) {
                throw std::runtime_error("Failed to prepare DELETE statement for configs");
            }
            sqlite3_bind_int(delStmt, 1, fileId);
            sqlite3_step(delStmt);
            sqlite3_finalize(delStmt);
        }

        // insert new configs
        sqlite3_stmt* insStmt = nullptr;
        const char* sqlInsert = "INSERT INTO config (file_id, description, content) VALUES (?, ?, ?);";

        if (sqlite3_prepare_v2(db, sqlInsert, -1, &insStmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare INSERT statement for configs");
        }

        for (const auto& cfg: file.configs) {
            sqlite3_bind_int(insStmt, 1, fileId);
            sqlite3_bind_text(insStmt, 2, cfg.description.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insStmt, 3, cfg.content.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(insStmt)!=SQLITE_DONE) {
                sqlite3_finalize(insStmt);
                throw std::runtime_error("Failed to insert config entry");
            }

            sqlite3_reset(insStmt);
            sqlite3_clear_bindings(insStmt);
        }
        sqlite3_finalize(insStmt);

        //commit
        execute("COMMIT");
        std::cout << "[DB] File model save successfully: " << file.filename << std::endl;

    } catch (const std::exception& e) {
        execute("ROLLBACK;");
        throw std::runtime_error(std::string("Error saving file model: ") + e.what());
    }
}

std::vector<Config> Database::fetchLogEntriesByFilename(const std::string& filename) {
    std::vector<Config> entries;

    //get fileID
    int fileId = getFileId(filename);
    if (fileId == -1) {
        throw std::runtime_error("File not found in DB: " + filename);
    }

    // Recover data 

    const char* sql = "SELECT description, content FROM config WHERE file_id = ? ORDER BY id ASC;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement for reading configs");
    }

    sqlite3_bind_int(stmt, 1, fileId);
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        Config cfg;
        cfg.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        cfg.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        entries.push_back(cfg);
    }

    sqlite3_finalize(stmt);

    return entries;
    
}