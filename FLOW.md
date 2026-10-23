# Bank App — Complete Flow, Loops & Methods

Yeh document project ka pura control flow, loops aur methods explain karta hai — entry point se le kar exit tak. Jahan code maujood hai wahan `[current]`, jahan design/plan hai wahan `[planned]` likha hai.

---

## 1. High-Level Architecture

```
┌──────────────────────────────────────────────────────┐
│                       main()                         │  ← entry point
│  banner print  +  Core object  +  Core::run()        │
└──────────────────────────┬───────────────────────────┘
                           │
┌──────────────────────────▼───────────────────────────┐
│                       Core                           │  ← application controller
│  outer loop: session + login state ka malik          │
└──────────────────────────┬───────────────────────────┘
                           │
        ┌──────────────────┴──────────────────┐
        │                                     │
┌───────▼────────┐                   ┌────────▼─────────┐
│      Auth      │                   │   Dashboard /    │
│ login/register │                   │   Menu (services)│
│  auth loop     │                   │   menu loop      │
└───────┬────────┘                   └────────┬─────────┘
        │                                     │
        └───────────────┬─────────────────────┘
                        │
              ┌─────────▼──────────┐
              │   Data layer       │
              │   SQLite (no ORM)  │  [planned]
              └────────────────────┘
```

**Responsibilities**

| Layer | File | Kaam |
| :--- | :--- | :--- |
| Entry | `main.cpp` | Setup + kick off |
| Controller | `includes/core/core.hpp`, `src/core/core.cpp` | App state, outer loop |
| Auth | `includes/core/auth.hpp`, `src/core/auth.cpp` | Login / Register / session |
| Menu | `src/core/menu.cpp` | Dashboard loop, operations dispatch |
| Banner | `includes/utils/banner.hpp`, `src/utils/banner.cpp` | ASCII banner |
| Validators | `includes/utils/utils.hpp` | Email / Phone / CNIC validation |
| Models | `includes/models/*.hpp` | `Customer`, `Account`, `Card` (plain structs) |
| Data | `[planned]` SQLite | CRUD, transactions |

---

## 2. Complete Call Flow (Sequence)

```
main()
 ├─ BannerPrinter printer
 ├─ printer.print_styled_banner("Cyrixon Bank!")
 └─ Core c
     └─ Core::run()                         ── outer loop (while running)
         │
         ├─[not logged in]──> Auth::authenticate()   ── auth loop (while not authenticated)
         │                      ├─ show_auth_menu()
         │                      ├─ Auth::login()      -> std::expected<Customer, ValidationError>
         │                      └─ Auth::_register()  -> std::expected<Customer, ValidationError>
         │
         └─[logged in]───────> Dashboard::run(session) ── menu loop (while session_active)
                                ├─ show_main_menu()
                                ├─ read choice
                                ├─ switch → service handler
                                ├─ logout → wapas Auth loop
                                └─ exit   → bahar, running = false
```

---

## 3. Entry Point — `main()`

```cpp
// main.cpp
int main()
{
    BannerPrinter printer;
    printer.print_styled_banner("Cyrixon Bank!");

    Core c;
    return 0;      // [planned] → c.run();
}
```

**Methods**

| Method | Class | Existing | Kaam |
| :--- | :--- | :---: | :--- |
| `BannerPrinter::print_styled_banner(const std::string&)` | `BannerPrinter` | ✅ current | 5-line ASCII banner print |
| `Core::Core()` | `Core` | ✅ current | Construct (abhi khali) |
| `Core::run()` | `Core` | ⏳ planned | Pura app loop start kare |

---

## 4. Core — Outer Loop

`Core` app state ka malik hai. Isme session (`logged_in`, current `Customer`) rakha jayega aur do phase chalenge: **auth** aur **dashboard**.

```cpp
// core.hpp
class Core
{
private:
    Auth auth;
    std::optional<Customer> session;   // [planned] login hone par set
    bool running = true;               // [planned]

public:
    Core();
    ~Core();
    void run();                        // [planned] outer loop
};
```

**Outer loop pseudocode**

```
Core::run():
    while (running):
        if (not session):
            auto user = auth.authenticate()      # Auth phase
            if (user)  session = *user           # login/register success
            else       running = false           # user exit (0)
        else:
            auto action = dashboard.run(*session) # Dashboard phase
            switch action:
                Logout -> session.reset()         # wapas Auth
                Exit   -> running = false         # pura app band
```

**Methods**

| Method | Existing | Return | Kaam |
| :--- | :--- | :--- | :--- |
| `Core::Core()` | ✅ | — | Auth etc. init |
| `Core::run()` | ⏳ planned | `void` | Outer loop drive kare |
| `Core::is_logged_in()` | ⏳ planned | `bool` | State check |
| `Core::logout()` | ⏳ planned | `void` | Session clear |

---

