# SQLite Usage Guide (No ORM)

Yeh documentation project main **SQLite ko bina kisi ORM** ke directly use karne ka tareeqa explain karti hai. Hum sirf SQLite ka C API (`sqlite3.h`) use karenge — koi hidden magic nahi, pura control SQL aur C++ ke haath main.

> **Note:** Purani MongoDB-style JSON database hata di gayi hai. Ab data SQLite main store hoga.

---

## 1. System Requirements

Pehle confirm karein ke SQLite installed hai:

```bash
pkg-config --modversion sqlite3        # version (e.g. 3.53.4)
ls /usr/include/sqlite3.h              # header
ldconfig -p | grep sqlite              # library
```

Agar missing ho:
```bash
# Debian/Ubuntu
sudo apt install libsqlite3-dev

# Arch
sudo pacman -S sqlite
```

---

## 2. CMake Setup

`CMakeLists.txt` main sirf do lines add karein:

```cmake
find_package(SQLite3 REQUIRED)

if(TARGET SQLite3::SQLite3)
    target_link_libraries(BankApp PRIVATE SQLite3::SQLite3)
else()
    target_link_libraries(BankApp PRIVATE SQLite::SQLite3)  # purana CMake
endif()
```

`FindSQLite3` CMake ke sath built-in aata hai. Naya target `SQLite3::SQLite3` hai; purane CMake main `SQLite::SQLite3` deprecated naam se milta hai — isliye dono handle kiye hain.

---

## 3. Core Lifecycle (Zaroori 4 Steps)

Har query ka pattern yeh hai:

```
sqlite3_open_v2  →  sqlite3_prepare_v2  →  bind → step → finalize  →  sqlite3_close
     (connect)         (compile SQL)        (values)  (run)  (cleanup)     (close)
```

- **prepare** SQL ko parse + compile karta hai (injection-safe, fast).
- **bind** values ko `?` placeholders se jorta hai.
- **step** query chalata hai: `SQLITE_ROW` per row, `SQLITE_DONE` jab khatam.
- **finalize** har statement ke baad lazmi — warna memory leak.

---

## 4. Connection Open / Close

### Raw C API

```cpp
#include <sqlite3.h>

sqlite3* db = nullptr;
int rc = sqlite3_open_v2("./data/bank.db", &db,
                         SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                         nullptr);
if (rc != SQLITE_OK) {
    std::println(std::cerr, "Cannot open DB: {}", sqlite3_errmsg(db));
    // handle error
}

// ... queries ...

sqlite3_close(db);
```

`":memory:"` path pass karein to in-memory database banega (testing ke liye).

### Recommended: RAII wrapper

`sqlite3*` aur `sqlite3_stmt*` ko manual band karna bhoolna aasan hai. Isliye RAII wrapper behtar hai:

```cpp
// includes/utils/sqlite.hpp  (maujood hai — src/utils/sqlite.cpp main implement)
#pragma once
#include <sqlite3.h>
#include <stdexcept>
#include <string>

namespace sqlite {

class Database {
public:
    explicit Database(const std::string& path) {
        if (sqlite3_open_v2(path.c_str(), &db_,
                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db_));
    }
    ~Database() { if (db_) sqlite3_close(db_); }

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    void exec(const std::string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err ? err : "unknown";
            sqlite3_free(err);
            throw std::runtime_error(msg);
        }
    }

    sqlite3* raw() const { return db_; }

private:
    sqlite3* db_ = nullptr;
};

} // namespace sqlite
```

---

## 5. Schema Creation

```cpp
db.exec(R"(
CREATE TABLE IF NOT EXISTS customers(
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    name     TEXT    NOT NULL,
    email    TEXT    NOT NULL UNIQUE,
    password TEXT    NOT NULL,
    phone    TEXT,
    address  TEXT
);

CREATE TABLE IF NOT EXISTS accounts(
    account_number INTEGER PRIMARY KEY,
    customer_id    INTEGER NOT NULL REFERENCES customers(id),
    account_type   INTEGER NOT NULL DEFAULT 0,
    balance        INTEGER NOT NULL DEFAULT 0,
    status         INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS cards(
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    card_number    TEXT NOT NULL UNIQUE,
    card_type      INTEGER NOT NULL,
    cvv            TEXT NOT NULL,
    status         INTEGER NOT NULL,
    expiry_date    TEXT,
    account_number INTEGER REFERENCES accounts(account_number)
);
)");
```

