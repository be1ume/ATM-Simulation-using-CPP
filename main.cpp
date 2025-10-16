#include <iostream>
#include <windows.h>    // for moving within the filesystem, for USB
#include <fstream>      // for saving data
#include <filesystem>   //for filesystem::exist()
#include <time.h>       // for srand
#include <conio.h>      //for _getch(), pop_back and getch()
#define MAX 3
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
        void init();
        int locate(int accountNumber);
        bool checkUsbDrive();
        string getUsbPath();
        void registerAccount();
        bool authenticate();
        void balanceInquiry();
        void deposit();
        void withdraw();
        void transfer();
        void changePin();
        string getPin();
        string encrypt(string pin);
        string decrypt(string pin);
        void save();
        void saveToUsb();
        void retrieve();
        void retrieveFromUsb();
        AccountInfo nameIdGetter();
};

//UI and animation;
void printToxy(int x, int y, string text);
void gotoxy(int x, int y);
void RegMenu();
void AuthMenu();
int tranMenu(AccountInfo x);
int getInt(int x);
string getStr(int x);

int main() {
    ATM atm;
    atm.init();
    atm.retrieve();
    atm.retrieveFromUsb();
    int count = 0;
    while (true) {
        system("cls");
        printToxy(35, 4, "I N S E R T   U S B   C A R D ...");
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
                printToxy(35, 4, "I N S E R T   U S B   C A R D .. ");
                printToxy(40,25, "\n"); system("pause");
            }
            if (count != 0) {
                count--;
                printToxy(34, 4, " \033[31mI N S E R T   U S B   C A R D ...\033[0m");
                printToxy(40,23, "\n\n\n");
                system("pause");
            }
            continue;
        } else {
            if (filesystem::exists(atm.getUsbPath() + string("pin.code.txt"))){
                printToxy(41, 7, "\033[33mR E G I S T E R E D\033[0m");
                printToxy(40,23, "\n\n\n"); system("pause"); system("cls");
                atm.retrieveFromUsb();
                printToxy(30, 5, "+_______________________________________+");
                printToxy(30, 6, "|                                       |");
                printToxy(30, 7, "|                 - - -                 |");
                printToxy(30, 8, "+_______________________________________+");
                printToxy(37, 4, "A U T H E N T I C A T I O N");
                printToxy(41, 7, "\033[33mR E G I S T E R E D\033[0m");
                if(atm.authenticate()){
                    atm.retrieve();
                    while(true){
                        switch(tranMenu(atm.nameIdGetter())){
                            case 1:
                                printToxy(53,22, "           ");
                                printToxy(53,19, "           ");
                                atm.balanceInquiry();
                                break;
                            case 2:
                                printToxy(53,22, "           ");
                                printToxy(53,19, "           ");
                                atm.withdraw();
                                break;
                            case 3:
                                printToxy(53,22, "           ");
                                printToxy(53,19, "           ");
                                atm.deposit();
                                break;
                            case 4:
                                printToxy(53,22, "           ");
                                printToxy(53,19, "           ");
                                atm.transfer();
                                break;
                            case 5:
                                printToxy(53,22, "           ");
                                printToxy(53,19, "           ");
                                atm.changePin();
                                atm.saveToUsb();
                                atm.save();
                                break;
                            case 6:
                                atm.saveToUsb();
                                atm.save();
                                printToxy(43,22, "\033[33mL  O  G  G  I  N  G     O  U  T");
                                printToxy(40,25, "\033[0m\n"); system("pause");
                                exit(0);
                            default:
                                printToxy(40,22, "\033[33mI  N  V  A  L  I  D     O  P  T  I  O  N");
                                printToxy(40,25, "\033[0m\n"); system("pause");
                        }
                    }
                } else {
                    printToxy(35,17,"\033[031m        E X I T I N G . . .           ");
                    printToxy(40,25, "\033[0m\n"); system("pause");
                    exit(0);
                }
            } else {
                printToxy(37, 7, "\033[33mN O T   R E G I S T E R E D\033[0m");
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
        printToxy(34, 21, "\033[31mN O   U S B   D E T E C T E D\033[0m");
        printToxy(40,25, "\033[0m\n"); system("pause");
        return;
    }
    if (isFull()) {
        printToxy(32, 21, "\033[31mD A T A B A S E  E X C E E D E D  M A X  C A P A C I T Y\033[0m");
        printToxy(40,25, "\033[0m\n"); system("pause");
        return;
    }

    if (isDuplicate(newAcc.accountNumber)) {
        printToxy(28, 21, "\033[31mA C C O U N T   A L R E A D Y   E X I S T\033[0m");
        printToxy(40,25, "\033[0m\n"); system("pause");
        return;
    }
    printToxy(37,6,"");
    getline(cin, newAcc.name);
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
    printToxy(37,12,"");
    cin >> newAcc.balance;
    printToxy(37,15,"");
    newAcc.pin = encrypt(getPin());
    printToxy(37,18,"");
    if(getPin() != decrypt(newAcc.pin)) {
        printToxy(44, 21, "\033[31mP I N   D I D   N O T   M A T C H\033[0m");
        cout<<"\033[0m";
        printToxy(40,25, "\033[0m\n"); cin.ignore(); system("pause");
        return;
    }
    if (newAcc.balance < 5000) {
        printToxy(29, 21, "\033[31mI N T I A L  D E P O S I T  M U S T  A T L E A S T  5 0 0 0");
        cout<<"\033[0m";
        printToxy(40,25, "\033[0m\n"); cin.ignore(); system("pause");
        return;
    }

    L.accounts[++L.last] = newAcc;
    printToxy(26, 21, "\033[33mR  E  G  I  S  T  E  R  E  D     S  U  C  C  E  S  S  F  U  L  L  Y\033[0m");
    printToxy(40,25, "\033[0m\n"); cin.ignore(); system("pause");
    saveToUsb();
    save();
    cout<<"\033[0m";
}


