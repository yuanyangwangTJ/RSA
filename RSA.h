#ifndef _RSA_H_
#define _RSA_H_

#include <NTL/ZZ.h>
#include <cstring>

using namespace NTL;

struct PublicKey {
    ZZ n, b;
};

struct PrivateKey {
    ZZ p, q, a;
};

struct Message {
    ZZ c1;
    std::string fileName;
};

class RSAUser {
public:
    RSAUser();
    void GenerateKey();
    void SendPublicKey(RSAUser&);
    void SendMessage(RSAUser&);
    void EncryptMessage();
    void DecryptMessage();

    PublicKey pk;
    Message M;

private:
    void createTempKey();
    void viewKey();
    PrivateKey sk;
    ZZ k;       // 临时会话密钥

};



#endif