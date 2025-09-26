#include <iostream>
#include <windows.h>
#include <fstream>
#include <filesystem> // for checking if file exists in the usb
#include <time.h>
#define MAX 5

using namespace std;

struct Date {
    int day;
    int month;
    int year;
};

struct AccountInfo {
    int accountNumber;
    string name;
    Date birthDate;
    int contactNumber;
    double balance;
    int pin;
};

struct List {
    AccountInfo accounts[MAX];
    int last;
};

struct CardInfo {
    int accountNumber;
    int pin;
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
        //void other();
};


int menu(){
    int choice;
    cout << "1. Register Account" << endl;
    cout << "2. Login" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter choice: ";
    cin >> choice;
    return choice;
}

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
    while (true) {
        if(!atm.checkUsbDrive()){
            cout<<"No USB drive detected. Please insert a USB drive."<<endl;
            system("pause");
            continue;
        } else {
            switch (menu()){
                case 1:
                    atm.registerAccount();
                    break;
                case 2: 
                    atm.retrieveFromUsb();
                    if(atm.authenticate()){
                        while(true){
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
                                    break;
                                case 6:
                                    cout << "Logging out..." << endl;
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
                case 3:
                    cout << "Exiting program." << endl;
                    return 0;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                    system("pause");

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

    cout << "Enter Name: ";
    cin.ignore(); getline(cin, newAcc.name);

    srand(time(NULL));
    newAcc.accountNumber = rand() % 99999 + 10000;
    cout << "Generated Account Number: " << newAcc.accountNumber << endl;

    cout << "Enter Birth Date (DD MM YYYY): ";
    cin >> newAcc.birthDate.day >> newAcc.birthDate.month >> newAcc.birthDate.year;
    cout << "Enter Contact Number: ";
    cin >> newAcc.contactNumber;
    cout << "Enter Initial Deposit: ";
    cin >> newAcc.balance;
    cout << "Enter PIN: ";
    cin >> newAcc.pin;

    if (filesystem::exists(getUsbPath() + string("pin.code.txt"))) {
        cout << "Failed to register..." << endl;
        cout << "USB Card already registered..." << endl;
        system("pause");
        return;
    }

    if (newAcc.balance < 5000) {
        cout << "Initial deposit must be at least 5000." << endl;
        cout << "Failed to register..." << endl;
        system("pause");
        return;
    }

    L.accounts[++L.last] = newAcc;

    cout << "Account registered successfully." << endl;
    saveToUsb();
    save();
}

void ATM::saveToUsb() {
    ofstream fout(getUsbPath() + string("pin.code.txt"));
    if (!fout) {
        cout << "Error creating file on USB drive." << endl;
        system("pause");
        return;
    }
    fout << L.accounts[L.last].accountNumber << "\t" << L.accounts[L.last].pin << endl;
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
    if (!fin) {
        cout << "Error opening database file." << endl;
        system("pause");
        return;
    }
    AccountInfo acc;
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
    if (!fin) {
        cout << "Error opening file on USB drive." << endl;
        system("pause");
        return;
    }
    fin >> card.accountNumber >> card.pin;
    fin.close();
    cout << "Retrieved from USB - Account Number: " << card.accountNumber << ", PIN: " << card.pin << endl;
}

bool ATM::authenticate() {
    int attempts = 3;
    int tempPin;
    for (int i=0; i<attempts; i++) {
        cout << "ACCOUTN NUM: "<< card.accountNumber << endl;
        cout << "Enter PIN: ";
        cin >> tempPin;
        if (tempPin == card.pin) {
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
    int newPin, oldPin, confirmPin;
    cout << "Enter old PIN: ";
    cin >> oldPin;
    if (oldPin == L.accounts[p].pin) {
        cout << "Enter new PIN: "; cin >> newPin;
        cout << "Confirm new PIN: "; cin >> confirmPin;
        if(newPin == confirmPin) {
            L.accounts[p].pin = newPin;
            card.pin = newPin;
            saveToUsb();
            save();
        } else {
            cout << "PINs do not match. Please try again." << endl;
            system("pause");
        }
    } else {
        cout << "Incorrect old PIN. Please try again." << endl;
        system("pause");
    }
}