#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <ctime>
#include <windows.h>
#include <conio.h>

using namespace std;

const char *ACCOUNTS_FILE = "accounts.dat";
const char *TRANS_FILE = "transactions.dat";

const int ADMIN_ID = 1000;
const int ADMIN_PIN = 7777;

struct Transaction
{
    long accountID;
    double amount;
    char type[10]; //"WITHDRAW", "DEPOSIT", "OPEN"
    time_t date;

    Transaction() {}
    Transaction(long acc, double amt, const char *t)
    {
        accountID = acc;
        amount = amt;
        strncpy(type, t, 9);
        type[9] = '\0';
        date = time(0);
    }
};

class Account
{
private:
    long accID;
    char name[50];
    int pin;
    double balance;

public:
    Account()
    {
        accID = 0;
        strcpy(name, "Unknown");
        pin = 0;
        balance = 0.0;
    }
    Account(long id, const char *n, int p, double bal)
    {
        accID = id;
        strncpy(name, n, 49);
        name[49] = '\0';
        pin = p;
        balance = bal;
    }

    long getID() const { return accID; }
    bool verifyPin(int p) const { return pin == p; }
    double getBal() const { return balance; }
    const char *getName() const { return name; }

    void deposit(double amt)
    {
        if (amt <= 0)
        {
            Beep(400, 150);
            cout << "\tInvalid amount";
            return;
        }

        balance += amt;
    }

    bool withdraw(double amt)
    {
        if (amt <= balance)
        {
            balance -= amt;
            return true;
        }
        return false;
    }

    void savetoFile(ostream &out)
    {
        out.write(reinterpret_cast<const char *>(this), sizeof(*this));
    }
    void readfromFile(istream &in)
    {
        in.read(reinterpret_cast<char *>(this), sizeof(*this));
    }
};

int inputPIN()
{
    int pin = 0;
    char ch;
    int count = 0;

    while (true)
    {
        ch = _getch();
        if (ch >= '0' && ch <= '9')
        {
            if (count < 4)
            {
                Beep(800, 60);
                cout << "*";
                pin = pin * 10 + (ch - '0');
                count++;
            }
        }
        else if (ch == 8)
        {
            if (count > 0)
            {
                cout << "\b \b";
                pin = pin / 10;
                count--;
            }
        }
        else if (ch == 13)
        {
            if (count == 4)
            {
                cout << "\n";
                return pin;
            }
        }
    }
}

void logTransaction(long accID, double amt, const char *type)
{
    Transaction t(accID, amt, type);
    ofstream file(TRANS_FILE, ios::binary | ios::app);
    file.write(reinterpret_cast<const char *>(&t), sizeof(Transaction));
    file.close();
}

long getnextAccountID()
{
    ifstream file(ACCOUNTS_FILE, ios::binary);
    if (!file)
        return 1001;

    file.seekg(0, ios::end);
    if (file.tellg() == 0)
        return 1001;

    long size = static_cast<long>(sizeof(Account));
    file.seekg(-size, ios::end);

    Account lastAcc;
    lastAcc.readfromFile(file);
    return lastAcc.getID() + 1;
}

void showHistory(long targetID)
{
    ifstream file(TRANS_FILE, ios::binary);
    if (!file)
    {
        cout << "\n\t[No Transactions Found]";
        return;
    }

    Transaction t;
    system("cls");
    cout << "\n\t========TRANSACTION HISTORY========";
    cout << "\n\tAccount ID: " << targetID;
    cout << "\n\t------------------------------------";
    cout << "\n\t|   TYPE   |   AMOUNT   |   TIME   |";
    cout << "\n\t------------------------------------\n";

    file.read(reinterpret_cast<char *>(&t), sizeof(Transaction));
    while (file)
    {
        if (t.accountID == targetID)
        {
            char *dt = ctime(&t.date);
            dt[strlen(dt) - 1] = '\0';
            cout << "\t|" << left << setw(10) << t.type << "| $" << left << setw(10) << t.amount << "|" << left << setw(10) << dt << "|\n";
        }
        file.read(reinterpret_cast<char *>(&t), sizeof(Transaction));
    }
    file.close();
}

