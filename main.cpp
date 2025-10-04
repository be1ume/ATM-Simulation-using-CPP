#include <iostream>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include <time.h>
#include <conio.h>
#define MAX 5
#define v 120

using namespace std;

struct Date {
    int day, month, year;
};

struct AccountInfo {
    int accountNumber;
    string name, contactNumber, pin;
    double balance;
    Date birthDate;
};

struct List {
    AccountInfo accounts[MAX];
    int last;
};

struct CardInfo {
    int accountNumber;
    string pin;
};

class ATM {
    private:
        List L;
        CardInfo card;
        bool isEmpty();
        bool isFull();
        bool isDuplicate(int accountNumber);
    public:
        int locate(int accountNumber);
        bool checkUsbDrive();
        void init();
        string getUsbPath();
        void registerAccount();
        void save();
        void saveToUsb();
        void retrieve();
        void retrieveFromUsb();
        bool authenticate();
        void balanceInquiry();
        void deposit();
        void withdraw();
        void transfer();
        void changePin();
        string getPin();
        string encrypt(string pin);
        string decrypt(string pin);
        //void other();
};

//UI animations;
void printToxy(int x, int y, string text);
void gotoxy(int x, int y);
void RegMenu();
int getInt(int x);
string getStr(int x);

int transactionMenu(){
    int choice;
    cout << "1. Balance Inquiry" << endl;
    cout << "2. Deposit" << endl;
    cout << "3. Withdraw" << endl;
    cout << "4. Transfer" << endl;
    cout << "5. Change PIN" << endl;
    cout << "6. Exit" << endl;
    cout << "Enter choice: ";
    cin >> choice;
    return choice;
}

int main() {
    ATM atm;
    atm.init();
    atm.retrieve();
    atm.retrieveFromUsb();
    int count = 0;
    while (true) {
        system("cls");
        printToxy(30, 4, "     I N S E R T   U S B   C A R D ...  ");
        printToxy(30, 5, "+_______________________________________+");
        printToxy(30, 6, "|                                       |");
        printToxy(30, 7, "|                 - - -                 |");
        printToxy(30, 8, "+_______________________________________+");
        printToxy(40, 11, "___________________");
        printToxy(40, 12, "+    |            _|");
        printToxy(40, 13, "|    |  A  T  M  |");
        printToxy(40, 14, "|____|____________|");
        printToxy(40, 15, "|  | __________ |");
        printToxy(40, 16, "|  | |_U__S__B_| |");
        printToxy(40, 17, "|\\_|____________|");
        printToxy(40, 18, "|  |            \\");
        printToxy(40, 19, "|  |             +");
        printToxy(40, 20, "|  |             |");
        printToxy(40, 21, "|  +             |");
        printToxy(40, 22, "\\  |             |");
        printToxy(40, 23, " \\_|____________ |");
        if(!atm.checkUsbDrive()){
            if(count == 0) {
                count++;
                printToxy(30, 4, "     I N S E R T   U S B   C A R D ..  ");
               
                printToxy(40,25, "\n"); system("pause");
            }
            if (count != 0) {
                count--;
                printToxy(30, 4, " \033[31m    I N S E R T   U S B   C A R D ...     \033[0m");
                printToxy(40,23, "\n\n\n");
                system("pause");
            }
            continue;
        } else {
            printToxy(29, 25, "[ \033[33mDO NOT REMOVE USB DRIVE WHILE USING ATM\033[0m ]");
            if (filesystem::exists(atm.getUsbPath() + string("pin.code.txt"))){
                printToxy(41, 7, "\033[33mR E G I S T E R E D\033[0m");
                printToxy(40,23, "\n\n\n"); system("pause"); system("cls");
                atm.retrieveFromUsb();
                if(atm.authenticate()){
                    atm.retrieve();
                    while(true){ //medjo dito
                        switch(transactionMenu()){
                            case 1:
                                atm.balanceInquiry();
                                break;
                            case 2:
                                atm.deposit();
                                break;
                            case 3:
                                atm.withdraw();
                                break;
                            case 4:
                                atm.transfer();
                                break;
                            case 5:
                                atm.changePin();
                                atm.saveToUsb();
                                atm.save();
                                break;
                            case 6:
                                cout << "Logging out..." << endl;
                                atm.saveToUsb();
                                atm.save();
                                system("pause");
                                exit(0);
                            default:
                                cout << "Invalid choice. Please try again." << endl;
                                system("pause");
                        }
                    }
                } else {
                    cout << "Authentication failed. Returning to main menu." << endl;
                    system("pause");
                }
            } else {
                printToxy(38, 7, "\033[33mN O T   R E G I S T E R E D\033[0m");
                printToxy(40,23, "\n\n\n"); system("pause"); system("cls");
                RegMenu();
                atm.registerAccount();
            }
        }
    }
    return 0;
};

