/**************************************
 * ClassName: AES
 * Function: Implement AES
 * ***********************************/

#include "AES.h"
#include <iostream>
#include <bitset>

using std::bitset;
using namespace std;

void PrintTest(byte plain[4*4]) {
    for(int i = 0; i < 16; i++) {
		cout << hex << plain[i].to_ulong() << " ";
		if((i+1) % 4 == 0)
			cout << endl;
	}
	cout << endl;
}

// 构造函数
AES::AES() {
    cout << "AES Create.\n";

    byte key[16] = {0x2b, 0x7e, 0x15, 0x16, 
            0x28, 0xae, 0xd2, 0xa6, 
            0xab, 0xf7, 0x15, 0x88,
            0x09, 0xcf, 0x4f, 0x3c};
 
	byte plain[16] = {0x32, 0x88, 0x31, 0xe0, 
					0x43, 0x5a, 0x31, 0x37,
					0xf6, 0x30, 0x98, 0x07,
					0xa8, 0x8d, 0xa2, 0x34};

    cout << "原文：\n";
    PrintTest(plain);
    KeyExpansion(key);
    AESEncrypt(plain);
    cout << "密文：\n";
    PrintTest(plain);
    AESDecrypt(plain);
    cout << "解密文：\n";
    PrintTest(plain);

}


// SubWord 是对每四个字节使用 AES 的 S 盒
word AES::SubWord(word &w) {
    word res;
    for (int i = 0; i < 32; i += 8) {
        int row = w[i+7] * 8 + w[i+6] * 4 + w[i+5] * 2 + w[i+4];
        int col = w[i+3] * 8 + w[i+2] * 4 + w[i+1] * 2 + w[i];
        byte val = SBox[row][col];
        for (int j = 0; j < 8; j++) {
            res[i + j] = val[j];
        }
    }
    return res;
}

// RotWord 定义为一个字节循环左移
word AES::RotWord(word& w) {
    word high = w << 8;
    word low = w >> 24;
    return (high | low);
}

// 将四个字节转换为一个 word
word AES::Word(byte k1, byte k2, byte k3, byte k4) {
    word res(0x00000000), tmp;
    tmp = k1.to_ulong();
    res |= (tmp << 24);
    tmp = k2.to_ulong();
    res |= (tmp << 16);
    tmp = k3.to_ulong();
    res |= (tmp << 8); 
    tmp = k4.to_ulong();
    res |= tmp;
    return res;
}

// 密钥扩展函数，对密钥 key[] 扩展得到 roundKey[]
void AES::KeyExpansion(byte key[4*4]) {
    // N 代表以 32 位字为单位表示 key 的长度
    int N = 16 / 4, i;
    for (i = 0; i < N; i++) {
        W[i] = Word(key[4*i], key[4*i+1], key[4*i+2], key[4*i+3]);
    }

    for (; i < 4 * (Nr+1); i++) {
        if (i % N == 0) {
            word tmp = RotWord(W[i-1]);
            W[i] = W[i-N] ^ SubWord(tmp) ^ Rcon[i/N - 1];
        } else {
            W[i] = W[i-N] ^ W[i-1];
        }
    }
}

// 轮密钥和明文逐比特异位运算
void AES::AddRoundKey(byte plain[4*4], word k[4]) {
    // W[0,1,2,3] 分别与明文进行异或操作
    word k1, k2, k3, k4;
    for (int i = 0; i < 4; i++) {
        k1 = k[i] >> 24;
        k2 = (k[i] << 8) >> 24;
        k3 = (k[i] << 16) >> 24;
        k4 = (k[i] << 24) >> 24;
        plain[i] = plain[i] ^ byte(k1.to_ulong());
        plain[i+4] = plain[i+4] ^ byte(k2.to_ulong());
        plain[i+8] = plain[i+8] ^ byte(k3.to_ulong());
        plain[i+12] = plain[i+12] ^ byte(k4.to_ulong());
    }
}

// SubBytes 实现，通过查表的方式完成
void AES::SubBytes(byte bits[4*4]) {
    for (int i = 0; i < 16; i++) {
        int row = bits[i][7]*8 + bits[i][6]*4 + bits[i][5]*2 + bits[i][4];
        int col = bits[i][3]*8 + bits[i][2]*4 + bits[i][1]*2 + bits[i][0]; 
        bits[i] = SBox[row][col];
    }
}

// ShiftRows 作用在状态的行上面
void AES::ShiftRows(byte bits[4*4]) {
    // 第一行保持不变
    // 第二行循环左移一位
    byte tmp = bits[4];
    for (int i = 0; i < 3; i++) {
        bits[i+4] = bits[i+5];
    }
    bits[7] = tmp;
    // 第三行循环左移两位
    for (int i = 0; i < 2; i++) {
        tmp = bits[i+8];
        bits[i+8] = bits[i+10];
        bits[i+10] = tmp;
    }
    // 第四行循环左移三位
    tmp = bits[15];
    for (int i = 3; i > 0; i--) {
        bits[i+12] = bits[i+11];
    }
    bits[12] = tmp;
}

// 列混合的外部算法 FieldMult(2,x)
byte AES::FieldMult(byte x) {
    int t = x[7];
    byte y = x << 1;
    if (t == 1) {
        y = y ^ byte(0x1B);
    }
    return y;
}

