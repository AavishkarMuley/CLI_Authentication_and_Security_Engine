# CLI Authentication and Security Engine

A robust, security-first Command-Line Interface (CLI) authentication engine engineered from scratch in C++. This project goes beyond basic credential matching by implementing **cryptographic salting with DJB2 hashing**, **Two-Factor Authentication (2FA)**, **Brute-Force Rate Limiting**, and **safe atomic file transaction patterns** for account management.

---

## 📌 Project Overview

Most academic authentication projects store credentials in plain text, load sensitive records into volatile memory unnecessarily, or fail under race conditions during file updates. This project implements enterprise-grade defensive security patterns in a pure C++ CLI environment:

1. **Cryptographic Protection:** Neutralizes dictionary and Rainbow Table attacks using a dynamic 5-character randomized salt concatenated before one-way DJB2 hashing.
2. **Multi-Factor Security (2FA):** Implements a two-step verification workflow pairing secret credentials with a simulated One-Time Password (OTP) gateway.
3. **Brute-Force Defense:** Enforces a 3-attempt rate limiter backed by a mandatory 15-second cooldown lockout.
4. **Atomic File Manipulation:** Utilizes the temporary-file-swap pattern to update and delete user credentials on disk without risking file corruption or triggering Windows file-handle sharing violations.
5. **Interactive Console Shielding:** Keystroke interception via Windows `_getch()` protects passwords against shoulder-surfing attacks while maintaining full `Backspace` and `Enter` handling.

---

## 🏗️ System Architecture & Workflow

```
                        [ User Enters Credentials ]
                                     │
                                     ▼
                        ┌─────────────────────────┐
                        │   Inspect login.txt     │
                        └────────────┬────────────┘
                                     │
                 ┌───────────────────┴───────────────────┐
                 ▼                                       ▼
          [ User Exists ]                         [ New User ]
                 │                                       │
         Fetch User's Salt                       Password Strength
                 │                                Policy Validation
       Compute Salted Hash                               │
                 │                               Generate New Salt
                 │                               Compute DJB2 Hash
                 │                                       │
                 ▼                               Append to login.txt
       Verify with Stored Hash                           │
                 │                                       │
        ┌────────┴────────┐                              │
        ▼                 ▼                              │
   [ Mismatch ]        [ Match ]                         │
        │                 │                              │
Attempts Left > 0?        ▼                              │
  ├── Yes ──► Retry    Generate 4-Digit OTP              │
  └── No  ──► Lockout     (2FA Challenge)                │
      (15s Freeze)        │                              │
                          ▼                              │
                 OTP Verified (2FA Passed)               │
                          │                              │
                          └──────────────┬───────────────┘
                                         │
                                         ▼
                                 [ User Dashboard ]
                                         │
                 ┌───────────────────────┼───────────────────────┐
                 ▼                       ▼                       ▼
       [ 1. Change Password ]   [ 2. Delete Account ]      [ 3. Logout ]
          • Verify Old Creds       • Verify Password          • Clear Session
          • Generate New Salt      • Skip User in Temp Swap   • Terminate
          • Atomic File Swap       • Atomic File Swap
```

---

## 🔑 Core Features & Security Modules

### 1. Cryptographic Salting & DJB2 Hashing
* Uses Daniel J. Bernstein’s deterministic DJB2 bit-shift hash algorithm (`hash * 33 + c`).
* Every user account receives a uniquely generated 5-character alphanumeric/symbolic salt. Identical user passwords generate completely distinct hash digests.

### 2. Real-Time Password Policy Engine (`isstrongpass`)
Enforces a 5-tier security standard before allowing account registration or password updates:
* **Length:** $\ge 8$ characters
* **Uppercase:** At least one capital letter (`A-Z`)
* **Lowercase:** At least one lowercase letter (`a-z`)
* **Numeric:** At least one digit (`0-9`)
* **Special Character:** At least one non-alphanumeric symbol (`!@#$%^&*`)
* Provides immediate color-coded terminal feedback using ANSI escape sequences (`\033[32m` for passed rules, `\033[31m` for failed rules).

### 3. Two-Factor Authentication (2FA Simulator)
* Upon successful password verification, the system halts and generates a cryptographically pseudorandom 4-digit security code.
* Access to the user dashboard is strictly denied until the exact OTP challenge is solved.

### 4. Brute-Force Rate Limiting & Account Lockout
* Tracks failed authentication attempts within the active session.
* Users are granted **3 consecutive attempts**.
* Upon the 3rd failed attempt, the engine triggers an automatic security lockout, freezing execution for **15 seconds** via `Sleep(15000)` to defeat high-frequency automated credential stuffing scripts.

