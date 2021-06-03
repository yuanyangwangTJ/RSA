#include <NTL/ZZ.h>
#include <iostream>
#include <bitset>

using namespace std;
using namespace NTL;
using std::bitset;

typedef bitset<8> byte;

byte FieldMult(byte x) {
    int t = x[7];
    byte y = x << 1;
    if (t == 1) {
        y = y ^ byte(0x1B);
    }
    return y;
}

void MixColumns(byte bits[4]) {
    byte x[4];
    for (int i = 0; i < 4; i++) {
        x[i] = bits[i];
    }
    bits[0] = x[1] ^ x[2] ^ x[3];
    bits[1] = x[0] ^ x[2] ^ x[3];
    bits[2] = x[0] ^ x[1] ^ x[3];
    bits[3] = x[0] ^ x[1] ^ x[2];

    for (int i = 0; i < 4; i++) {
        x[i] = FieldMult(x[i]);
        cout << x[i] << endl;
    }
    bits[0] = bits[0] ^ x[0] ^ x[1];
    bits[1] = bits[1] ^ x[1] ^ x[2];
    bits[2] = bits[2] ^ x[2] ^ x[3];
    bits[3] = bits[3] ^ x[0] ^ x[3];        
}

void PrintTest(byte bits[4]) {
    for (int i = 0; i < 4; i++) {
        cout << hex << bits[i].to_ulong() << ' ';
    }
    cout << endl;
}

int main()
{
    byte bits[4] = {
        0xdb, 0x13, 0x53, 0x45
    };
    PrintTest(bits);
    MixColumns(bits);
    PrintTest(bits);
    
    return 0;
}