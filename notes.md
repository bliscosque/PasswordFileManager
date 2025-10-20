# Database creation
```sql
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

```
