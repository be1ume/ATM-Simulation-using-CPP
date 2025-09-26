#include <iostream>
#include <windows.h>
#include <fstream>
#include <filesystem> // for checking if file exists in the usb
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
    double intialDeposit;
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

class RegistrationModule {
    private:
        List L;
        CardInfo card;
        bool isEmpty();
        bool isFull();
        int locate(int accountNumber);
        bool isDuplicate(int accountNumber);
    public:
        bool checkUsbDrive();
        void init();
        string getUsbPath();
        void registerAccount();
        void save();
        void saveToUsb();
        void retrieveFromUsb();
        void retrieve();
};

class TransactionModule {
    private:
        RegistrationModule RM;
    public:
        void deposit();
        void withdraw();
        void transfer();
};

int menu(){
    int choice;
    cout << "1. Register Account" << endl;
    cout << "2. Exit" << endl;
    cout << "Enter choice: ";
    cin >> choice;
    return choice;
}

int main() {
    RegistrationModule regMod;
    regMod.init();
    regMod.retrieve();
    regMod.retrieveFromUsb();
    while (true) {
        if(!regMod.checkUsbDrive()){
            cout<<"No USB drive detected. Please insert a USB drive."<<endl;
            system("pause");
            continue;
        } else {
            switch (menu()){
                case 1:
                    regMod.registerAccount();
                    break;
                case 2:
                    cout << "Exiting program." << endl;
                    return 0;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                    system("pause");

            }
        }
    }
    regMod.registerAccount();
    regMod.saveToUsb();
    return 0;
};

void RegistrationModule::init() {
    L.last = -1;
}

bool RegistrationModule::isEmpty() {
    return (L.last == -1);
}

bool RegistrationModule::isFull() {
    return (L.last == MAX - 1);
}

int RegistrationModule::locate(int accountNumber) {
    for (int i = 0; i <= L.last; i++) {
        if (L.accounts[i].accountNumber == accountNumber) {
            return i;
        }
    }
    return -1;
}

bool RegistrationModule::isDuplicate(int accountNumber) {
    return (locate(accountNumber) != -1);
}

bool RegistrationModule::checkUsbDrive() {
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

string RegistrationModule::getUsbPath() {
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

void RegistrationModule::registerAccount() {
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
    cout << "Enter Account Number: ";
    cin >> newAcc.accountNumber;

    if (isDuplicate(newAcc.accountNumber)) {
        cout << "Account already exists." << endl;
        system("pause");
        return;
    }

    cout << "Enter Name: ";
    cin.ignore(); getline(cin, newAcc.name);
    cout << "Enter Birth Date (DD MM YYYY): ";
    cin >> newAcc.birthDate.day >> newAcc.birthDate.month >> newAcc.birthDate.year;
    cout << "Enter Contact Number: ";
    cin >> newAcc.contactNumber;
    cout << "Enter Initial Deposit: ";
    cin >> newAcc.intialDeposit;
    cout << "Enter PIN: ";
    cin >> newAcc.pin;

    if (filesystem::exists(getUsbPath() + string("pin.code.txt"))) {
        cout << "Failed to register..." << endl;
        cout << "USB Card already registered..." << endl;
        system("pause");
        return;
    }

    L.accounts[++L.last] = newAcc;

    cout << "Account registered successfully." << endl;
    saveToUsb();
    save();
}

void RegistrationModule::saveToUsb() {
    ofstream fout(getUsbPath() + string("pin.code.txt"));
    if (!fout) {
        cout << "Error creating file on USB drive." << endl;
        system("pause");
        return;
    }
    fout << L.accounts[L.last].accountNumber << "\t" << L.accounts[L.last].pin << endl;
    fout.close();
}

void RegistrationModule::save() {
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
             << L.accounts[i].intialDeposit << "\t"
             << L.accounts[i].pin << endl;
    }
    cout << "Database saved successfully." << endl;
    system("pause");
    fout.close();
}

void RegistrationModule::retrieve() {
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
        fin >> acc.contactNumber >> acc.intialDeposit >> acc.pin;
        L.accounts[++L.last] = acc;
    }
    fin.close();
}

void RegistrationModule::retrieveFromUsb() {
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