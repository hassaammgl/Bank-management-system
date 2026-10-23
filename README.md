# Cyrixon Bank App

A terminal-based bank management system written in modern **C++23**. It demonstrates how banks work with money under the hood — customers, accounts, session-gated services, and every deposit recorded in a local **SQLite** database. Passwords are hashed with **libsodium**, never stored in plain text.

For the full control-flow design (state machine, loops, method tables) see **[FLOW.md](FLOW.md)**; for developer notes see **[usage.md](usage.md)**.

---

## Features

- **Register** — validated name / email / phone / CNIC / password (CNIC, email, phone checks included), auto-created account with a generated account number, choosable account type (`Current / Saving / Business`)
- **Login** — email + password verified against a libsodium hash; failed attempts loop back to the auth menu
- **Session management** — after login a `Session` (customer + account) is owned by `Core`; Logout returns to the auth menu, Exit closes the app
- **Dashboard** — menu loop with balance view, deposit (preset + custom amounts), and stubs for withdraw / transfer / cards / loans / history
- **Persistence** — raw SQLite3 C API (no ORM): `customers`, `accounts`, `transactions` tables with foreign keys

> Current status: register, login, dashboard, session flow and deposit are implemented; withdraw, transfer, transaction history, cards and loans are next up (see [`src/core/dashboard.cpp`](src/core/dashboard.cpp)).

---

## Tech Stack

| Piece | Choice |
| :--- | :--- |
| Language | C++23 (`std::expected`, `std::print`) |
| Build | CMake ≥ 3.20, presets |
| Database | SQLite3 (raw C API, no ORM) |
| Crypto / IDs | libsodium (Argon2-style password hashing, random IDs) |

---

## Project Layout

```
Bank App/
├── main.cpp                  # entry point: banner + SQLite/sodium init + Core::run()
├── CMakeLists.txt            # build config (warnings-as-noticed, globbed sources)
├── CMakePresets.json         # configure/build presets
├── FLOW.md                   # complete design doc: flow, loops, methods
├── usage.md                  # developer usage notes
├── includes/                 # headers
│   ├── core/                 #   Core, Auth, Dashboard, Session, db
│   ├── models/               #   Customer, Account, Transaction, Card structs
│   ├── constants/            #   shared constants
│   └── utils/                #   banner, validators, sodium helpers
├── src/                      # implementations (mirrors includes/)
└── data/bank.db              # local SQLite database (git-ignored)
```

---

## Build & Run

Requirements: GCC/Clang with C++23 support, CMake ≥ 3.20, sqlite3, libsodium, pkg-config.

```bash
# configure (uses the preset: GCC, Debug, out/build/<preset>)
cmake --preset "GCC 16.2.1 x86_64-pc-linux-gnu"

# build
cmake --build --preset "GCC 16.2.1 x86_64-pc-linux-gnu"

# run
./out/build/GCC\ 16.2.1\ x86_64-pc-linux-gnu/BankApp
```

The database is created automatically at `data/bank.db` on first run (path can be overridden via the `BANKAPP_DB_PATH` compile definition).

---

## How the Flow Works

```
main()
 └─ Core::run()                       # outer loop — owns the session
     ├─ [no session] → Auth::run()    # login / register → creates Session
     └─ [session set] → Dashboard::run(session)
          ├─ 1..7  → service handlers (session by reference)
          ├─ 8     → Logout  → session.reset(), back to Auth
          └─ 0     → Exit    → app closes
```

Every deposit updates the account row in SQLite and mirrors the new balance into the in-memory session, so the dashboard always shows the live state.

---

## Roadmap

- [x] Project structure + CMake build
- [x] Register with validation, login with password hashing
- [x] Session-based Core ↔ Auth ↔ Dashboard state machine
- [x] Deposit (preset amounts + custom)
- [x] `transactions` table + model (`TransactionTable`)
- [ ] Withdraw, Transfer (both write a transaction record)
- [ ] Transaction history (option 7, `find_by_account`)
- [ ] Card services (option 5)
- [ ] Loan services (option 6)
