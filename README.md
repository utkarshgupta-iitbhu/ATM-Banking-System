# 🏦 ATM Banking System (C++)

![ATM](https://img.shields.io/badge/Project-ATM%20Simulator-yellow)
![C++](https://img.shields.io/badge/Language-C++17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-informational)
![File Handling](https://img.shields.io/badge/Storage-Binary%20Files-orange)

A **console-based ATM / Banking System** built in **C++** that demonstrates real-world concepts like **binary file handling**, **object serialization**, **secure PIN input**, **admin & user roles**, and **transaction logging** — all wrapped in a clean, interactive UI with sound feedback.

---

## 📌 Key Features

### 👤 User Features

* Secure login using **4-digit masked PIN (`****`)**
* Create a new bank account
* Deposit & withdraw money
* View transaction history with timestamps
* Close/Delete account permanently

### 🛡️ Admin Features

* Special **admin login** (ID: `1000`, PIN: `7777`)
* View **all accounts** in the system
* See total number of accounts
* See total money stored in the bank

### 🔊 UI & UX Enhancements

* Real ATM-like **beep sounds** on key actions
* Clean, aligned console UI
* Error sounds for invalid actions
* Confirmation prompts before destructive operations

---

## 🖥️ Console UI Screenshots

### Main Menu

```text
=========================================
         WELCOME TO UG BANK ATM
=========================================
1. Login
2. Create Account
3. Exit
-----------------------------------------
Select:
```

### Create Account Screen

```text
====================================
         CREATE NEW ACCOUNT
====================================
Enter Name      : John Doe
Set 4-digit PIN : ****
Confirm PIN     : ****
Initial Deposit : $5000
------------------------------------
Account ID      : 1001
------------------------------------
Account created successfully!
```

### User Dashboard

```text
===============================================
            WELCOME John Doe
===============================================
Account ID: 1001
-----------------------------------------------
1. Check Balance
2. Deposit
3. Withdraw
4. Transaction History
5. Logout
6. Close Account
-----------------------------------------------
Select:
```

### Admin Dashboard

```text
===========ADMIN DASHBOARD===========
-------------------------------------
| Acc ID |     NAME     |  BALANCE  |
-------------------------------------
| 1001   | John Doe     | $5000.00  |
-------------------------------------
Total Accounts: 1
Total Balance : $5000.00
-------------------------------------
```

---

## 🧱 Architecture Overview

This project follows a **modular & layered design**:

* **Data Layer** → Binary `.dat` files
* **Business Logic** → Account & Transaction classes
* **Presentation Layer** → Console UI + sound feedback

---

## 📂 Files Created at Runtime

> These files are **NOT committed to GitHub** (ignored via `.gitignore`)

| File               | Purpose                                  |
| ------------------ | ---------------------------------------- |
| `accounts.dat`     | Stores all bank account records (binary) |
| `transactions.dat` | Stores all transaction logs (binary)     |

The program **automatically creates these files** when needed.

---

## 🔍 Preprocessor Directives Explained

```cpp
#include <iostream>   // Console input/output
#include <fstream>    // File handling (ifstream, ofstream, fstream)
#include <cstring>    // strcpy, strncpy
#include <iomanip>    // setw, setprecision, formatting
#include <ctime>      // time(), time_t, ctime()
#include <windows.h>  // Beep(), Sleep(), Windows-specific APIs
#include <conio.h>    // _getch() for masked PIN input
```

> ⚠️ Note: This project is **Windows-only** due to `windows.h` and `conio.h`.

---

## 🧩 Core Data Structures

### 📦 `struct Transaction`

Stores a **single transaction record**:

```cpp
struct Transaction {
    long accountID;
    double amount;
    char type[10];   // OPEN, DEPOSIT, WITHDRAW
    time_t date;     // Timestamp
};
```

* Stored in `transactions.dat`
* Uses `time_t` to log exact transaction time
* Serialized directly into binary file

---

### 🏦 `class Account`

Represents a **bank account**:

```cpp
class Account {
    long accID;
    char name[50];
    int pin;
    double balance;
};
```

#### Responsibilities:

* PIN verification
* Deposit & withdrawal logic
* Binary file serialization (`saveToFile`, `readFromFile`)

> 💡 Entire object is written to disk using `reinterpret_cast`, simulating **low-level binary persistence**.

---

## 🔐 Secure PIN Input (`inputPIN()`)

### Why not `cin`?

* `cin` shows PIN on screen ❌
* ATM systems must **mask PIN input**

### How it works:

```cpp
char ch = _getch();  // Reads key without echo
```

* Digits → prints `*`
* Backspace → deletes last digit
* Enter → accepts only if exactly 4 digits entered
* Beep sound on each key press

✔️ Fully supports **backspace**
✔️ Mimics real ATM behavior

---

## 💾 File Input / Output Explained

### Writing Objects

```cpp
out.write(reinterpret_cast<const char*>(this), sizeof(*this));
```

* Converts object memory → raw bytes
* Writes entire object at once

### Reading Objects

```cpp
in.read(reinterpret_cast<char*>(this), sizeof(*this));
```

* Reads raw bytes back into object

> Interviewers often ask about **binary serialization** — this project demonstrates it clearly.

---

## 🔁 Account Number Generation

```cpp
long getnextAccountID();
```

Logic:

1. Open `accounts.dat`
2. Jump to **last record** using `seekg`
3. Read last account ID
4. Return `lastID + 1`

✔️ Ensures **unique auto-incremented IDs**

---

## 🗑️ Delete / Close Account Logic

### Why deletion is tricky

You **cannot delete part of a binary file directly**.

### Solution Used (Industry Pattern):

1. Create `temp.dat`
2. Copy all accounts **except target account**
3. Delete original `accounts.dat`
4. Rename `temp.dat` → `accounts.dat`

Same logic applies to deleting transactions from `transactions.dat`.

✔️ Safe
✔️ Reliable

---

## 👨‍💼 Admin Module Explained

Hardcoded admin credentials:

```cpp
ADMIN_ID  = 1000
ADMIN_PIN = 7777
```

Admin can:

* View all accounts
* See total balance in bank
* See number of active users

> Demonstrates **role-based access control**.

---

## 🎨 Console UI Design

* Fixed-width tables using `setw`
* Consistent separators
* Clear section headers
* Clean navigation menus

```cpp
cout << fixed << setprecision(2);
```

Ensures all money values show **2 decimal places**.

---

## 🔊 Sound Design (ATM-like)

Uses Windows `Beep(frequency, duration)`:

| Action       | Frequency | Duration |
| ------------ | --------- | -------- |
| Button press | 800 Hz    | 60 ms    |
| Success      | 1000 Hz   | 100 ms   |
| Error        | 400 Hz    | 150 ms   |

Adds **realistic ATM feedback**.

---

## 🚀 How to Run

```bash
g++ atm.cpp -o atm.exe
atm
```

> Tested using **MinGW on Windows**

---

## 📌 Tech Stack 🛠️

**Language:** C++
**Libraries:** STL, Windows API, Conio

---

## 👤 Author

**Utkarsh Gupta**  
Aspiring Software Engineer | C++ Enthusiast

![ATM](https://img.shields.io/badge/Project-ATM%20Simulator-yellow)
![C++](https://img.shields.io/badge/Language-C++17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-informational)
![File Handling](https://img.shields.io/badge/Storage-Binary%20Files-orange)