bool deleteAccount(long targetID)
{
    ifstream inFile(ACCOUNTS_FILE, ios::binary);
    if (!inFile)
    {
        cout << "\n\tAccount database not found!";
        return false;
    }

    ofstream tempFile("temp.dat", ios::binary);
    if (!tempFile)
    {
        cout << "\n\t[System Error] :: Unable to create temp File!";
        inFile.close();
        return false;
    }

    Account acc;
    bool deleted = false;

    while (inFile.read(reinterpret_cast<char *>(&acc), sizeof(Account)))
    {
        if (acc.getID() == targetID)
        {
            deleted = true;
            continue;
        }
        tempFile.write(reinterpret_cast<char *>(&acc), sizeof(Account));
    }

    inFile.close();
    tempFile.close();

    if (!deleted)
    {
        cout << "\n\tAccount not found!";
        remove("temp.dat");
        return false;
    }

    if (remove(ACCOUNTS_FILE) != 0)
    {
        cout << "\n\t[System Error] :: Cannot delete original file!";
        remove("temp.dat");
        return false;
    }

    if (rename("temp.dat", ACCOUNTS_FILE) != 0)
    {
        cout << "\n\t[System Error] :: Cannot rename temp file!";
        return false;
    }

    return true;
}

void deleteTransactions(long targetID)
{
    ifstream inFile(TRANS_FILE, ios::binary);
    if (!inFile)
    {
        cout << "\n\tTransaction database not found!";
        return;
    }

    ofstream tempFile("temp_trans.dat", ios::binary);
    if (!tempFile)
    {
        cout << "\n\t[System Error] :: Unable to create temp File!";
        inFile.close();
        return;
    }

    Transaction t;
    bool found = false;

    while (inFile.read(reinterpret_cast<char *>(&t), sizeof(Transaction)))
    {
        if (t.accountID == targetID)
        {
            found = true;
            continue;
        }
        tempFile.write(reinterpret_cast<char *>(&t), sizeof(Transaction));
    }

    inFile.close();
    tempFile.close();

    if (!found)
    {
        remove("temp_trans.dat");
        return;
    }

    if (remove(TRANS_FILE) != 0)
    {
        cout << "\n\t[Warning] :: Could not delete transaction history!";
        remove("temp_trans.dat");
        return;
    }

    if (rename("temp_trans.dat", TRANS_FILE) != 0)
    {
        cout << "\n\t[Warning] :: Could not update transaction file!";
    }
}

void adminDashboard()
{
    int totalaccounts = 0;
    double totalbalance = 0.0;
    system("cls");
    cout << "\n\t===========ADMIN DASHBOARD===========";
    cout << "\n\t-------------------------------------";
    cout << "\n\t| Acc ID |     NAME     |  BALANCE  |";
    cout << "\n\t-------------------------------------\n";

    ifstream file(ACCOUNTS_FILE, ios::binary);
    if (!file)
    {
        cout << "\n\t[DATABASE EMPTY]";
    }
    else
    {
        Account acc;
        while (file.read(reinterpret_cast<char *>(&acc), sizeof(Account)))
        {
            cout << "\t|" << left << setw(8) << acc.getID() << "|" << left << setw(14) << acc.getName() << "| $" << left << setw(9) << acc.getBal() << "|\n";
            totalaccounts++;
            totalbalance += acc.getBal();
        }
    }

    cout << "\n\t-------------------------------------";
    cout << "\n\tTotal Accounts: " << totalaccounts;
    cout << "\n\tTotal Balance : $" << totalbalance;
    cout << "\n\t-------------------------------------";

    cout << "\n\n\tPress Enter to Logout...";
    cin.ignore();
    cin.get();
}

void userDashboard(long id)
{
    bool loggedIn = true;
    while (loggedIn)
    {
        fstream file(ACCOUNTS_FILE, ios::binary | ios::in | ios::out);
        Account currentUser;

        if (!file)
        {
            cout << "Account database missing!";
            return;
        }

        while (file.read(reinterpret_cast<char *>(&currentUser), sizeof(Account)))
        {
            if (currentUser.getID() == id)
            {
                break;
            }
        }

        long pos = -static_cast<long>(sizeof(Account));

        system("cls");
        cout << "\n\t===============================================";
        cout << "\n\t            WELCOME " << currentUser.getName();
        cout << "\n\t===============================================";
        cout << "\n\tAccount ID: " << currentUser.getID();
        cout << "\n\t-----------------------------------------------";
        cout << "\n\t1. Check Balance";
        cout << "\n\t2. Deposit";
        cout << "\n\t3. Withdraw";
        cout << "\n\t4. Transaction History";
        cout << "\n\t5. Logout";
        cout << "\n\t6. Close Account";
        cout << "\n\t-----------------------------------------------";
        cout << "\n\n\tSelect: ";

        int choice;
        cin >> choice;
        Beep(800, 60);

        double amt;

        switch (choice)
        {
        case 1:
            cout << "\n\tCurrent Balance: $" << currentUser.getBal() << "\n";
            break;
        case 2:
            cout << "\tEnter the amount to deposit: $";
            cin >> amt;

            if (amt <= 0)
            {
                cout << "\n\tInvalid amount!";
            }
            else
            {
                currentUser.deposit(amt);
                file.seekp(pos, ios::cur);
                currentUser.savetoFile(file);
                logTransaction(id, amt, "DEPOSIT");
                cout << "\n\tDeposit Successful!";
                Beep(1000, 100);
            }
            break;
        case 3:
            cout << "\tEnter the amount to be withdrawn: $";
            cin >> amt;
            if (currentUser.withdraw(amt))
            {
                file.seekp(pos, ios::cur);
                currentUser.savetoFile(file);
                logTransaction(id, amt, "WITHDRAW");
                cout << "\n\tCash dispensed";
                Beep(1000, 100);
            }
            else
            {
                cout << "\n\tInsufficient Funds";
                Beep(400, 150);
            }
            break;
        case 4:
            showHistory(id);
            break;
        case 5:
            system("cls");
            cout << "\n\n\n\t========================================";
            cout << "\n\t   Thank you for using UG BANK ATM!";
            cout << "\n\t   Goodbye, " << currentUser.getName() << "!";
            cout << "\n\t========================================\n";
            Sleep(1500);
            loggedIn = false;
            break;
        case 6:
            char confirm;
            cout << "\n\tAre you sure you want to close this account? (Y/N): ";
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y')
            {
                file.close();
                if (deleteAccount(id))
                {
                    deleteTransactions(id);
                    cout << "\n\tAccount Closed Successfully!";
                    Sleep(2000);
                    loggedIn = false;
                }
                else
                {
                    cout << "\n\tError closing account!";
                }
            }
            break;
        }

        if (loggedIn)
        {
            cout << "\n\n\tPress Enter to return to menu...";
            cin.ignore();
            cin.get();
        }

        file.close();
    }
}