bool ATM::authenticate() {
    int p = locate(card.accountNumber);
    string tempPin;
    for (int i=3; i>0; i--) {
        AuthMenu();
        printToxy(43, 13, "\033[33m");
        tempPin = getPin();
        if (tempPin == decrypt(L.accounts[p].pin)) {
            printToxy(40, 17, "C O R R E C T   P I N");
            printToxy(40,25, "\n"); system("pause");
            cout<<"\033[0m";
            return true;
        } else {
            gotoxy(34,17);
            cout<<"I N C O R R E C T   P I N  ( "<<i-1<<" )";
            printToxy(40,25, "\n"); system("pause");
        } cout<<"\033[0m";
    }  return false;
}

void ATM::balanceInquiry() {
    int p = locate(card.accountNumber);
    if (p != -1) {
        gotoxy(15,19);
        cout << "\033[33mYour Balance: Php " << L.accounts[p].balance << endl;
        printToxy(38,22, "C  U  R  R  E  N  T    B  A  L  A  N  C  E ");
        cout<<"\033[0m";
        printToxy(40,25, "\n"); system("pause");
    } else {
        printToxy(53,22, "E R R O R");
        cout<<"\033[0m";
    }
 }

void ATM::deposit() {
    int p = locate(card.accountNumber);
    printToxy(41, 22, "\033[33mE  N  T  E  R     A   M  M  O  U  N  T\033[0m");
    gotoxy(15,19);
    cout << "\033[33mEnter Amount To Deposit: Php ";
    double amount; cin >> amount;
    if (p!= -1) {
        L.accounts[p].balance += amount;
        printToxy(28, 22, "\033[33mD  E  P  O  S  I  T  E  D     S  U  C  C  E  S  S  F  U  L  L  Y\033[0m");
        saveToUsb(); save();
        cout<<"\033[0m";
        printToxy(40,25, "\n"); system("pause");
    } else {
        printToxy(53,22, "E R R O R");
        cout<<"\033[0m";
        printToxy(40,25, "\n"); system("pause");
    }
}

void ATM::withdraw() {
    int p = locate(card.accountNumber);
    printToxy(41, 22, "\033[33mE  N  T  E  R     A   M  M  O  U  N  T\033[0m");
    gotoxy(15,19);
    cout << "\033[33mEnter Amount To Withdraw: Php ";
    double amount; cin >> amount;
    if (p!= -1){
        if (amount < L.accounts[p].balance){
            L.accounts[p].balance -= amount;
            printToxy(28,22, "\033[33mW  I  T  H  D  R  A  W  N     S  U  C  C  E  S  S  F  U  L  L  Y\033[0m");
            saveToUsb(); save();
            cout<<"\033[0m";
            printToxy(40,25, "\n"); system("pause");
        }
        else {
            printToxy(41,22, "\033[31mI N S U F I C I E N T   B A L A N C E  \033[0m");
            printToxy(40,25, "\n"); system("pause");
        }
    }
}

