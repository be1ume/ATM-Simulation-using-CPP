#include <iostream>
#include <conio.h> // for _getch()
using namespace std;
int v = 120;
int getpincode() {
    int pincode = 0;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch >= '0' && ch <= '9') {
            pincode = pincode * 10 + (ch - '0');
            cout << '*';
        } else if (ch == '\b') {
            if (pincode > 0) {
                cout << "\b \b";
                pincode /= 10;
            }
        }
    }
    cout << endl; return pincode;
}
int encrypt(int pin);
int decrypt(int pin);
int main() {
    int pincode = getpincode();
 
    cout << "pincode entered: " << pincode << endl;
    pincode=encrypt(pincode);
    cout << "Encrypted pincode entered: " << pincode << endl;
    pincode=decrypt(pincode);
    cout << "Decrypted pincode entered: " << pincode << endl;
 
    return 0;
}
int encrypt(int pin){
return pin + v;
}
int decrypt(int pin){
return pin - v;
}