void ATM::init() {
    L.last = -1;
}

bool ATM::isEmpty() {
    return (L.last == -1);
}

bool ATM::isFull() {
    return (L.last == MAX - 1);
}

int ATM::locate(int accountNumber) {
    for (int i = 0; i <= L.last; i++) {
        if (L.accounts[i].accountNumber == accountNumber) {
            return i;
        }
    }
    return -1;
}

bool ATM::isDuplicate(int accountNumber) {
    return (locate(accountNumber) != -1);
}

bool ATM::checkUsbDrive() {
    char drive = 'D';
    char rootPath[4] = " :\\";
    for (drive = 'C'; drive <= 'Z'; drive++) {
        rootPath[0] = drive;
        UINT driveType = GetDriveTypeA(rootPath);
        if (driveType == DRIVE_REMOVABLE) {
            return true;
        }
    }
    return false;
}

string ATM::getUsbPath() {
    char drive = 'D';
    char rootPath[4] = " :\\";
    for (drive = 'C'; drive <= 'Z'; drive++) {
        rootPath[0] = drive;
        UINT driveType = GetDriveTypeA(rootPath);
        if (driveType == DRIVE_REMOVABLE) {
            return string(rootPath);
        }
    }
    return "";
}

void ATM::registerAccount() {
    AccountInfo newAcc;
    cout<<"\033[33m";
    if (!checkUsbDrive()) {
        cout << "No USB drive detected." << endl;
        system("pause");
        return;
    }
    if (isFull()) {
        cout << "Database exceeded maximum capacity." << endl;
        system("pause");
        return;
    }

    if (isDuplicate(newAcc.accountNumber)) {
        cout << "Account already exists." << endl;
        system("pause");
        return;
    }
    printToxy(37,6,"");
    getline(cin, newAcc.name);
    printToxy(38, 21, newAcc.name);
    srand(time(NULL));
    newAcc.accountNumber = rand() % 99999 + 10000;
    printToxy(89, 6,"");
    cout << newAcc.accountNumber;
    printToxy(37,9,"");
    newAcc.birthDate.day = getInt(2);
    printToxy(42, 9, "");
    newAcc.birthDate.month = getInt(2);
    printToxy(47, 9, "");
    newAcc.birthDate.year = getInt(2);
    printToxy(94, 9,"");
    newAcc.contactNumber = getStr(11);
    printToxy(38,21,"");
    cout << newAcc.contactNumber;
    printToxy(37,12,"");
    cin >> newAcc.balance;
    printToxy(37,15,"");
    newAcc.pin = encrypt(getPin());
    printToxy(37,18,"");
    if(getPin() != decrypt(newAcc.pin)) {
        printToxy(38, 21, "PINs do not match. Registration failed.");
        cout<<"\033[0m";
        printToxy(40,23, "\n\n\n"); cin.ignore(); system("pause");
        return;
    }

    if (filesystem::exists(getUsbPath() + string("pin.code.txt"))) {
        printToxy(38, 21, "A CCOUNT ALREADY EXIST...");
        cout<<"\033[0m";
        printToxy(40,23, "\n\n\n"); cin.ignore(); system("pause");
        return;
    }

    if (newAcc.balance < 5000) {
        printToxy(38, 21, "Initial deposit must be at least 5000.");
        cout<<"\033[0m";
        printToxy(40,23, "\n\n\n"); cin.ignore(); system("pause");
        return;
    }

    L.accounts[++L.last] = newAcc;

    printToxy(38, 21, "Account registered successfully.");
    printToxy(40,23, "\n\n\n"); cin.ignore(); system("pause");
    saveToUsb();
    save();
    cout<<"\033[0m";
}

