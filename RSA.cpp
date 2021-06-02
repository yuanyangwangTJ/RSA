/************************************
 * ClassName: RSAUser
 * Function: 实现 RSA 加密使用者的类对象
 * *********************************/
#include "RSA.h"
#include "Random.h"
#include <iostream>
#include <ctime>
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
    // char ch;
    // cin >> ch;
    // switch(ch){
    //     case '1': m = 8; break;
    //     case '2': m = 16; break;
    //     default:  m = 8;
    // }
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