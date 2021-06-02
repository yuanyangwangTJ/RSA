/************************************
 * ClassName: RSAUser
 * Function: 实现 RSA 加密使用者的类对象
 * *********************************/
#include "RSA.h"
#include "Random.h"
#include <iostream>
#include <ctime>
#include <NTL/ZZ.h>

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
    // 生成私钥的随机素数 p, q
    cout << "p and q are generating ...\n";
    PrimeGen G(m);
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