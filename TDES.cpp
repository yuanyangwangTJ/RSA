/***********************************
 * Class Name: TDES
 * Function: 实现 Triple DES
 * ********************************/
#include "TDES.h"
#include <cstring>
#include <iostream>
#include <fstream>

#define ERROR -1

using namespace std;

// 构造函数
TDES::TDES() {
    keyRead();
    generateKeys();
    // for (int i = 0; i < 16; i++) {
    //     cout << sk[0][i] <<endl;
    // }
    string s = "1011000100100011010001010110011110001001101010111100110111101111";
    cout << "字符串：" << s << endl;
    bitset<64> tmp(s);
    plain = tmp;
    cout << "明文为：" << plain << endl;
    TDESEncrypt();
    cout << "密文为：" << cipher << endl;
    TDESDecrypt();
    cout << "解密为：" << plain << endl;

}

// 读取密钥
void TDES::keyRead() {
    ifstream fin("key.dat");
    if (!fin) {
        cout << "ERROR in opening 'key.dat'";
        exit(ERROR);
    }

    string stmp;
    for (int i = 0; i < 3; i++) {
        fin >> stmp;
        bitset<64> btmp(stmp);
        k[i] = btmp;
    }
}

// 对 56 位密钥的前后部分进行左移
bitset<28> TDES::leftShift(bitset<28> k, int shift) {
    bitset<28> tmp = k;
    for (int i = 27; i >= 0; i--) {
        if (i - shift < 0) {
            k[i] = tmp[i - shift + 28];
        } else {
            k[i] = tmp[i - shift];
        }
    }
    return k;
}

// 64 位密钥转换为 48 位
void TDES::key64To48(bitset<64> key, bitset<48> subkey[16]) {
    bitset<56> realKey;
    bitset<48> compressKey;
    bitset<28> left, right;

    // 去掉奇偶校验位，将 64 位密钥变为 56 位
    for (int i = 0; i < 56; i++) {
        realKey[55 - i] = key[64 - PC1[i]];
    }

    // 生成子密钥，保存在 subkey[16] 中
    for (int r = 0; r < 16; r++) {
        // 前后 28 位处理
        for (int i = 28; i < 56; i++) {
            left[i - 28] = realKey[i];
        }
        for (int i = 0; i < 28; i++) {
            right[i] = realKey[i];
        }
        // 左移
        left = leftShift(left, SHIFTS[r]);
        right = leftShift(right, SHIFTS[r]);
        // 压缩置换，由 56 位得到 48 位子密钥
        for (int i = 28; i < 56; i++) {
            realKey[i] = left[i - 28];
        }
        for (int i = 0; i < 28; i++) {
            realKey[i] = right[i];
        }
        for (int i = 0; i < 48; i++) {
            compressKey[47 - i] = realKey[56 - PC2[i]];
        }
        subkey[r] = compressKey;
    }
}

// 为每一个密钥生成 16 个子密钥，存储在 sk 中
void TDES::generateKeys() {
    for (int i = 0; i < 3; i++) {
        key64To48(k[i], sk[i]);
    }
}

// DES 的 f 函数
bitset<32> TDES::function(bitset<32> A, bitset<48> k) {
    bitset<48> EXA;
    bitset<32> RES;
    // 扩展置换，32 -> 48
    for (int i = 0; i < 48; i++) {
        EXA[47 - i] = A[32 - E[i]];
    }
    // 异或
    EXA = EXA ^ k;
    // 使用 S 盒置换
    int x = 0;
    for (int i = 0; i < 48; i += 6) {
        int row = EXA[47 - i] * 2 + EXA[47 - i - 5];
        int col = EXA[46 - i] * 8 + EXA[45 - i] * 4 + EXA[44 - i] * 2 + EXA[43 - i];
        int num = SBox[i / 6][row][col];
        bitset<4> Sbox(num);
        for (int j = 0; j < 4; j++) {
            RES[31 - x - j] = Sbox[3 - j];
        }
        x += 4;
    }
    // P 置换
    bitset<32> tmp = RES;
    for (int i = 0; i < 32; i++) {
        RES[31 - i] = tmp[32 - P[i]];
    }
    return RES;
}

// DES 加密(单次加密)，n 表示第 n 个密钥(可以为 0,1,2)
bitset<64> TDES::desEncrypt(bitset<64> plaintext, int n) {
    bitset<64> currentBits, ciphertext;
    bitset<32> left, right, newLeft;

    // 初始置换 IP
    for (int i = 0; i < 64; i++) {
        currentBits[63 - i] = plaintext[64 - IP[i]];
    }

    // 获取 Li 和 Ri
    for (int i = 32; i < 64; i++) {
        left[i - 32] = currentBits[i];
    }
    for (int i = 0; i < 32; i++) {
        right[i] = currentBits[i];
    }

    // 16 轮迭代（子密钥逆序使用）
    for (int r = 0; r < 16; r++) {
        newLeft = right;
        right = left ^ function(right, sk[n][r]);
        left = newLeft;
    }

    // 合并 L16 和 R16，合并为 R16L16
    for (int i = 0; i < 32; i++) {
        ciphertext[i] = left[i];
    }
    for (int i = 32; i < 64; i++) {
        ciphertext[i] = right[i - 32];
    }

    // 结尾置换 FP
    currentBits = ciphertext;
    for (int i = 0; i < 64; i++) {
        ciphertext[63 - i] = currentBits[64 - FP[i]];
    }
    return ciphertext;
}

// DES 解密(单次解密)，n 表示第 n 个密钥(可以为 0,1,2)
bitset<64> TDES::desDecrypt(bitset<64> ciphertext, int n) {
    bitset<64> plaintext, currentBits;
    bitset<32> left, right, newLeft;
    // 初始置换 IP
    for (int i = 0; i < 64; i++) {
        currentBits[63 - i] = ciphertext[64 - IP[i]];
    }
    // 获取 Li 和 Ri
    for (int i = 32; i < 64; i++) {
        left[i - 32] = currentBits[i];
    }
    for (int i = 0; i < 32; i++) {
        right[i] = currentBits[i];
    }

    // 16 轮迭代（子密钥逆序使用）
    for (int r = 0; r < 16; r++) {
        newLeft = right;
        right = left ^ function(right, sk[n][15 - r]);
        left = newLeft;
    }
    // 合并 L16 和 R16，合并为 R16L16
    for (int i = 0; i < 32; i++) {
        plaintext[i] = left[i];
    }
    for (int i = 32; i < 64; i++) {
        plaintext[i] = right[i - 32];
    }
    currentBits = plaintext;
    for (int i = 0; i < 64; i++) {
        plaintext[63 - i] = currentBits[64 - FP[i]];
    }
    return plaintext;
}

// TDES 加密（三重加密）
void TDES::TDESEncrypt() {
    cipher = desEncrypt(plain, 0);
    cipher = desDecrypt(cipher, 1);
    cipher = desEncrypt(cipher, 2);
}

// TDES 解密（三重解密）
void TDES::TDESDecrypt() {
    plain = desDecrypt(cipher, 2);
    plain = desEncrypt(plain, 1);
    plain = desDecrypt(plain, 0);
}