**Recommended PRAGMAs** (open ke foran baad):

```cpp
db.exec("PRAGMA foreign_keys = ON;");   // FK constraints enforce kare
db.exec("PRAGMA journal_mode = WAL;");  // concurrent reads, better durability
db.exec("PRAGMA busy_timeout = 5000;"); // lock par 5s wait
```

---

## 6. Prepared Statement Wrapper

```cpp
class Statement {
public:
    Statement(sqlite3* db, const std::string& sql) {
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));
    }
    ~Statement() { if (stmt_) sqlite3_finalize(stmt_); }

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    void bind(int i, int v)               { sqlite3_bind_int(stmt_, i, v); }
    void bind(int i, long long v)         { sqlite3_bind_int64(stmt_, i, v); }
    void bind(int i, double v)            { sqlite3_bind_double(stmt_, i, v); }
    void bind(int i, const std::string& v){ sqlite3_bind_text(stmt_, i, v.c_str(), -1, SQLITE_TRANSIENT); }
    void bind(int i, std::nullptr_t)      { sqlite3_bind_null(stmt_, i); }

    bool step()  { return sqlite3_step(stmt_) == SQLITE_ROW; }
    void reset() { sqlite3_reset(stmt_); sqlite3_clear_bindings(stmt_); }

    int         col_int(int i)    { return sqlite3_column_int(stmt_, i); }
    long long   col_i64(int i)    { return sqlite3_column_int64(stmt_, i); }
    double      col_double(int i) { return sqlite3_column_double(stmt_, i); }
    std::string col_text(int i)   { const char* t = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, i)); return t ? t : ""; }

private:
    sqlite3_stmt* stmt_ = nullptr;
};
```

---

## 7. CRUD Examples

### 7.1 Insert

```cpp
Statement s(db.raw(), "INSERT INTO customers(name,email,password,phone,address) VALUES(?,?,?,?,?)");
s.bind(1, cust.name);
s.bind(2, cust.email);
s.bind(3, cust.password);
s.bind(4, cust.phone);
s.bind(5, cust.address);
s.step();                                  // SQLITE_DONE
long long new_id = sqlite3_last_insert_rowid(db.raw());
```

### 7.2 Find One

```cpp
Statement s(db.raw(), "SELECT id,name,email,balance FROM customers WHERE email = ?");
s.bind(1, email);
if (s.step()) {                            // ek row mili
    Customer c;
    c.id    = s.col_i64(0);                // (agar model main id rakhna ho)
    c.name  = s.col_text(1);
    c.email = s.col_text(2);
}
```

### 7.3 Find Many (loop)

```cpp
Statement s(db.raw(), "SELECT id,name,email FROM customers WHERE name LIKE ? ORDER BY name");
s.bind(1, "%" + search + "%");
std::vector<Customer> results;
while (s.step()) {                         // SELECT ka loop
    Customer c;
    c.name  = s.col_text(1);
    c.email = s.col_text(2);
    results.push_back(std::move(c));
}
```

### 7.4 Update

```cpp
Statement s(db.raw(), "UPDATE accounts SET balance = balance + ? WHERE account_number = ?");
s.bind(1, amount);
s.bind(2, account_number);
s.step();
int changed = sqlite3_changes(db.raw());   // kitni rows affect hui
```

### 7.5 Delete

```cpp
Statement s(db.raw(), "DELETE FROM cards WHERE card_number = ?");
s.bind(1, card_number);
s.step();
```

---

## 8. Transactions (Transfer / Atomic Operations)

Transfer jaise operations main **do updates** ek sath success ya fail hone chahiye:

