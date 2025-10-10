# Database creation
```sql
CREATE TABLE file (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    filename TEXT NOT NULL,
    last_modified DATETIME
);

CREATE TABLE config (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    file_id INTEGER NOT NULL,
    description TEXT,
    content TEXT NOT NULL,
    FOREIGN KEY (file_id) REFERENCES file(id)
);

```