int main()
{
    cout << fixed << setprecision(2);
    while (true)
    {
        system("cls");
        cout << "\n\t=========================================";
        cout << "\n\t         WELCOME TO UG BANK ATM";
        cout << "\n\t=========================================";
        cout << "\n\t1. Login";
        cout << "\n\t2. Create Account";
        cout << "\n\t3. Exit";
        cout << "\n\t-----------------------------------------";
        cout << "\n\tSelect: ";
        int choice;
        cin >> choice;
        Beep(800, 60);

        if (choice == 1)
        {
            long id;
            int pin;

            cout << "\n\tEnter Account ID: ";
            cin >> id;
            cout << "\n\tEnter 4 digit PIN: ";
            pin = inputPIN();
            while (pin > 9999 || pin < 1000)
            {
                Beep(400, 150);
                cout << "\nInvalid PIN. IT SHOULD BE 4 DIGIT. ENTER AGAIN: ";
                cin >> pin;
            }

            if (id == ADMIN_ID && pin == ADMIN_PIN)
            {
                adminDashboard();
            }
            else
            {
                ifstream file(ACCOUNTS_FILE, ios::binary);
                Account acc;
                bool found = false;
                while (file.read(reinterpret_cast<char *>(&acc), sizeof(Account)))
                {
                    if (acc.getID() == id && acc.verifyPin(pin))
                    {
                        found = true;
                        break;
                    }
                }
                file.close();

                if (found)
                    userDashboard(id);
                else
                {
                    Beep(400, 150);
                    cout << "\tInvalid Credentials";
                    Sleep(1000);
                }
            }
        }
        else if (choice == 2)
        {
            system("cls");
            char name[50];
            int pin;
            double bal;

            long newID = getnextAccountID();

            cout << "\n\t====================================";
            cout << "\n\t         CREATE NEW ACCOUNT";
            cout << "\n\t====================================\n";

            cout << "\tEnter Name      : ";
            cin.ignore();
            cin.getline(name, 50);

            int pin1, pin2;
            do
            {
                cout << "\tSet 4-digit PIN : ";
                pin1 = inputPIN();

                cout << "\tConfirm PIN     : ";
                pin2 = inputPIN();

                if (pin1 != pin2)
                    cout << "\n\t[!] PINs do not match. Try again.\n";
            } while (pin1 != pin2);

            pin = pin1;

            cout << "\tInitial Deposit : $";
            cin >> bal;

            while (bal <= 0)
            {
                Beep(400, 150);
                cout << "\t[!] Invalid amount. Enter again: $";
                cin >> bal;
            }

            cout << "\n\t------------------------------------";
            cout << "\n\tAccount ID      : " << newID;
            cout << "\n\t------------------------------------";

            Account newAcc(newID, name, pin, bal);
            ofstream file(ACCOUNTS_FILE, ios::binary | ios::app);
            newAcc.savetoFile(file);
            file.close();

            logTransaction(newID, bal, "OPEN");

            Sleep(1200);
            cout << "\n\tAccount created successfully!";
            Sleep(1200);
        }
        else if (choice == 3)
        {
            break;
        }
    }
    return 0;
}
