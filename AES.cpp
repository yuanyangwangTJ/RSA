/**************************************
 * ClassName: AES
 * Function: Implement AES
 * ***********************************/

#include "AES.h"
#include <bitset>

using std::bitset;

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
        return res;
    }
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
    tmp = k1.to_ulong();
    res |= tmp;
    return res;
}

// 密钥扩展函数，对密钥 key[] 扩展得到 roundKey[]
void AES::KeyExpansion() {
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