/************************************
 * ClassName: RSAUser
 * Function: 实现 RSA 加密使用者的类对象
 * *********************************/
#include "RSA.h"
#include "Random.h"
#include "AES.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

using namespace std;
using namespace NTL;

void printChoose() {
    cout << "Please choose the length of random prime: \n";
    cout << "1. 512  bits\n";
    cout << "2. 1024 bits\n";
}

RSAUser::RSAUser() {
    cout << "Create RSA user " << endl;

}

void RSAUser::GenerateKey() {
    // 选择 RSA 素数的比特长度
    int m = 8;
    
    printChoose();

    PrimeGen G(m);
    // 生成私钥的随机素数 p, q
    cout << "p and q are generating ...\n";
    do {
        sk.p = G.GeneratePrime();
        sk.q = G.GeneratePrime();
    } while (sk.p == sk.q);

    // 生成公钥 n
    pk.n = sk.p * sk.q;
    ZZ Euler = (sk.p - 1) * (sk.q - 1);    // 欧拉函数数值

    do {
        pk.b = G.GenerateRandom() % Euler;
    } while(GCD(pk.b, Euler) != 1);

    // 使用 NTL 库中的求逆函数 InvModStatus()
    InvModStatus(sk.a, pk.b, Euler);
}

// 发送 RSA 公钥
void RSAUser::SendPublicKey(RSAUser &B) {
    B.pk = this->pk;
}

// 创建临时密钥
void RSAUser::createTempKey() {
    PRNG G(2);
    k = G.GenerateRandom();
}

// 加密信息
void RSAUser::EncryptMessage() {
    // 生成临时密钥
    createTempKey();
    cout << "Encrypt k = " << k << endl;
    // 利用公钥加密 k 得到 c1，此处使用 ZZ_p 类计算
    ZZ_p::init(pk.n);
    ZZ_p k_p = to_ZZ_p(k);
    M.c1 = rep(power(k_p, pk.b));

    // 将临时密钥 k 转化为 128 比特形式
    bitset<128> key(to_ulong(k));
    // 以此创建 AES 加密系统
    AES E(key);

    // 需要加密的文件路径输入
    string fileName;
    cout << "Please input the file path: \n";
    cin >> fileName;
    // 打开文件
    ifstream fin(fileName, ios::binary);
    ofstream fout("cipher.txt", ios::binary);

    // 因为逐字符读取文件比较慢，使用缓存区的方式一次性读取 16*1024 字节
    bitset<128> buffer[1024];
    // former 记录密文 y(i-1) ，初始化为 0
    bitset<128> former(0);
    bitset<128> cipher;     // 密文

    // 开始读取需要加密的文件
    while (fin && !fin.eof()) {
        // 一次性读取最多 16*1024 字节的内容
        fin.read((char*)&buffer, 16*1024);
        streamsize readNum = fin.gcount();
        streamsize i;
        for (i = 0; i < readNum / 16; i++) {
            cipher = E.AESEncrypt(former ^ buffer[i]);
            former = cipher;
            // 写入密文至文件中
            fout.write((char*)&cipher, 16);
        }
        streamsize end = readNum % 16;
        // 文件读取结束操作，使用 PKCS7Padding 处理末尾
        if (fin.eof()) {
            // 末尾填充的数字
            bitset<128> pkcs(16 - end);
            pkcs = pkcs << 120;
            for (streamsize j = 16; j > end; j--) {
                streamsize k = j - end - 1;
                buffer[i] = ((buffer[i] << k*8) ^ pkcs) >> k*8; 
            }
            cout << buffer[i] << endl;
            cipher = E.AESEncrypt(former ^ buffer[i]);
            fout.write((char*)&cipher, 16);
        }
    }

    cout << "File encryption finished.\n";

    // 关闭文件
    fin.close();
    fout.close();
}

// 解密信息
void RSAUser::DecryptMessage() {
    // 利用密钥来解密 c1，得到临时密钥 k
    ZZ_p::init(pk.n);
    ZZ_p c1_p = to_ZZ_p(M.c1);
    k = rep(power(c1_p, sk.a));
    cout << "Decrypt k = " << k << endl;

    // 使用密钥 k 解密
    bitset<128> key(to_ulong(k));
    // 创建 AES 加密解密系统
    AES D(key);

    // 打开文件
    ifstream fin("cipher.txt", ios::binary);
    ofstream fout("2.txt", ios::binary);

    // buffer 缓冲区读取
    bitset<128> buffer[1024];
    // former 记录密文 C(i-1)，初始化为 0
    bitset<128> former(0);
    bitset<128> plain;      // 明文

    // 开始读取解密文件
    while (fin && !fin.eof()) {
        fin.read((char*)&buffer, 16*1024);
        streamsize readNum = fin.gcount();

        bool flag = false;  // 判断是否结束的标志
        if (fin.eof() || fin.peek() == EOF) {
            flag = true;
        }
        streamsize i, bufferMax = readNum / 16;
        // 使用 end 记录需要解密的缓冲区最大编号
        if (flag) bufferMax--;

        for (i = 0; i < bufferMax; i++) {
            plain = D.AESDecrypt(buffer[i]) ^ former;
            former = buffer[i];
            // 写入解密文件
            fout.write((char*)&plain, 16);
        }
        // 处理 PKCS7Padding 结尾
        if (flag) {
            // end 表示额外填充的字节数
            cout << buffer[i] << endl;
            // 最后一次解密
            plain = D.AESDecrypt(buffer[i]) ^ former;
            cout << plain << endl;
            streamsize end = (plain >> 120).to_ulong();
            cout << end << endl;
            for (streamsize j = 16; j > end; j--) {
                // 最后的字节处理
                bitset<8> text((plain << (j-1)*8 >> 120).to_ulong());
                fout.write((char*)&text, 1);
            }
        }
    }

    cout << "File dencryption finished.\n";
    fin.close();
    fout.close();

}

// 发送信息
void RSAUser::SendMessage(RSAUser& A) {
    A.M = this->M;
}