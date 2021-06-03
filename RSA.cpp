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

    // 因为逐字符读取文件比较慢，使用缓存区的方式一次性读取 1024 字节
    char buffer[1024];
    while (fin && !fin.eof()) {
        // 一次性读取最多 1024 字节的内容
        fin.read(buffer, 1024);
        long readNum = fin.gcount();
        
    }

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
}

// 发送信息
void RSAUser::SendMessage(RSAUser& A) {
    A.M = this->M;
}