void ATM::saveToUsb() {
    ofstream fout(getUsbPath() + string("pin.code.txt"));
    if (!fout) {
        cout << "Error creating file on USB drive." << endl;
        system("pause");
        return;
    }
    fout << L.accounts[L.last].accountNumber << "\t" << encrypt(L.accounts[L.last].pin) << endl;
    fout.close();
}

void ATM::save() {
    ofstream fout("database.txt");
    if (!fout) {
        cout << "Error creating database file." << endl;
        system("pause");
        return;
    }
    for (int i = 0; i <= L.last; i++) {
        fout << L.accounts[i].accountNumber << "\t"
            << L.accounts[i].name << "\t"
            << L.accounts[i].birthDate.day << "/"
             << L.accounts[i].birthDate.month << "/"
             << L.accounts[i].birthDate.year << "\t"
             << L.accounts[i].contactNumber << "\t"
             << L.accounts[i].balance<< "\t"
             << L.accounts[i].pin << endl;
    }
    cout << "Database saved successfully." << endl;
    system("pause");
    fout.close();
}

void ATM::retrieve() {
    ifstream fin("database.txt");
    if (!fin) return;
    AccountInfo acc;
    L.last = -1;
    while (fin >> acc.accountNumber) {
        fin.ignore(); getline(fin, acc.name, '\t');
        char slash;
        fin >> acc.birthDate.day >> slash >> acc.birthDate.month >> slash >> acc.birthDate.year;
        fin >> acc.contactNumber >> acc.balance>> acc.pin;
        L.accounts[++L.last] = acc;
    }
    fin.close();
}

void ATM::retrieveFromUsb() {
    ifstream fin(getUsbPath() + string("pin.code.txt"));
    if (!fin) return;
    fin >> card.accountNumber >> card.pin;
    fin.close();
    cout << "Retrieved from USB - Account Number: " << card.accountNumber << ", PIN: " << card.pin << endl;
}

bool ATM::authenticate() {
    int attempts = 3;
    int p = locate(card.accountNumber);
    string tempPin;
    for (int i=0; i<attempts; i++) {
        cout << "ACCOUTN NUM: "<< card.accountNumber << endl;
        cout << "Enter PIN: ";
        tempPin = getPin();
        if (tempPin == decrypt(L.accounts[p].pin)) {
            cout << "Authentication successful." << endl;
            return true;
        } else {
            cout << "Incorrect credentials. Try again." << endl;
        }
    } return false;
}

void ATM::balanceInquiry() { 
    int p = locate(card.accountNumber);
    if (p != -1) {
        cout << "Current Balance: " << L.accounts[p].balance << endl;
        system("pause");
    } else {
        cout << "Account not found." << endl;
        system("pause");
    }
}

void ATM::deposit() {
    int p = locate(card.accountNumber);
    cout << "Enter amount to deposit: ";
    double amount; cin >> amount;
    if (p!= -1) {
        L.accounts[p].balance += amount;
        cout << "Deposit successful. New balance: " << L.accounts[p].balance << endl;
        saveToUsb();
        save();
        system("pause");
    } else {
        cout << "Error processing deposit." << endl;
        system("pause");
    }
}

void ATM::withdraw() {
    int p = locate(card.accountNumber);
    cout << "Enter amount to withdraw: ";
    double amount; cin >> amount;
    if (p!= -1){
        if (amount < L.accounts[p].balance){
            L.accounts[p].balance -= amount;
            cout << "Withdrawal successful: "<< amount <<". New balance: " << L.accounts[p].balance << endl;
            system("pause");
        }
        else {
            cout << "Insufficient balance." << endl;
            system("pause");
        }
    }
}

void ATM::transfer() {
    int p = locate(card.accountNumber);
    int target;
    cout << "Enter target account number: ";
    cin >> target;
    int t = locate(target);
    if (t == -1) {
        cout << "Target account not found." << endl;
        system("pause");
        return;
    } else if (t == p){
        cout << "Cannot transfer to the same account." << endl;
        system("pause");
        return;
        return;
    } else {
        cout << "Enter ammount to transfer: ";
        double amount; cin >> amount;
        if (amount < L.accounts[p].balance){
            L.accounts[p].balance -= amount;
            L.accounts[t].balance += amount;
            cout << "Transfer successful: "<< amount <<". New balance: " << L.accounts[p].balance << endl;
            system("pause");
        } else {
            cout << "Insufficient balance." << endl;
            system("pause");
        }
    }
}

