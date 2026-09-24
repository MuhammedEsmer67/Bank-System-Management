# Bank-System-Management

A console-based **Bank Management System** written in C++ with full Object-Oriented design. It models the day-to-day operations of a bank branch: user accounts with role-based logins, client accounts with balances, transfers between clients, and multi-currency conversion — all persisted to plain text files, no database required.

![C++](https://img.shields.io/badge/C%2B%2B-OOP-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-informational)
![IDE](https://img.shields.io/badge/IDE-Visual%20Studio%202022-purple)

---

## Overview

A logged-in User navigates a permission-gated menu of panels — managing Clients (the bank's customers), managing other Users, running transfers, and handling currency exchange — where each panel is only accessible if that User's role grants it. Every screen in the program — from the login screen to the currency calculator — is its own class, and every one of those classes shares the same header/footer/permission-check logic through a single base class, so the whole UI is consistent and easy to extend.

There is no database: every entity (`clsUser`, `clsClient`, `clsCurrency`) persists itself to its own flat text file (`Users.txt`, `Clients.txt`, `Currencies.txt`), with each record on one line and fields joined by a custom delimiter (`#//#`). Reading a file means splitting each line back into an object; writing means re-serializing every object in the vector back to lines. This "load whole file → vector → operate → save whole file" pattern repeats across the project and is the core idea to understand before reading any single class.

## Key Design Ideas

### 1. Object lifecycle via an internal `enMode`
Every persisted class (`clsUser`, `clsClient`, `clsCurrency`) carries a private `enMode` enum (e.g. `EmptyUser`, `UpdateUser`, `AddNewUser`) set in the constructor. The object itself knows *why* it exists — was it just constructed for a brand-new record, loaded from disk for editing, or returned as a "not found" placeholder? `Save()` then does a `switch` on that mode instead of the caller having to pass a separate "insert or update" flag:

- `AddNewUser`/`AddNewClient` → checks the file for a duplicate key first, appends a new line (`ios::app`), then flips its own mode to `Update...` so a second `Save()` call updates instead of re-inserting.
- `Update...` (default) → reloads the whole vector, finds the matching record by key (username / account number / country), overwrites it with `*this`, and rewrites the file.
- `Empty...` → refuses to save at all.

### 2. "Null Object" instead of null pointers
`Find()` never returns `nullptr` and never throws. If nothing matches, it returns a dedicated *empty* object (`_GetEmptyUserObject()`, `_GetEmptyClientObject()`, `_GetEmptyObject()`) whose mode is `Empty...`. Callers check `isEmpty()` afterwards. This keeps the calling code simple (no null checks, no exceptions) and is what makes `GlobalUser.h` safe at startup — see below.

### 3. Soft delete, then physical rewrite (`MarkToDelete`)
`DeleteUser()` / `DeleteClient()` don't touch the file directly. They load every record into memory, flip a private `_MarkToDelete` flag on the matching one, and call the normal save routine — which, when it rewrites the file, simply skips any record flagged for deletion. The delete is logically instant in memory, and the file on disk is only ever fully rewritten once, the same way an update is. It's the same save pipeline reused for a different purpose instead of a separate delete code path.

### 4. `clsPerson` as the shared identity base
`clsUser` and `clsClient` both publicly inherit from `clsPerson`, which owns first name, last name, email, and phone number. A **User** is a person plus login credentials and permissions; a **Client** is a person plus an account number, PIN, and balance. Shared fields live once, in one place, instead of being copy-pasted into both classes.

### 5. `GlobalUser.h` — the current session
```cpp
static clsUser CurrentUser = clsUser::Find("");
```
At program startup there's no logged-in user yet, so this line intentionally searches for a username that can't exist, which safely returns an *empty* `clsUser` rather than crashing. `CurrentUser` is a single global instance shared across the whole program: once login succeeds, it's reassigned to the real, matched user, and from that point on every screen reads `CurrentUser` to know who's logged in, what they're allowed to do, and whose username to stamp on log entries (transfer logs, login history, etc.), and it's reassigned to *empty* `clsUser` when logging out.

### 6. Role-based permissions via bitmasking
`clsUser::enPermissions` defines each capability as its own bit (`ListClients = 1`, `AddNewClient = 2`, `DeleteClient = 4`, `UpdateClient = 8`, …, `All = -1`). A user's `_Permissions` field is just the bitwise-OR of whatever they're allowed to do. Checking access is then a single check:
```cpp
bool isPermissionAccessible(enPermissions Permission)
{
    if (_Permissions == enPermissions::All) return true;
    return (Permission & _Permissions);
}
```
This lets one short field represent any combination of the nine permissions instead of nine separate boolean flags, and lets `clsScreen::_CheckAccessRights()` gate every screen with one reusable line.

### 7. `clsScreen` — one base class for every panel
Every screen class in the project (login, menus, add/find/update/delete for users, clients and currencies, transfers, deposits, etc.) inherits from `clsScreen`. It centralizes two protected static helpers:
- `_DrawScreenHeader(Title, SubTitle, Tabs)` — prints the boxed title, the logged-in user's name (from `CurrentUser`), and today's date, formatted the same way everywhere.
- `_CheckAccessRights(Permission, Title)` — checks `CurrentUser.isPermissionAccessible(...)` and prints a consistent "Access Denied" box if it fails.

Because every screen goes through this one class, changing the header layout or the denied-access message is a one-line edit in `clsScreen.h` that instantly applies to every panel in the program — the UI is modular by construction rather than by discipline.

### 8. Recursive menu/panel flow
Screens don't loop back to the main menu with `while` loops re-entering a function from the top; instead, after a panel finishes its action it calls back into its parent menu function, which can call back into a child screen, and so on. Navigating "back" simply means returning up that call chain. It keeps each screen's control flow self-contained, at the cost of the call stack growing with how deep a user navigates before going back.

### 9. Currency conversion through a common base (USD)
`clsCurrency` stores each currency's rate as *how much of it equals 1 USD*. Converting is always routed through USD instead of needing a rate for every possible currency pair:
```cpp
float ToUSD(float Amount) { return Amount / _CurrencyRate; }

float FromCurrencyToAnother(clsCurrency CurrencyTo, float Amount)
{
    float USDAmount = ToUSD(Amount);
    if (CurrencyTo.getCurrencyCode() == "USD") return USDAmount;
    return USDAmount * CurrencyTo.getCurrencyRate();
}
```
Any currency → USD is one division; USD → any currency is one multiplication; any → any is just those two steps chained. This avoids storing an O(n²) table of pairwise rates for n currencies.

---

## Features

- 🔐 **Login security** — accounts lock after 3 failed login attempts; passwords are stored encrypted (`clsUtility::EncryptText/DecryptText`), never in plain text.
- 📜 **Login history log** — every login attempt is appended to `LoginRegister.txt` with a timestamp, independent from the live user table.
- 🗑️ **Soft-delete (`MarkToDelete`)** — deletions are staged in memory and applied as a filtered rewrite, reusing the same save pipeline as every update.
- 💸 **Money transfers** — balance moves between two clients atomically in code (debit + credit), with every transfer appended to `TransferLog.txt`.
- 🌍 **Currency exchange** — list, find, and update currency rates, plus a USD-anchored converter between any two currencies.
- 🔑 **Bitwise role permissions** — nine distinct capabilities packed into a single `short`, checked with one bitwise comparison per screen.
- 🧭 **Recursive panel navigation** — screens call back into their parent menu on completion rather than looping, keeping each screen's flow self-contained.
- 🖥️ **One shared screen base (`clsScreen`)** — header, current user/date display, and access-denial messaging are defined once and reused by every panel in the system.
- 🧍 **Shared `clsPerson` base** — users and clients reuse the same name/email/phone fields and validation logic instead of duplicating them.
- 💾 **Flat-file persistence** — every entity round-trips through a delimiter-based (`#//#`) text format, so the whole system runs with zero external dependencies.

---

## Project Structure

```
📁 BankManagementSystem/
├── 📄 BankManagementSystem.sln
├── 📁 BankManagementSystem/
│   ├── 📁 Header Files/
│   │   ├── 📁 Core/
│   │   │   ├── clsClient.h
│   │   │   ├── clsCurrency.h
│   │   │   ├── clsPerson.h
│   │   │   └── clsUser.h
│   │   ├── 📁 Lib/
│   │   │   ├── clsChar.h
│   │   │   ├── clsDate.h
│   │   │   ├── clsInputValidation.h
│   │   │   ├── clsString.h
│   │   │   └── clsUtility.h
│   │   ├── 📁 Screens/
│   │   │   ├── clsScreen.h
│   │   │   ├── clsLoginScreen.h
│   │   │   ├── clsMainScreen.h
│   │   │   └── 📁 Main Menu/
│   │   │       ├── 📁 Currency Exchange/
│   │   │       │   ├── clsCurrencyCalculatorScreen.h
│   │   │       │   ├── clsCurrencyExchangeScreen.h
│   │   │       │   ├── clsFindCurrencyScreen.h
│   │   │       │   ├── clsListCurrencies.h
│   │   │       │   └── clsUpdateRateScreen.h
│   │   │       ├── 📁 Manage Clients/
│   │   │       │   ├── clsAddNewClientScreen.h
│   │   │       │   ├── clsClientListScreen.h
│   │   │       │   ├── clsDeleteClientScreen.h
│   │   │       │   ├── clsFindClientScreen.h
│   │   │       │   ├── clsLoginRegisterScreen.h
│   │   │       │   └── clsUpdateClientScreen.h
│   │   │       ├── 📁 Manage Users/
│   │   │       │   ├── clsAddNewUserScreen.h
│   │   │       │   ├── clsDeleteUserScreen.h
│   │   │       │   ├── clsFindUserScreen.h
│   │   │       │   ├── clsManageUsersScreen.h
│   │   │       │   ├── clsShowUserListScreen.h
│   │   │       │   └── clsUpdateUserScreen.h
│   │   │       └── 📁 Transactions/
│   │   │           ├── clsDepositScreen.h
│   │   │           ├── clsTotalBalancesScreen.h
│   │   │           ├── clsTransactionsScreen.h
│   │   │           ├── clsTransferLogScreen.h
│   │   │           ├── clsTransferScreen.h
│   │   │           └── clsWithdrawScreen.h
│   │   └── GlobalUser.h
│   └── 📁 Source Files/
│       └── BankManagementSystem.cpp
```

## Build and run

### Visual Studio 2022/2026 Community

1. Install Visual Studio Community with the **Desktop development with C++** workload.
2. Clone the repo: `git clone https://github.com/MuhammedEsmer67/Bank-System-Management.git`
3. Open the project and double-click the `.sln` file.
4. Press **Ctrl + F5** (Start Without Debugging). A console window shows the output.

### PowerShell (g++)

```
cd BankManagementSystem
g++ -std=c++17 BankManagementSystem.cpp -o BankSystem
.\BankSystem
```