## 5. Auth — Authentication & Auth Loop

Auth do cheezein deta hai: **Login** aur **Register**. Dono `std::expected<Customer, ValidationError>` return karenge (better error handling).

### 5.1 Auth loop

Abhi `Auth` constructor khud hi sab kuch kar deta hai (`auth.cpp:6`) jo galat jagah hai. Isko `authenticate()` method me le jana chahiye:

```
Auth::authenticate()  ->  std::expected<Customer, ValidationError>
    while (true):                              # auth loop
        show_auth_menu()                       # 1) Login  2) Register  0) Exit
        choice = read_choice()
        switch choice:
            1 -> result = login();    if (result) return result
            2 -> result = _register();if (result) return result
            0 -> return unexpected(Exit)       # [planned] Exit error/code
            _ -> print "Invalid choice"; continue
```

### 5.2 Methods (current + planned)

| Method | Signature | Existing | Kaam |
| :--- | :--- | :---: | :--- |
| `Auth::Auth()` | `Auth()` | ✅ | Abhi menu + dispatch (isay `authenticate()` me move karna hai) |
| `Auth::login()` | `void login()` → ⏳ `std::expected<Customer, ValidationError>` | ✅ | Email + password phir DB verify |
| `Auth::_register()` | `void _register()` → ⏳ expected | ✅ | Register flow + validation |
| `Auth::collectRegistrationData()` | `std::expected<Register, ValidationError>` | ✅ | Inputs collect + rules check |
| `Auth::show_auth_menu()` | `void` | ⏳ planned | Menu print |
| `Auth::authenticate()` | `std::expected<Customer, ValidationError>` | ⏳ planned | Auth loop gate |
| `Auth::verify_credentials(email, password)` | `std::expected<Customer, ValidationError>` | ⏳ planned | SQLite se match |
| `Auth::hash_password(pw)` | `std::string` | ⏳ planned | Password hashing |

### 5.3 Registration validation flow (`collectRegistrationData`, current)

`auth.cpp:41` par jo rules lagte hain, order yeh hai:

```
collect inputs (name, email, phone, cnic, address, password, cpassword)
  │
  ├─ name/password empty?      -> unexpected(EmptyFields)
  ├─ password != cpassword?    -> unexpected(PasswordMismatch)
  ├─ !isValidEmail(email)?     -> unexpected(InvalidEmail)
  ├─ !isValidPhone(phone)?     -> unexpected(InvalidPhone)
  ├─ !isValidCNIC(cnic)?       -> unexpected(InvalidCNIC)
  └─ sab theek                 -> return reg
```

**Validation helpers** (`includes/utils/utils.hpp`)

| Function | Regex kaam |
| :--- | :--- |
| `isValidEmail(email)` | `user@example.com` |
| `isValidPhone(phone)` | `03001234567` / `+923001234567` / `0321-1234567` |
| `isValidCNIC(cnic)` | `35201-1234567-1` |
| `errorToString(err)` | `ValidationError` → readable message |

### 5.4 Login loop (planned)

```
Auth::login() -> expected<Customer, ValidationError>:
    for attempt in 1..MAX_ATTEMPTS:            # login retry loop
        email    = read("Email")
        password = read("Password")
        auto user = verify_credentials(email, password)   # SQLite
        if (user) return *user
        print "Invalid credentials, try again"
    return unexpected(InvalidCredentials)
```

---

## 6. Menu / Dashboard — Inner Loop

Yeh app ka sabse active loop hai (`src/core/menu.cpp`, filhaal khali `[planned]`).

```
Dashboard::run(session):
    while (session_active):                    # menu loop
        show_main_menu()
        choice = read_choice()
        switch choice:
            1 -> show_balance(session)
            2 -> deposit(session)
            3 -> withdraw(session)
            4 -> transfer(session)
            5 -> list_transactions(session)
            6 -> manage_cards(session)         # sub-menu (nested loop)
            7 -> manage_loans(session)         # sub-menu (nested loop)
            8 -> logout()  -> return Action::Logout
            0 -> return Action::Exit
            _ -> print "Invalid choice"; continue
```

**Main menu (intended)**

```
===== Dashboard =====
 1) Balance
 2) Deposit
 3) Withdraw
 4) Transfer
 5) Transaction History
 6) Cards
 7) Loans
 8) Logout
 0) Exit
```

**Methods (planned)**

| Method | Return | Kaam |
| :--- | :--- | :--- |
| `Dashboard::run(session)` | `Action` (Logout/Exit) | Menu loop drive |
| `Dashboard::show_main_menu()` | `void` | Options print |
| `Dashboard::show_balance(session)` | `void` | Balance display |
| `Dashboard::deposit(session)` | `void` | Amount add + record txn |
| `Dashboard::withdraw(session)` | `void` | Balance check + deduct |
| `Dashboard::transfer(session)` | `void` | Do accounts ke darmiyan |
| `Dashboard::list_transactions(session)` | `void` | History print |
| `Dashboard::manage_cards(session)` | `void` | Nested cards loop |
| `Dashboard::manage_loans(session)` | `void` | Nested loans loop |

