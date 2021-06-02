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
    // GenerateKey();
    // cout << sk.p << endl;
    // cout << sk.q << endl;
    // cout << "a: " << sk.a << endl;
    // cout << pk.n << endl;
    // cout << pk.b << endl;

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
void RSAUser::EncryptMessege() {
    // 生成临时密钥
    createTempKey();
    cout << "Encrypt k = " << k << endl;
    ZZ_p::init(pk.n);
    ZZ_p k_p = to_ZZ_p(k);
    M.c1 = rep(power(k_p, pk.b));

}

// 解密信息
void RSAUser::DecryptMessege() {
    ZZ_p::init(pk.n);
    ZZ_p c1_p = to_ZZ_p(M.c1);
    k = rep(power(c1_p, sk.a));
    cout << "Decrypt k = " << k << endl;
}

// 发送信息
void RSAUser::SendMessege(RSAUser& A) {
    A.M = this->M;
}