void ATM::transfer() {
    int p = locate(card.accountNumber);
    int target;
    printToxy(43,22, "\033[33mE N T E R   A C C O U N T   N O . \033[0m");
    gotoxy(15,19);
    cout << "\033[33mEnter Account Number for Transfer: ";
    cin >> target;
    int t = locate(target);
    if (t == -1) {
        printToxy(38,22, "\033[31mA C C O U N T   D O E S   N O T   E X I S T\033[0m");
        printToxy(40,25, "\n"); system("pause");
        return;
    } else if (t == p){
        printToxy(37,22, "\033[31mC A N N O T   T R A N S F E R   T O   S E L F\033[0m");
        printToxy(40,25, "\n"); system("pause");
        return;
    } else {
        printToxy(40,22, "\033[33mE  N  T  E  R     A   M  M  O  U  N  T ");
        gotoxy(15,19);
        cout << "\033[33mEnter Amount To Transfer: Php              ";
        gotoxy(45,19);
        double amount; cin >> amount;

        if (amount < L.accounts[p].balance){
            L.accounts[p].balance -= amount;
            L.accounts[t].balance += amount;
            printToxy(26,22, "T  R  A  N  S  F  E  R  E  D     S  U  C  C  E  S  S  F  U  L  L  Y");
            save();
            printToxy(40,25, "\033[0m\n"); system("pause");
        } else {
            printToxy(40,22, "\033[31m I N S U F I C I E N T   B A L A N C E\033[0m");
            printToxy(40,25, "\033[0m\n"); system("pause");
        }
    }
}

void ATM::changePin() {
    int p = locate(card.accountNumber);
    string newPin, oldPin, confirmPin;
    printToxy(42,22, "\033[33mE  N  T  E  R    O  L  D    P  I  N");
    gotoxy(15,19);
    cout << "\033[33mEnter Old Pin: ";
    oldPin = getPin();
    if (oldPin == decrypt(L.accounts[p].pin)) {
        printToxy(41,22, "\033[33mE  N  T  E  R    N  E  W    P  I  N  ");
        gotoxy(15,19);
        cout << "Enter new PIN:             ";
        gotoxy(30, 19); newPin = getPin();
        printToxy(38,22, "C  O  N  F  I  R  M    N  E  W    P  I  N  ");
        gotoxy(15,19);
        cout << "Confirm new PIN:             ";
        gotoxy(32, 19);confirmPin = getPin();
        if(newPin == confirmPin) {
            L.accounts[p].pin = encrypt(newPin);
            card.pin = encrypt(newPin);
            printToxy(36,22, "P I N   C H A N G E D   S U C C E S S F U L L Y ");
            printToxy(40,25, "\033[0m\n"); system("pause");
        } else {
            printToxy(35,22, "\033[31mP  I  N    D  O  E  S    N  O  T    M  A  T  C  H");
            printToxy(40,25, "\033[0m\n"); system("pause");
        }
    } else {
        printToxy(42,22, "\033[31mI  N  C  O  R  R  E  C  T    P  I  N");
        printToxy(40,25, "\033[0m\n"); system("pause");
    }
}

