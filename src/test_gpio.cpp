#include <iostream>    // Used for printf() statements
#include <wiringPi.h> // Include WiringPi library!

using namespace std;

int main(void)
{
    // Setup stuff:
    wiringPiSetupGpio(); // Initialize wiringPi -- using Broadcom pin numbers

    cout << "digitalRead(25)=" << digitalRead(25) << endl;
    cout << "digitalRead(28)=" << digitalRead(28) << endl;

    return 0;
}
