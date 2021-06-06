#ifndef _ENGINE_H_
#define _ENGINE_H_

/********************************
 * File Name: Engine.hpp
 * Function: 实现程序的启动运行
 * *****************************/

#include "RSA.h"
#include "conio.hpp"
#include <iostream>

using namespace std;

// 按任意键继续
void pressAnyKey() {
    cout << "--------------------------------------\n";
    cout << "Press any key to continue...\n";
    _getch();
}

// 程序运行启动
void RSAEngine() {
    const string welcome = ""\
        "--------------------------------------\n"\
        "| Welcome to RSA cryptosystem.       |\n"\
        "| Follow the guidance to experience. |\n"\
        "| 1. Create RSA Users                |\n"\
        "| 2. Build RSA key                   |\n"\
        "| 3. Encrypt files and transfer      |\n"\
        "| 4. Receive files and decrypt       |\n"\
        "--------------------------------------\n";
    cout << welcome << endl;
    pressAnyKey();
    clear();
    // 创建 RSA 使用者
    cout << "Now, firstly, create RSA users\n"
         << "Alice and Bob\n";
    pressAnyKey();

    RSAUser Alice, Bob;     // RSAUser 初始化
    cout << "RSA users have been created !\n";
    pressAnyKey();
    clear();

    // 创建 RSA 密码公钥和私钥
    cout << "Secondly, Alice build RSA key\n"
         << "public key and private key\n";
    pressAnyKey();

    Alice.GenerateKey();

    // 传输公钥
    cout << "Now Alice send public key to Bob.\n";
    Alice.SendPublicKey(Bob);
    cout << "Bob has received public key.\n";
    pressAnyKey();
    clear();

    // 加密文件
    cout << "Thirdly, Bob encrypts file and tranfers to Alice\n";
    pressAnyKey();
    Bob.EncryptMessage();
    Bob.SendMessage(Alice);
    pressAnyKey();
    clear();

    // 解密文件
    cout << "Finally, Alice decrypts file using private key\n";
    pressAnyKey();
    Alice.DecryptMessage();
    pressAnyKey();
    clear();

    cout << welcome << endl;
    cout << "All process finished !\n";
    cout << "Repeat again, y/n ?\n";
    char ch = _getch();
    switch(ch) {
        case 'Y':
        case 'y': {
            clear();
            RSAEngine();
            break;
        }
        default: break;
    }
}

#endif