string ATM::getPin() {
    string pincode;
    int ctr = 0;
    char ch;
    while(1){
        ch = _getch();
        if(ch == '\r'){
            if(ctr==4) break;
            else continue;
        }
        else if(ch >= '0' && ch <='9'){
            if(ctr<6){
                pincode += ch;
                cout << '*';
                ctr++;
                if(ctr==6){
                    cout<<endl;
                    break;
                }
            }
        }
        else if(ch == '\b'){
            if(!pincode.empty()){
                cout << "\b \b";
                ctr--;
                pincode.pop_back();
            }
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

void ATM::saveToUsb() {
    ofstream fout(getUsbPath() + string("pin.code.txt"));
    if (!fout) return;
    fout << L.accounts[L.last].accountNumber << "\t" << encrypt(L.accounts[L.last].pin) << endl;
    fout.close();
}

void ATM::save() {
    ofstream fout("database.txt");
    if (!fout) return;
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
}

int getInt(int x) {
    int ctr = 0;
    int num = 0;
    char ch;
    while (ctr < x) {
        ch = getch();
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
    printToxy(12, 6,  "|  Account Name:      |                             |  Account Number:    |                    |");
    printToxy(12, 7,  "|_____________________|_____________________________|_____________________|____________________|");
    printToxy(12, 8,  "|                     |                             |                     |                    |");
    printToxy(12, 9,  "|  Birthday:          |  mm / dd / yy               |  Contact Number:    |  +63               |");
    printToxy(12, 10, "|_____________________|_____________________________|_____________________|____________________|");
    printToxy(12, 11, "|                     |                                                                        |");
    printToxy(12, 12, "|  Initial Deposit:   |                                                                        |");
    printToxy(12, 13, "|_____________________|________________________________________________________________________|");
    printToxy(12, 14, "|                     |                                                                        |");
    printToxy(12, 15, "|  Enter PIN Code:    |                                                                        |");
    printToxy(12, 16, "|_____________________|________________________________________________________________________|");
    printToxy(12, 17, "|                     |                                                                        |");
    printToxy(12, 18, "|  Confirm PIN Code:  |                                                                        |");
    printToxy(12, 19, "|_____________________|________________________________________________________________________|");
    printToxy(12, 20, "|                                                                                              |");
    printToxy(12, 21, "|                                         --- --- ---                                          |");
    printToxy(12, 22, "|______________________________________________________________________________________________|");
}

void AuthMenu(){
    printToxy(30, 11, "_________________________________________");
    printToxy(30, 12, "|        |                              |");
    printToxy(30, 13, "|  PIN:  |                              |");
    printToxy(30, 14, "|________|______________________________|");
    printToxy(25, 15, "___________________________________________________");
    printToxy(25, 16, "|                                                 |");
    printToxy(25, 17, "|                   --- --- ---                   |");
    printToxy(25, 18, "|_________________________________________________|");
}

AccountInfo ATM::nameIdGetter(){
    AccountInfo temp;
    int p = locate(card.accountNumber);
    temp.accountNumber = card.accountNumber;
    temp.name = L.accounts[p].name;
    return temp;
}

int tranMenu(AccountInfo x){
    int opt;
    printToxy(12, 4, "________________________________________________________________________________________________");
    printToxy(12, 5, "|                                                                                              |");
    printToxy(12, 6, "|                                T  R  A  N  S  A  C  T  I  O  N                               |");
    printToxy(12, 7, "|______________________________________________________________________________________________|");
    printToxy(12, 8, "|                     |                             |                     |                    |");
    printToxy(12, 9, "|  Name:              |                             |  Account No.:       |                    |");
    printToxy(12, 10, "|_____________________|_____________________________|_____________________|____________________|");
    printToxy(12, 11, "|                     |                                                                        |");
    printToxy(12, 12, "|                     |           [1] Balance Inquiry       [4] Fund Transfer                  |");
    printToxy(12, 13, "|                     |                                                                        |");
    printToxy(12, 14, "|    Option: [   ]    |           [2] Withdraw              [5] Change PIN Code                |");
    printToxy(12, 15, "|                     |                                                                        |");
    printToxy(12, 16, "|                     |           [3] Deposit               [6] Exit                           |");
    printToxy(12, 17, "|_____________________|________________________________________________________________________|");
    printToxy(12, 18, "|                                                                                              |");
    printToxy(12, 19, "|                                        --- --- ---                                           |");
    printToxy(12, 20, "|______________________________________________________________________________________________|");
    printToxy(12, 21, "|                                                                                              |");
    printToxy(12, 22, "|                                        --- --- ---                                           |");
    printToxy(12, 23, "|______________________________________________________________________________________________|");
    cout<<"\033[33m";
    printToxy(37,9, x.name);
    gotoxy(89, 9); cout<<x.accountNumber;
    gotoxy(27,14); cin>>opt;
    cout<<"\033[0m";
    return opt;
}