// 列混合运算快速软件实现
void AES::MixColumns(byte bits[4*4]) {
    // 对每一列遍历
    for (int j = 0; j < 4; j++) {
        // 下面是对每一列的具体操作
        byte x[4];
        for (int i = 0; i < 4; i++) {
            x[i] = bits[4*i+j];
        }
        bits[j] = x[1] ^ x[2] ^ x[3];
        bits[j+4] = x[0] ^ x[2] ^ x[3];
        bits[j+8] = x[0] ^ x[1] ^ x[3];
        bits[j+12] = x[0] ^ x[1] ^ x[2];

        for (int i = 0; i < 4; i++) {
            x[i] = FieldMult(x[i]);
        }
        bits[j] = bits[j] ^ x[0] ^ x[1];
        bits[j+4] = bits[j+4] ^ x[1] ^ x[2];
        bits[j+8] = bits[j+8] ^ x[2] ^ x[3];
        bits[j+12] = bits[j+12] ^ x[0] ^ x[3];        
    }
}

// SubBytes 的逆操作
void AES::InvSubBytes(byte bits[4*4]) {
    // 使用解密 S 盒完成操作
    for (int i = 0; i < 16; i++) {
        int row = bits[i][7]*8 + bits[i][6]*4 + bits[i][5]*2 + bits[i][4];
        int col = bits[i][3]*8 + bits[i][2]*4 + bits[i][1]*2 + bits[i][0]; 
        bits[i] = InvSBox[row][col];
    }
}

// ShiftRows 的逆操作
void AES::InvShiftRows(byte bits[4*4]) {
    // 第一行保持不变
    // 第二行循环右移一位
    byte tmp = bits[7];
    for (int i = 3; i > 0; i--) {
        bits[i+4] = bits[i+3];
    }
    bits[4] = tmp;
    // 第三行循环右移两位
    for (int i = 0; i < 2; i++) {
        tmp = bits[i+8];
        bits[i+8] = bits[i+10];
        bits[i+10] = tmp;
    }
    // 第四行循环右移三位
    tmp = bits[12];
    for (int i = 0; i < 3; i++) {
        bits[i+12] = bits[i+13];
    }
    bits[15] = tmp;
}

// MixColumns 的逆操作算法
void AES::InvMixColumns(byte bits[4*4]) {
    // 对每一列进行遍历
    for (int j = 0; j < 4; j++) {
        // 下面是对每一列的具体操作
        byte x[4];
        for (int i = 0; i < 4; i++) {
            x[i] = bits[4*i+j];
        }
        bits[j] = x[1] ^ x[2] ^ x[3];
        bits[j+4] = x[0] ^ x[2] ^ x[3];
        bits[j+8] = x[0] ^ x[1] ^ x[3];
        bits[j+12] = x[0] ^ x[1] ^ x[2];

        for (int i = 0; i < 4; i++) {
            x[i] = FieldMult(x[i]);
        }
        bits[j] = bits[j] ^ x[0] ^ x[1];
        bits[j+4] = bits[j+4] ^ x[1] ^ x[2];
        bits[j+8] = bits[j+8] ^ x[2] ^ x[3];
        bits[j+12] = bits[j+12] ^ x[0] ^ x[3];        

        x[0] = FieldMult(x[0] ^ x[2]);
        x[1] = FieldMult(x[1] ^ x[3]);
        for (int i = 0; i < 4; i++) {
            bits[4*i+j] = bits[4*i+j] ^ x[i%2];
        }

        x[0] = FieldMult(x[0] ^ x[1]);
        for (int i = 0; i < 4; i++) {
            bits[4*i+j] = bits[4*i+j] ^ x[0];
        }
    }
}

// AES 加密
void AES::AESEncrypt(byte plain[4*4]) {
    word wk[4];     // 存储回合密钥
    // 轮密钥 0 加密
    for (int i = 0; i < 4; i++) {
        wk[i] = W[i];
    }
    AddRoundKey(plain, wk);
    // 第 1-9 轮加密
    for (int r = 1; r < Nr; r++) {
        SubBytes(plain);
        ShiftRows(plain);
        MixColumns(plain);
        for (int i = 0; i < 4; i++) {
            wk[i] = W[4*r+i];
        }
        AddRoundKey(plain, wk);

    }


    // 第 10 轮加密，少 MixColumns 操作
    SubBytes(plain);
    ShiftRows(plain);
    for (int i = 0; i < 4; i++) {
        wk[i] = W[4*Nr+i];
    }
    AddRoundKey(plain, wk);
    
}

// AES 解密
void AES::AESDecrypt(byte cipher[4*4]) {
    word wk[4];     // 存储回合密钥
    // 解密第 10 轮加密
    for (int i = 0; i < 4; i++) {
        wk[i] = W[4*Nr+i];
    }
    AddRoundKey(cipher, wk);
    InvShiftRows(cipher);
    InvSubBytes(cipher);

    // 第 9-1 轮加密的解密
    for (int r = Nr - 1; r > 0; r--) {
        for (int i = 0; i < 4; i++) {
            wk[i] = W[4*r+i];
        }
        AddRoundKey(cipher, wk);
        InvMixColumns(cipher);
        InvShiftRows(cipher);
        InvSubBytes(cipher);
    }
    // 第 0 轮加密的解密
    for (int i = 0; i < 4; i++) {
        wk[i] = W[i];
    }
    AddRoundKey(cipher, wk);
}