**Nested sub-menu pattern** (cards/loans ke liye):

```
manage_cards(session):
    while (in_cards_menu):
        show_cards_menu()                      # 1) List 2) Add 3) Block 0) Back
        choice = read_choice()
        switch choice:
            0 -> in_cards_menu = false         # wapas main menu
            ...
```

---

## 7. Loops Summary

| Loop | Jagah | Condition | Exit kab |
| :--- | :--- | :--- | :--- |
| **Outer loop** | `Core::run()` | `while (running)` | User "Exit" (0) |
| **Auth loop** | `Auth::authenticate()` | `while (not authenticated)` | Login/Register success ya Exit |
| **Login retry loop** | `Auth::login()` | `for attempt < MAX` | Valid credentials ya attempts khatam |
| **Menu loop** | `Dashboard::run()` | `while (session_active)` | Logout (8) ya Exit (0) |
| **Sub-menu loop** | `manage_cards/loans` | `while (in_submenu)` | "Back" (0) |

Loop nesting: `Outer → (Auth | Menu → Sub-menu)`. Logout menu loop se nikalta hai par outer loop chalta rehta hai (wapas Auth).

---

## 8. State & Session

```cpp
struct Session        // [planned]
{
    std::string customer_id;
    std::string name;
    std::string email;
    bool active = true;
};
```

- **Session** login ke baad set, logout par clear.
- **`running`** (Core) — pura app chalu hai ya nahi.
- **`session_active`** (Dashboard) — current menu loop ka flag.
- Models (`Customer`, `Account`, `Card`) plain structs hain (SQLite rows ke liye).

---

## 9. Error Handling Pattern

Project `std::expected` use karta hai (`auth.hpp:14`, `collectRegistrationData`):

```cpp
std::expected<T, ValidationError> result = do_something();
if (!result)
    std::println(std::cerr, "{}", errorToString(result.error()));
else
    use(*result);
```

`ValidationError` values: `EmptyFields`, `PasswordMismatch`, `InvalidEmail`, `InvalidPhone`, `InvalidCNIC` (+ planned: `InvalidCredentials`, `AccountNotFound`, `InsufficientFunds`).

**Input rule:** `std::cin >> choice` ke baad ya `std::getline` ke pehle `std::cin.ignore(numeric_limits<streamsize>::max(), '\n')` zaroor karein, warna newline skip issue hota hai.

---

## 10. Data Layer (SQLite, no ORM) `[planned]`

```
Auth / Dashboard handlers
      │
      ▼
Data access (raw sqlite3 C API)
  ├─ db.open(path)                -> sqlite3_open_v2
  ├─ db.exec(sql)                 -> schema/migrations
  ├─ stmt.prepare(sql)            -> sqlite3_prepare_v2
  ├─ stmt.bind(i, value)          -> sqlite3_bind_*
  ├─ stmt.step()                  -> SQLITE_ROW / SQLITE_DONE
  └─ stmt.finalize()              -> sqlite3_finalize
```

Tables (planned): `customers`, `accounts`, `cards`, `loans`, `transactions`.
Har write (deposit/withdraw/transfer) ek SQLite **transaction** me hoga.

---

## 11. Full Pseudocode (sab ek jagah)

```
main():
    print_banner("Cyrixon Bank!")
    Core().run()

Core::run():
    while running:
        if not session:
            r = auth.authenticate()
            session = r ? r.value : exit  → running = false
        else:
            switch dashboard.run(session):
                Logout -> session = nullopt
                Exit   -> running = false

Auth::authenticate():
    while true:
        choice = show_auth_menu_and_read()
        if choice == 1 and (u = login()):          return u
        if choice == 2 and (u = _register()):      return u
        if choice == 0:                            return Exit

Auth::_register():
    r = collectRegistrationData()
    if not r: print(errorToString(r.error())); return unexpected
    return persist(r.value)      # SQLite insert + return Customer

Dashboard::run(session):
    while session.active:
        choice = show_main_menu_and_read()
        switch choice:
            1..7 -> handler(session)
            8 -> return Logout
            0 -> return Exit
```

---

## 12. Status Checklist

| Cheez | Status |
| :--- | :--- |
| `main` + banner | ✅ current |
| `Auth` register + validation | ✅ current |
| `Auth` logic constructor me | ⚠️ refactor → `authenticate()` loop |
| `Core::run()` outer loop | ⏳ planned |
| `Dashboard` menu loop (`menu.cpp` empty) | ⏳ planned |
| Session/state | ⏳ planned |
| SQLite data layer | ⏳ planned |
| Login retry loop + credential verify | ⏳ planned |
