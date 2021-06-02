#ifndef _RSA_H_
#define _RSA_H_

#include <NTL/ZZ.h>

using namespace NTL;

struct PublicKey {
    ZZ n, b;
};

struct PrivateKey {
    ZZ p, q, a;
};

struct Messege {
    ZZ c1;
};

class RSAUser {
public:
    RSAUser();
    void GenerateKey();
    void SendPublicKey(RSAUser&);
    void SendMessege(RSAUser&);
    void EncryptMessege();
    void DecryptMessege();

    PublicKey pk;
    Messege M;

private:
    void createTempKey();
    PrivateKey sk;
    ZZ k;       // 临时会话密钥

};



#endif