void ATM::changePin() {
    int p = locate(card.accountNumber);
    string newPin, oldPin, confirmPin;
    cout << "Enter old PIN: ";
    oldPin = getPin();
    if (oldPin == decrypt(L.accounts[p].pin)) {
        cout << "Enter new PIN: "; newPin = getPin();
        cout << "Confirm new PIN: "; confirmPin = getPin();
        if(newPin == confirmPin) {
            L.accounts[p].pin = encrypt(newPin);
            card.pin = encrypt(newPin);
        } else {
            cout << "PINs do not match. Please try again." << endl;
            system("pause");
        }
    } else {
        cout << "Incorrect old PIN. Please try again." << endl;
        system("pause");
    }
}

string ATM::getPin() {
    string pincode;
    int ctr = 0;
    char ch;
    while ((ch = _getch()) != '\r' && ctr < 6) {
        if (ch >= '0' && ch <= '9') {
            pincode += ch;
            ctr++;
            cout << '*';
        } else if (ch == '\b') {
            if (!pincode.empty()) {
                cout << "\b \b";
                ctr--;
                pincode.pop_back();
            }
        } else {
        }
    }
    cout << endl;
    return pincode;
}

string ATM::encrypt(string pin){
    for(int i=0; i<pin.size(); i++){
        pin[i]=pin[i]+v;
    }
    return pin;
}

string ATM::decrypt(string pin){
    for(int i=0; i<pin.size(); i++){
        pin[i]=pin[i]-v;
    }
    return pin;
}

int getInt(int x) {
    int ctr = 0;
    int num = 0; 
    char ch;
    while (ctr < x) {
        ch = _getch();
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            cout << ch; 
            ctr++;
        } else if (ch == '\b') {
            if (num > 0) {
                cout << "\b \b";
                num /= 10;
                ctr--;
            }
        }
    } return num;
}

string getStr(int x){
    int ctr = 0;
    string num;
    char ch;
    while ((ch = _getch()) != '\r' && ctr < x) {
        if (ch >= '0' && ch <= '9') {
            num += ch;
            ctr++;
            cout << ch;
        } else if (ch == '\b') {
            if (!num.empty()) {
                cout << "\b \b";
                ctr--;
                num.pop_back();
            }
        } else {
        }
    }
    cout << endl;
 
    return num;
}


// UI here
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void printToxy(int x, int y, string text) {
    gotoxy(x, y);
    cout << text;
}


void RegMenu(){
    printToxy(12, 1,  "________________________________________________________________________________________________");
    printToxy(12, 2,  "|                                                                                              |");
    printToxy(12, 3,  "|                               R  E  G  I  S  T  R  A  T  I  O  N                             |");
    printToxy(12, 4,  "|______________________________________________________________________________________________|");
    printToxy(12, 5,  "|                     |                             |                     |                    |");
    printToxy(12, 6,  "|  Account Name       |                             |  Account Number     |                    |");
    printToxy(12, 7,  "|_____________________|_____________________________|_____________________|____________________|");
    printToxy(12, 8,  "|                     |                             |                     |                    |");
    printToxy(12, 9,  "|  Birthday           |  -- / -- / --               |  Contact Number     |  +63               |");
    printToxy(12, 10, "|_____________________|_____________________________|_____________________|____________________|");
    printToxy(12, 11, "|                     |                                                                        |");
    printToxy(12, 12, "|  Initial Deposit    |                                                                        |");
    printToxy(12, 13, "|_____________________|________________________________________________________________________|");
    printToxy(12, 14, "|                     |                                                                        |");
    printToxy(12, 15, "|  Enter PIN Code     |                                                                        |");
    printToxy(12, 16, "|_____________________|________________________________________________________________________|");
    printToxy(12, 17, "|                     |                                                                        |");
    printToxy(12, 18, "|  Confirm PIN Code   |                                                                        |");
    printToxy(12, 19, "|_____________________|________________________________________________________________________|");
    printToxy(12, 20, "|                                                                                              |");
    printToxy(12, 21, "|                                         --- --- ---                                          |");
    printToxy(12, 22, "|______________________________________________________________________________________________|");
}