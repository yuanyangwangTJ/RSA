#include <NTL/ZZ.h>
#include <iostream>
#include <fstream>
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
    bitset<128> data[1024];
    ifstream fin("1.txt", ios::binary);
    ofstream fout("2.txt", ios::binary);

    while (!fin.eof()) {
        fin.read((char*)&data, 16*1024);
        streamsize readNum = fin.gcount();
        cout << readNum << " " << data << endl;
        fout.write((char*)&data, readNum);

    }
    cout << data[0] << endl;
    cout << data[1] << endl;

    fin.close();
    fout.close();
    return 0;
}