### 5. Atomic Account Management (Change & Delete)
* **Change Password:** Verifies current credentials, generates a fresh salt, re-hashes the new password, and safely swaps records using the Temp File Replacement Pattern.
* **Delete Account:** Requires confirmation and password validation before stripping the user record from persistent storage and purging the in-memory cache.
* **Windows File-Lock Handling:** Explicitly flushes and closes all open `ifstream`/`ofstream` handles before invoking `remove()` and `rename()` to prevent OS access violations (`EACCES`).

---

## 📊 Complexity Analysis

| Module | Operation | Underlying Structure | Time Complexity | Space Complexity |
| :--- | :--- | :--- | :--- | :--- |
| **Auth** | Salt Generation | Random Character Pool | $O(S)$ *(S = salt length)* | $O(1)$ |
| **Auth** | Password Hashing | Bit-Shift Loop (DJB2) | $O(L)$ *(L = password length)* | $O(1)$ |
| **Auth** | User Verification | Disk Stream Scan / Cache | $O(N)$ Disk / $O(1)$ RAM | $O(1)$ |
| **Security** | Policy Validation | Single-Pass Loop | $O(L)$ *(L = password length)* | $O(1)$ |
| **Engine** | Change Password | Stream Filter + Swap | $O(N)$ *(N = total records)* | $O(1)$ |
| **Engine** | Delete Account | Stream Filter + Swap | $O(N)$ *(N = total records)* | $O(1)$ |

---

## 📂 Database Schema (`login.txt`)

User credentials are saved in flat storage using pipe delimiters (`|`). Plain passwords are never written to disk:

```text
username|salt|hashed_password
```

### Sample Database Entries:
```text
aavishkar|7x#A2|18446744073709551614
henry|9z$K1|12938491823901928312
alice|mK9!0|98234819238471928341
```

---

## ⚙️ Compilation & Installation

### Prerequisites
* **Operating System:** Windows (relies on `<conio.h>` for `_getch()` and `<windows.h>` for console encoding/sleep APIs)
* **Compiler:** MinGW / GCC, Clang, or MSVC supporting **C++11 or higher**

### Build Commands:
```bash
# 1. Clone this repository
git clone https://github.com/your-username/cli-auth-security-engine.git

# 2. Navigate to project root
cd cli-auth-security-engine

# 3. Compile source code
g++ -std=c++11 main.cpp -o AuthEngine.exe

# 4. Launch executable
AuthEngine.exe
```

---

## 🖥️ Interactive CLI Demonstrations

### 1. New User Registration with Password Policy Validation
```text
====================================================
                Auth System
====================================================
Enter your Username: aavishkar
Enter your Password: ********

Account not found.
Create a new account
Enter a stronger password!

Password Strength Checklist:
❌ Length of password should be atleast 8.
❌ It should contain atleast one uppercase character.
✅ It should contain atleast one lowercase character.
✅ It should contain atleast one digit.
❌ It should contain atleast one special character.

[Re-enter meeting all criteria]
Account created successfully

====================================================
                Welcome aavishkar!
====================================================
```

### 2. Login, Rate Limiting & 2FA Challenge
```text
====================================================
                Auth System
====================================================
Enter your Username: aavishkar
Enter your Password: ****

Invalid login credentials.
2 attempt(s) left
Try entering password again: ********

User found
Your OTP for login is: 4892
Enter the otp: 4892
Correct OTP entered!
Loading your Account....

====================================================
                Welcome aavishkar!
====================================================

What do you want to do, aavishkar?
1 - Change password.
2 - Delete account.
3 - Logout.
```

### 3. Password Modification Workflow
```text
Enter choice: 1
Enter your current password: ********
User confirmed
Enter new password: *********
Enter new password again to confirm: *********

✅ Password matches.
Confirmation : Change password?(Y/N) y
✅ Password changed successfully.
```

---

## 🛡️ Key System Design Considerations

* **Shoulder-Surfing Defense:** Keystroke interception via Windows `_getch()` guarantees password characters are never echoed directly on screen.
* **File Stream Isolation:** Avoids concurrency locks by strictly ensuring stream objects (`ifstream`/`ofstream`) close prior to system-level disk deletion (`remove`) or renaming (`rename`).
* **State Synchronization:** In-memory hash maps (`unordered_map`) are updated synchronously with disk writes, ensuring parity between volatile cache and non-volatile storage.
