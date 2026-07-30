 # ATM System — C++ Console App

A console-based ATM (Automated Teller Machine) simulation written in C++, allowing bank clients to authenticate with their account number and PIN, then perform transactions — withdrawals, deposits, and balance checks — against persistent file storage. Built on top of [MyLib](../../07-Algorithms&ProblemSolvingLevel3/MyLibrary) and its `MyBank` module, sharing the same client data model and file format as the [Bank Management System](../BankProjectThirdProject).

## Features

**Client Authentication**
- Login via Account Number + PIN, matched against `Clients.txt` (case-insensitive, whitespace-trimmed).
- Loops until valid credentials are entered — no fixed retry limit.

**Transactions**
- **Quick Withdraw** — pick a preset amount (20, 50, 100, 200, 400, 600, 800, 1000) from a menu, or exit without withdrawing.
- **Normal Withdraw** — enter any custom amount, as long as it's a multiple of 5.
- **Deposit** — enter any positive amount to add to the balance.
- **Check Balance** — view the current account balance.
- All transactions are blocked if the withdrawal amount exceeds the current balance ("Insufficient balance").
- Every successful transaction is persisted immediately to `Clients.txt`.

**Navigation**
- Returns to the ATM Main Menu automatically after each operation.
- Logout returns to the Login screen without closing the program.

## Dependency: MyLib / MyBank

This project depends on two things from [MyLib](../../07-Algorithms&ProblemSolvingLevel3/MyLibrary):

- **`MyLib`** (general utilities) — `MyLib::AllLettersToUpper`, `MyLib::TrimSpaces`, `MyLib::Read_num_in_range`, `MyLib::PauseAndClearScreen`.
- **`MyBank`** (a module shared with the Bank Management System) — `stClient` struct, `LoadClientsDataFromFile`, `SearchClientInVector`, `DepositToAccount`, `PrintClientRecord`.
- **`MyIO`** — `ReadString`, `ReadPositiveNumber`, `ReadPositiveDoubleNumber`.

Reusing `MyBank::stClient` (via `using stClient = MyBank::stClient;`) means the ATM and the Bank Management System operate on the exact same client records and file format — a deposit made through the ATM is immediately visible to the Bank Management System and vice versa.

Make sure `libMyLib.a` is built first — see MyLib's own README for build instructions.

## How to build and run

```bash
g++ CPP-ATM-System.cpp -I/path/to/MyLibrary -L/path/to/MyLibrary -lMyLib -o atm_system.out
./atm_system.out
```

(Replace `/path/to/MyLibrary` with the actual path — see MyLib's README for an environment-variable shortcut.)

## How it works

1. **Login Screen** — reads Account Number and PIN, authenticates against `Clients.txt` via `AuthenticateClient`; re-prompts on failure.
2. **ATM Main Menu** — `RunTheChoice` routes to one of five screens based on the selected option.
3. Each transaction screen (`ShowQuickWithdrawScreen`, `ShowNormalWithdrawScreen`, `ShowDepositScreen`, `ShowBalanceScreen`) reads any needed input, then hands off to a shared transaction pipeline.
4. After any transaction screen finishes, control returns to the Main Menu automatically (`GoToMainMenue`, with a pause so the result stays visible).
5. **Logout** re-invokes `LoginScreen`, restarting the authentication loop for a new session — the program itself keeps running.

### Shared transaction pipeline

Both withdrawals and deposits funnel through the same two functions, differing only in a `bool IsWithdraw` flag:

```
ShowQuickWithdrawScreen ─┐
ShowNormalWithdrawScreen ─┼─► ProcessTransaction ─► HandleAccountTransaction ─► MyBank::DepositToAccount
ShowDepositScreen ───────┘
```

- `ProcessTransaction` re-loads the client list from file, re-locates the current client by account number (guards against stale in-memory data), and skips the operation entirely if the client isn't found or the amount is `0`.
- `HandleAccountTransaction` checks `CanWithdrawAmount` for withdrawals (blocks if it would exceed the balance), then applies the amount as negative (withdraw) or positive (deposit) through `MyBank::DepositToAccount`, and syncs the in-memory `Client` with the updated record.

### Data model (from `MyBank`)

```cpp
struct stClient {
    string NbrAcount, PinCode, FullName, NbrPhone;
    double AccountBalance;
    bool MarkForDelete = false;
};
```

### Code structure

| Section | Responsibility |
|---|---|
| Enums | `enAtmMenuOptions`, `enQuickWithdrawOptions` |
| Authentication | `AuthenticateClient`, `IsClientAuthorized`, `LoginScreen` |
| Menu & navigation | `ShowAtmMainMenu`, `GetMenuChoice`, `RunTheChoice`, `GoToMainMenue` |
| Check Balance | `ShowBalanceScreen` |
| Quick Withdraw | `ShowQuickWithdrawScreen`, `GetQuickWithdrawChoice`, `GetAmountFromChoice` |
| Normal Withdraw | `ShowNormalWithdrawScreen`, `GetAmountMultupleOf5` |
| Deposit | `ShowDepositScreen` |
| Shared transaction logic | `CanWithdrawAmount`, `ProcessTransaction`, `HandleAccountTransaction`, `Withdraw`, `Deposit` |

## Requirements

- A C++ compiler supporting C++11 or later.
- The compiled `MyLib` static library (`libMyLib.a`), including its `MyBank` module, available on the include/library path.
- A terminal that supports `clear` (Linux/macOS, or WSL on Windows).
- A `Clients.txt` file with at least one existing client record (shared format with the Bank Management System).

## Known limitations / notes

- There's no confirmation prompt ("Are you sure? y/n") before a transaction is committed — every valid amount is applied immediately once entered.
- A withdrawal that exceeds the balance prints "Insufficient balance" and returns straight to the Main Menu — it does **not** re-prompt on the same screen for another amount.
- No session timeout — a logged-in client stays authenticated indefinitely until they explicitly choose Logout.
- No PIN masking — the PIN is echoed to the terminal as plain text while typing.

## Possible improvements

- Add a confirmation step ("Are you sure you want to perform this transaction? y/n") before committing any withdrawal or deposit.
- Retry the current transaction screen on insufficient balance instead of returning to the Main Menu (via a loop, not recursion).
- Add a per-client transaction history/log instead of only the running balance.
- Mask PIN input on the login screen.

## About

This project shares its data layer with the [Bank Management System](../BankProjectThirdProject) through the common `MyBank` module — it's a practical example of the same client records being safely accessed from two independent front-ends (staff-facing bank management vs. self-service ATM) built on one shared, well-tested library.