```cpp
db.exec("BEGIN IMMEDIATE;");
try {
    // 1. Paisay kaat
    { Statement s(db.raw(), "UPDATE accounts SET balance = balance - ? WHERE account_number = ?");
      s.bind(1, amount); s.bind(2, from); s.step(); }

    // 2. Balance check (negative na ho)
    long long bal;
    { Statement s(db.raw(), "SELECT balance FROM accounts WHERE account_number = ?");
      s.bind(1, from); s.step(); bal = s.col_i64(0); }
    if (bal < 0) throw std::runtime_error("Insufficient funds");

    // 3. Paisay daal
    { Statement s(db.raw(), "UPDATE accounts SET balance = balance + ? WHERE account_number = ?");
      s.bind(1, amount); s.bind(2, to); s.step(); }

    // 4. Log transaction
    { Statement s(db.raw(), "INSERT INTO transactions(from_acc,to_acc,amount) VALUES(?,?,?)");
      s.bind(1, from); s.bind(2, to); s.bind(3, amount); s.step(); }

    db.exec("COMMIT;");
} catch (const std::exception& e) {
    db.exec("ROLLBACK;");
    std::println(std::cerr, "Transfer failed: {}", e.what());
}
```

`ROLLBACK` throw se pehle lazmi — warna partial changes reh jayenge.

---

## 9. Error Handling

Har SQLite call ka return code check karein. Helpers:

```cpp
if (rc != SQLITE_OK) {
    std::println(std::cerr, "SQLite error {}: {}", rc, sqlite3_errmsg(db));
}
```

Common codes: `SQLITE_OK`, `SQLITE_ROW`, `SQLITE_DONE`, `SQLITE_CONSTRAINT` (unique/FK violation), `SQLITE_BUSY` (locked), `SQLITE_ERROR`.

App ke `std::expected` pattern ke sath:

```cpp
std::expected<Customer, ValidationError> find_customer(const Database& db, const std::string& email);
```

---

## 10. Models ↔ Rows Mapping

Models (`includes/models/*.hpp`) ab **plain structs** hain (koi JSON macro nahi). Inko rows se manually bharein:

```cpp
Customer row_to_customer(const Statement& s) {
    Customer c;
    c.name    = s.col_text(0);
    c.email   = s.col_text(1);
    c.phone   = s.col_text(2);
    c.address = s.col_text(3);
    return c;
}
```

Enum ↔ integer mapping (jaise `AccountType`, `CardStatus`) explicitly karein:

```cpp
AccountType to_type(int v) {
    switch (v) {
        case 0: return AccountType::Current;
        case 1: return AccountType::Saving;
        default: return AccountType::Business;
    }
}
```

---

## 11. Tips & Rules

- **Kabhi** values ko string concatenate na karein — **hamesha** `?` bind use karein (SQL injection se bachne ke liye).
- `SQLITE_TRANSIENT` use karein taake SQLite string ki copy rakhe (aapka buffer temporary ho sakta hai).
- Har `prepare` ka ek `finalize` hona chahiye (RAII isay automatic kar deta hai).
- Reuse hoti queries ka prepared statement ek baar bana kar `reset()` karein.
- `email`, `card_number` par `UNIQUE` index lazmi; queries par `CREATE INDEX`.
- `sqlite3_last_insert_rowid()` insert ke foran baad padhein.
- WAL mode + `busy_timeout` concurrent use ko safe banate hain.

---

## 12. Quick Reference

| Kaam | Function |
| :--- | :--- |
| Open | `sqlite3_open_v2` |
| Close | `sqlite3_close` |
| One-off SQL | `sqlite3_exec` |
| Compile query | `sqlite3_prepare_v2` |
| Bind value | `sqlite3_bind_int/int64/double/text/null` |
| Run | `sqlite3_step` → `SQLITE_ROW` / `SQLITE_DONE` |
| Read column | `sqlite3_column_int/int64/double/text` |
| Cleanup | `sqlite3_finalize` |
| Last insert id | `sqlite3_last_insert_rowid` |
| Rows affected | `sqlite3_changes` |
| Error text | `sqlite3_errmsg` |
| Transaction | `BEGIN` / `COMMIT` / `ROLLBACK` |
