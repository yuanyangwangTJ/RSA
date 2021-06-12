#include <iostream>
#include <cstring>
#include <NTL/ZZ.h>
#include <algorithm>
#include <bitset>

using namespace NTL;
using namespace std;
using std::bitset;

string ZZToBits(ZZ num, const size_t n) {
	string s = "";
	ZZ last;

	while (num != 0) {
		last = num % 2;
		if (last == 1) {
			s += '1';
		} else {
			s += '0';
		}
		num /= 2;
	}
	for (size_t i = s.length(); i < n; i++) {
		s += '0';
	}
	reverse(s.begin(), s.end());
	return s;
}

const unsigned char DERTable[] = "0123456789ABCDEF";

void PrintInDER(const string s) {
    cout << "modulus:";
    string pairstr = "";
    for (size_t i = 0; i < s.length(); i += 4) {
        int index = 0;
        
        for (size_t j = i; j < i + 4; j++) {
            index = index << 1;
            if (s[j] == '1') {
                index += 1;
            }
        }
        pairstr += DERTable[index];
        if (pairstr.length() == 2) {
            cout << pairstr;
            if (i + 4 < s.length()) {
                cout << ":";
            }
            pairstr = "";
        }
        if (i % 120 == 0) {
            cout << endl << '\t';
        }
    }
    cout << endl;
}

const string Base64Map =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

void PrintInPEM(const string s) {
	string res = "";
	size_t len = s.length() / 8;	// 比特串转化为字节的长度
	size_t i;
	unsigned char triBytes[3];		// 存储三个字节

	for (i = 0; i+3 <= len; i += 3) {
		for (int j = 0; j < 3; j++) {
			bitset<8> tmp(s.substr(8*i + 8*j, 8));
			triBytes[j] = tmp.to_ulong();
		}
		res += Base64Map[triBytes[0] >> 2];
		res += Base64Map[((triBytes[0]<<4) & 0x30) | (triBytes[1] >> 4)];
		res += Base64Map[((triBytes[1]<<2) & 0x3c) | (triBytes[2] >> 6)];
		res += Base64Map[triBytes[2] & 0x3f];
	}

	if (i < len) {
		if (len - i == 1) {
			bitset<8> tmp(s.substr(8*i, 8));
			triBytes[0] = tmp.to_ulong();
			res += Base64Map[triBytes[0] >> 2];
			res += Base64Map[(triBytes[0]<<4) & 0x30];
			res += "==";
		} else {
			for (int j = 0; j < 2; j++) {
				bitset<8> tmp(s.substr(8*i + 8*j, 8));
				triBytes[j] = tmp.to_ulong();
			}
			res += Base64Map[triBytes[0] >> 2];
			res += Base64Map[((triBytes[0]<<4) & 0x30) | (triBytes[1] >> 4)];
			res += Base64Map[(triBytes[1]<<2) & 0x3c];
			res += "=";
		}
	}

	cout << res << endl;
}

int main() {
	ZZ num(4164596496416416);
	
	string s = ZZToBits(num, 64);
	cout << s << endl;
	cout << s.length() << endl;
	PrintInDER(s);
	cout << endl;
	PrintInPEM(s);
	
	return 0;
}