#include <iostream>
using namespace std;

int menu(){
    int choice;
    cout << "1. Register Account" << endl;
    cout << "2. Login" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter choice: ";
    if (cin>>choice){
        return choice;
    } else {
        cout << "invalid input. Please enter a number." << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
    return choice;
}

int main(int argc, const char** argv) {
    while (true) {

        switch (menu())
        {
        case 1:
            cout << "case 1" << endl;
            break;
        default:
            break;
        }
    }
    return 0;
}
